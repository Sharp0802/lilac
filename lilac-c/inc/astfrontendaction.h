#ifndef ASTFRONTENDACTION_H
#define ASTFRONTENDACTION_H

#include <clang/Frontend/FrontendAction.h>

#include "astvisitor.h"
#include "frontendactionfactory.h"

namespace lilac
{
    class ASTFrontendAction : public clang::ASTFrontendAction
    {
        FrontendActionFactory* m_Owner;

    public:
        explicit ASTFrontendAction(FrontendActionFactory* owner);

        void ReportASTInfo(llvm::StringRef file, const ASTInfo& info) const;

    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance& ci,
            llvm::StringRef file) override;
    };
}

#endif //ASTFRONTENDACTION_H
