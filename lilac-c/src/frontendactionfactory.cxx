#include "frontendactionfactory.h"

#include "astfrontendaction.h"

namespace lilac
{
    FrontendActionFactory::FrontendActionFactory() = default;

    std::unique_ptr<clang::FrontendAction> FrontendActionFactory::create()
    {
        return std::make_unique<ASTFrontendAction>();
    }
}
