#include "astconsumer.h"

#include <clang/AST/RecursiveASTVisitor.h>

#include "astvisitor.h"

namespace lilac
{

    ASTConsumer::ASTConsumer(ASTFrontendAction* action, clang::CompilerInstance& ci, llvm::StringRef file)
        : m_Action(action),
          m_Compiler(ci),
          m_File(file)
    {
    }

    void ASTConsumer::HandleTranslationUnit(clang::ASTContext& ctx)
    {
        auto* unit = ctx.getTranslationUnitDecl();
        ASTVisitor visitor;
        visitor.TraverseDecl(unit);
        m_Action->ReportASTInfo(m_File, visitor.GetInfo());
    }
}
