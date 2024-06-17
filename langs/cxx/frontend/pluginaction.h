#pragma once

#include "astvisitor.h"
#include "context.h"

namespace lilac::cxx
{
    class LilacAction final : public clang::PluginASTAction
    {
        LilacContext m_Context;

    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;

        bool ParseArgs(const clang::CompilerInstance&, const std::vector<std::string>&) override;

        void EndSourceFileAction() override;

        ActionType getActionType() override;
    };

    class LilacASTConsumer final : public clang::SemaConsumer
    {
        LilacASTVisitor m_Visitor;

    public:
        explicit LilacASTConsumer(LilacContext& context);

        void InitializeSema(clang::Sema& sema) override;

        void HandleTranslationUnit(clang::ASTContext& context) override;
    };
}
