#include "astvisitor.h"

namespace lilac::cxx
{
    bool ASTVisitor::VisitCXXRecordDecl(clang::RecordDecl* decl)
    {
        RecordVisitor visitor(decl);
        visitor.TraverseDecl(decl);
        m_Records.push_back(visitor.GetInfo());
        return true;
    }

    bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* decl)
    {
        EnumVisitor visitor(decl);
        visitor.TraverseDecl(decl);
        m_Enums.push_back(visitor.GetInfo());
        return true;
    }

    bool ASTVisitor::VisitFunctionDecl(clang::FunctionDecl* decl)
    {
        if (IsVisibilityAllowed(decl))
            m_Functions.push_back(decl);
        return true;
    }

    ASTInfo ASTVisitor::GetInfo() const
    {
        return {
            m_Records,
            m_Enums,
            m_Functions
        };
    }
}
