#include "frontendactionfactory.h"

#include "astfrontendaction.h"

namespace lilac::cxx
{
    FrontendActionFactory::FrontendActionFactory() = default;

    void FrontendActionFactory::ReportASTInfo(const llvm::StringRef file, const ASTInfo& info)
    {
    }

    std::unique_ptr<clang::FrontendAction> FrontendActionFactory::create()
    {
        return std::make_unique<ASTFrontendAction>(this);
    }
}
