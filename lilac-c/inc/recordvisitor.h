#ifndef RECORDVISITOR_H
#define RECORDVISITOR_H
#include <clang/AST/RecursiveASTVisitor.h>

#include "visitorbase.h"

namespace lilac
{
    struct RecordInfo
    {
        clang::RecordDecl* Handle;
        std::vector<clang::MSPropertyDecl*> Properties;
        std::vector<clang::CXXMethodDecl*> Methods;
    };

    class RecordVisitor
            : public clang::RecursiveASTVisitor<RecordVisitor>,
              public VisitorBase<RecordInfo>
    {
        clang::RecordDecl* m_Record;
        std::vector<clang::MSPropertyDecl*> m_Properties;
        std::vector<clang::CXXMethodDecl*> m_Methods;

    public:
        explicit RecordVisitor(clang::RecordDecl* record);

        bool VisitMSPropertyDecl(clang::MSPropertyDecl* property);

        bool VisitCXXMethodDecl(clang::CXXMethodDecl* method);

        [[nodiscard]]
        RecordInfo GetInfo() const override;
    };
}

#endif //RECORDVISITOR_H
