#ifndef ENUMVISITOR_H
#define ENUMVISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>

#include "visitorbase.h"

namespace lilac
{
    struct EnumInfo
    {
        clang::EnumDecl* Handle;
        std::vector<clang::EnumConstantDecl*> Constant;
    };

    class EnumVisitor
            : public clang::RecursiveASTVisitor<EnumVisitor>,
              public VisitorBase<EnumInfo>
    {
        clang::EnumDecl* m_Enum;
        std::vector<clang::EnumConstantDecl*> m_Constant;

    public:
        explicit EnumVisitor(clang::EnumDecl* decl);

        bool VisitEnumConstantDecl(clang::EnumConstantDecl* constant);

        [[nodiscard]]
        EnumInfo GetInfo() const override;
    };
}

#endif //ENUMVISITOR_H
