#ifndef ASTCONSUMER_H
#define ASTCONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>

namespace lilac
{
    class ASTConsumer : public clang::ASTConsumer
    {
    public:
        ASTConsumer(clang::CompilerInstance& ci, llvm::StringRef file);

        void HandleTranslationUnit(clang::ASTContext& ctx) override;
    };
}

#endif //ASTCONSUMER_H
