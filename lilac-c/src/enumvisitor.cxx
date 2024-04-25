#include "enumvisitor.h"

namespace lilac
{
    EnumVisitor::EnumVisitor(clang::EnumDecl* decl): m_Enum(decl)
    {
    }

    bool EnumVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl* constant)
    {
        if (IsVisibilityAllowed(constant))
        {
            m_Constant.push_back(constant);
            llvm::outs() << m_Enum->getName() << "::" << constant->getName() << '\n';
        }
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
