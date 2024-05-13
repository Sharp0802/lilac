#include <clang/AST/Mangle.h>

#include "cxxrecordvisitor.h"
#include "functionvisitor.h"

namespace lilac::cxx
{
    std::string GetActualName(const clang::CXXRecordDecl* decl)
    {
        std::stack<std::string> ns;
        for (auto parent = decl->getParent(); !parent->isTranslationUnit(); parent = parent->getParent())
            ns.push(llvm::cast<clang::NamespaceDecl>(parent)->getName().str());
        std::stringstream ss;
        while (!ns.empty())
        {
            ss << ns.top() << "::";
            ns.pop();
        }

        return std::format(
            "{}.{}{}",
            decl->isStruct() ? "struct" : "class",
            ss.str(),
            decl->getName().str());
    }

    Visitor<clang::CXXRecordDecl>::Visitor(clang::CXXRecordDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Type, GetActualName(decl), decl->getName().str())
    {
    }

    clang::CXXMethodDecl* FindMethod(
        const std::vector<clang::CXXMethodDecl*>& methods,
        const std::string&                        name,
        const std::string&                        error)
    {
        clang::CXXMethodDecl* target = nullptr;
        for (const auto method: methods)
        {
            if (method->getName() != name)
                continue;
            if (target)
            {
                llvm::errs() << error << '\n';
                return nullptr;
            }
            target = method;
        }
        return target;
    }

    bool Visitor<clang::CXXRecordDecl>::VisitMSPropertyDecl(clang::MSPropertyDecl* decl)
    {
        if (!IsExported(decl))
            return true;

        const auto* getter = decl->hasGetter()
            ? FindMethod(
                m_Methods,
                decl->getGetterId()->getName().str(),
                std::format(
                    "Ambiguous getter name for {}; will be skipped.",
                    decl->getQualifiedNameAsString()))
            : nullptr;
        const auto* setter = decl->hasSetter()
            ? FindMethod(
                m_Methods,
                decl->getSetterId()->getName().str(),
                std::format(
                    "Ambiguous setter name for {}; will be skipped.",
                    decl->getQualifiedNameAsString()))
            : nullptr;
        if (!getter && !setter)
        {
            llvm::errs() << "A property must have either a getter or a setter; will be skipped.\n";
            return false;
        }

        core::Hierarchy property
        {
            core::HOK_Property,
            decl->getNameAsString(),
            decl->getNameAsString()
        };

        if (getter)
            property.Members.emplace(core::HOK_Getter, GetActualName(getter), getter->getNameAsString());
        if (setter)
            property.Members.emplace(core::HOK_Setter, GetActualName(setter), setter->getNameAsString());

        m_Hierarchy.Members.emplace(property);

        return true;
    }

    bool Visitor<clang::CXXRecordDecl>::VisitCXXMethodDecl(clang::CXXMethodDecl* decl)
    {
        if (decl->getIdentifier() && IsExported(decl))
        {
            m_Methods.push_back(decl);
            m_Hierarchy.Members.emplace(
                decl->isStatic() ? core::HOK_Function : core::HOK_Method,
                GetActualName(decl),
                decl->getNameAsString());
        }
        return true;
    }

    core::Hierarchy Visitor<clang::CXXRecordDecl>::GetHierarchy() const
    {
        return m_Hierarchy;
    }
}
