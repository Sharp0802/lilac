#include "enumvisitor.h"

namespace lilac::cxx
{
    Visitor<clang::EnumDecl>::Visitor(clang::EnumDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Type, decl->getNameAsString(), decl->getNameAsString())
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
