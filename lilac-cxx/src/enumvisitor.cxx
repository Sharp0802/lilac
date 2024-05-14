#include "enumvisitor.h"

namespace lilac::cxx
{
    std::string GetActualName(const clang::EnumDecl* decl)
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
            "enum.{}{}",
            ss.str(),
            decl->getName().str());
    }

    Visitor<clang::EnumDecl>::Visitor(clang::EnumDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Type, GetActualName(decl), decl->getNameAsString())
    {
        const auto type = decl->getIntegerType();
        const auto size = decl->getASTContext().getTypeSize(type) / 8;

        m_Hierarchy.GetConstantData().Signed = type->isSignedIntegerType();
        m_Hierarchy.GetConstantData().Size = size;
    }

    bool Visitor<clang::EnumDecl>::VisitEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        core::Hierarchy h{
            core::HOK_Constant,
            decl->getNameAsString(),
            decl->getNameAsString()
        };
        h.GetConstantData() = m_Hierarchy.GetConstantData();
        if (h.GetConstantData().Signed)
            h.GetConstantData().ConstantSigned = decl->getValue().getSExtValue();
        else
            h.GetConstantData().ConstantUnsigned = decl->getValue().getZExtValue();
        m_Hierarchy.Members.emplace(h);

        return true;
    }

    core::Hierarchy Visitor<clang::EnumDecl>::GetHierarchy() const
    {
        return m_Hierarchy;
    }
}
