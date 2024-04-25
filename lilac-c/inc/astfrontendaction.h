#ifndef ASTFRONTENDACTION_H
#define ASTFRONTENDACTION_H

#include <clang/Frontend/FrontendAction.h>

#include "frontendactionfactory.h"

namespace lilac
{
    class ASTFrontendAction : public clang::ASTFrontendAction
    {
        FrontendActionFactory* m_Owner;

    public:
        ASTFrontendAction(FrontendActionFactory* owner);

    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance& ci,
            llvm::StringRef file) override;
    };
}

#endif //ASTFRONTENDACTION_H
