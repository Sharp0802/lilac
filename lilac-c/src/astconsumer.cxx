#include "astconsumer.h"

#include <clang/AST/RecursiveASTVisitor.h>

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

        bool VisitFunctionDecl(clang::FunctionDecl* decl)
        {
            if (IsVisibilityAllowed(decl))
                m_Functions.push_back(decl);
            return true;
        }

        [[nodiscard]]
        ASTInfo GetInfo() const override
        {
            return {
                m_Records,
                m_Enums,
                m_Functions
            };
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
