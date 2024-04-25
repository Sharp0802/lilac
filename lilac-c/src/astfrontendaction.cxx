#include "astfrontendaction.h"

#include "astconsumer.h"

namespace lilac
{
    ASTFrontendAction::ASTFrontendAction(FrontendActionFactory* owner): m_Owner(owner)
    {
    }

    std::unique_ptr<clang::ASTConsumer> ASTFrontendAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef file)
    {
        return std::make_unique<ASTConsumer>(ci, file);
    }
}
