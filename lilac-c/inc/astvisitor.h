#ifndef ASTVISITOR_H
#define ASTVISITOR_H

#include "enumvisitor.h"
#include "recordvisitor.h"

namespace lilac
{
    struct ASTInfo
    {
        std::vector<RecordInfo> Records;
        std::vector<EnumInfo> Enums;
        std::vector<clang::FunctionDecl*> Functions;
    };

    class ASTVisitor final
            : public clang::RecursiveASTVisitor<ASTVisitor>,
              public VisitorBase<ASTInfo>
    {
        std::vector<RecordInfo> m_Records;
        std::vector<EnumInfo> m_Enums;
        std::vector<clang::FunctionDecl*> m_Functions;

    public:
        bool VisitCXXRecordDecl(clang::RecordDecl* decl);

        bool VisitEnumDecl(clang::EnumDecl* decl);

        bool VisitFunctionDecl(clang::FunctionDecl* decl);

        [[nodiscard]]
        ASTInfo GetInfo() const override;
    };
}

#endif //ASTVISITOR_H
