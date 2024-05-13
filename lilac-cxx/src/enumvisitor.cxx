#include "enumvisitor.h"

namespace lilac::cxx
{
    Visitor<clang::EnumDecl>::Visitor(clang::EnumDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Type, decl->getNameAsString(), decl->getNameAsString())
    {
        m_Hierarchy.Constant.Signed = decl->getIntegerType()->isSignedIntegerType();
    }

    bool Visitor<clang::EnumDecl>::VisitEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        core::Hierarchy h{
            core::HOK_Constant,
            decl->getNameAsString(),
            decl->getNameAsString()
        };
        if ((h.Constant.Signed = m_Hierarchy.Constant.Signed))
            h.Constant.ConstantSigned = decl->getValue().getSExtValue();
        else
            h.Constant.ConstantUnsigned = decl->getValue().getZExtValue();
        m_Hierarchy.Members.emplace(h);

        return true;
    }

    core::Hierarchy Visitor<clang::EnumDecl>::GetHierarchy() const
    {
        return m_Hierarchy;
    }
}
