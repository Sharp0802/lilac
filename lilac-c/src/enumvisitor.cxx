#include "enumvisitor.h"

namespace lilac::cxx
{
    EnumVisitor::EnumVisitor(clang::EnumDecl* decl): m_Enum(decl)
    {
    }

    bool EnumVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl* constant)
    {
        if (IsVisibilityAllowed(constant))
            m_Constant.push_back(constant);
        return true;
    }

    EnumInfo EnumVisitor::GetInfo() const
    {
        return {
            m_Enum,
            m_Constant
        };
    }
}
