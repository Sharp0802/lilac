#include <utility>

#include "visitor.h"

#include <fstream>

#include "cxxrecordvisitor.h"
#include "enumvisitor.h"
#include "functionvisitor.h"

namespace lilac::cxx
{
    bool IsExported(const clang::NamedDecl* decl)
    {
        return decl->getVisibility() == clang::DefaultVisibility;
    }

    bool IsExported(const clang::FunctionDecl* decl)
    {
        return
            IsExported(clang::cast<clang::NamedDecl>(decl)) &&
            decl->getAccess() == clang::AS_public;
    }

    ASTVisitor::ASTVisitor(): m_Hierarchy(core::HOK_Root, "%root", "%root")
    {
    }

    core::Hierarchy* TraverseHierarchy(core::Hierarchy* root, clang::DeclContext* decl)
    {
        // Traverse namespaces
        std::stack<clang::NamespaceDecl*> nss;
        for (auto parent = decl->getParent(); ; parent = parent->getParent())
        {
            if (parent->isTranslationUnit())
                break;
            if (!parent->isNamespace() || parent->isInlineNamespace())
                return nullptr;

            nss.push(clang::cast<clang::NamespaceDecl>(parent));
        }

        // Ensure hierarchy
        auto* cursor = root;
        while (!nss.empty())
        {
            const auto ns = nss.top();
            nss.pop();

            // Query Hierarchy for namespace; If not exists, create one.
            const core::Hierarchy* next = nullptr;
            for (auto& member: cursor->Members)
            {
                if (member.Kind != core::HOK_Namespace)
                    continue;
                if (member.ActualName == ns->getNameAsString())
                    next = &member;
            }
            if (!next)
            {
                auto [e, _] = cursor->Members.emplace(
                    core::HOK_Namespace,
                    ns->getNameAsString(),
                    ns->getNameAsString());
                next = &*e;
            }

            // Move to next
            cursor = const_cast<core::Hierarchy*>(next);
        }

        return cursor;
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    template<typename T>
    bool VisitDecl(core::Hierarchy* root, T* decl)
    {
        if (!IsExported(decl)) return true;

        auto* parent = TraverseHierarchy(root, decl);
        if (!parent) return true;

        Visitor<T> visitor{ decl };
        const auto r = visitor.TraverseDecl(decl);
        parent->Members.emplace(visitor.GetHierarchy());

        return r;
    }

    bool ASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl) // NOLINT(*-convert-member-functions-to-static)
    {
        return cxx::VisitDecl(&m_Hierarchy, decl);
    }

    bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* decl) // NOLINT(*-convert-member-functions-to-static)
    {
        return cxx::VisitDecl(&m_Hierarchy, decl);
    }

    bool ASTVisitor::VisitFunctionDecl(clang::FunctionDecl* decl) // NOLINT(*-convert-member-functions-to-static)
    {
        return cxx::VisitDecl(&m_Hierarchy, decl);
    }

    core::Hierarchy ASTVisitor::GetHierarchy() const
    {
        return m_Hierarchy;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    ASTConsumer::ASTConsumer(std::string out) : m_Out(std::move(out))
    {
    }

    void ASTConsumer::HandleTranslationUnit(clang::ASTContext& ctx)
    {
        ASTVisitor visitor{};
        visitor.TraverseTranslationUnitDecl(ctx.getTranslationUnitDecl());

        std::ofstream ofs(m_Out);
        if (ofs.bad())
        {
            llvm::errs() << "Couldn't open file '" << m_Out << "'.";
            return;
        }

        ofs << visitor.GetHierarchy().ToString();
    }

    std::unique_ptr<clang::ASTConsumer> ASTFrontendAction::CreateASTConsumer(
        clang::CompilerInstance&,
        llvm::StringRef)
    {
        return std::make_unique<ASTConsumer>(m_Out);
    }

    ASTFrontendAction::ASTFrontendAction(std::string out) : m_Out(std::move(out))
    {
    }

    std::unique_ptr<clang::FrontendAction> FrontendActionFactory::create()
    {
        return std::make_unique<ASTFrontendAction>(m_Out);
    }

    FrontendActionFactory::FrontendActionFactory(std::string out) : m_Out(std::move(out))
    {
    }
}
