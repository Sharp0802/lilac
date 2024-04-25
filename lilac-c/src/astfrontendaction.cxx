#include "astfrontendaction.h"

#include "astconsumer.h"

namespace lilac
{
    ASTFrontendAction::ASTFrontendAction(FrontendActionFactory* owner): m_Owner(owner)
    {
    }

    void ASTFrontendAction::ReportASTInfo(const llvm::StringRef file, const ASTInfo& info) const
    {
        m_Owner->ReportASTInfo(file, info);
    }

    std::unique_ptr<clang::ASTConsumer> ASTFrontendAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef file)
    {
        return std::make_unique<ASTConsumer>(this, ci, file);
    }
}
