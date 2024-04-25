#ifndef FRONTENDACTIONFACTORY_H
#define FRONTENDACTIONFACTORY_H

#include <clang/Tooling/Tooling.h>

#include "astvisitor.h"

namespace lilac
{
    class FrontendActionFactory : public clang::tooling::FrontendActionFactory
    {
    public:
        FrontendActionFactory();

        void ReportASTInfo(llvm::StringRef file, const ASTInfo& info);

        std::unique_ptr<clang::FrontendAction> create() override;
    };
}

#endif //FRONTENDACTIONFACTORY_H
