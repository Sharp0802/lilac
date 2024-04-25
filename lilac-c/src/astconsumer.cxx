#include "astconsumer.h"

#include <clang/AST/RecursiveASTVisitor.h>

#include "enumvisitor.h"
#include "recordvisitor.h"

namespace lilac
{

    class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor>
    {
        std::vector<RecordInfo> m_Records;
        std::vector<EnumInfo> m_Enums;

    public:
        bool VisitCXXRecordDecl(clang::RecordDecl* decl)
        {
            RecordVisitor visitor(decl);
            visitor.TraverseDecl(decl);
            m_Records.push_back(visitor.GetInfo());
            return true;
        }

        bool VisitEnumDecl(clang::EnumDecl* decl)
        {
            EnumVisitor visitor(decl);
            visitor.TraverseDecl(decl);
            m_Enums.push_back(visitor.GetInfo());
            return true;
        }
    };

    ASTConsumer::ASTConsumer(clang::CompilerInstance& ci, llvm::StringRef file)
    {
    }

    void ASTConsumer::HandleTranslationUnit(clang::ASTContext& ctx)
    {
        auto* unit = ctx.getTranslationUnitDecl();
        ASTVisitor visitor;
        visitor.TraverseDecl(unit);
    }
}
