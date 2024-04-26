#ifndef ASTCONSUMER_H
#define ASTCONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>

#include "astfrontendaction.h"

namespace lilac::cxx
{
    class ASTConsumer : public clang::ASTConsumer
    {
        ASTFrontendAction* m_Action;
        clang::CompilerInstance& m_Compiler;
        llvm::StringRef m_File;

    public:
        ASTConsumer(ASTFrontendAction* action, clang::CompilerInstance& ci, llvm::StringRef file);

        void HandleTranslationUnit(clang::ASTContext& ctx) override;
    };
}

#endif //ASTCONSUMER_H
