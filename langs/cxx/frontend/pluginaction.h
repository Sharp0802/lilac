#pragma once

#include "pch.h"

namespace lilac::cxx
{
    class LilacAction final : public clang::PluginASTAction
    {
    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;

        ActionType getActionType() override;

        bool ParseArgs(const clang::CompilerInstance& CI, const std::vector<std::string>& arg) override;
    };

    class LilacASTConsumer final : public clang::SemaConsumer
    {
    public:
        void InitializeSema(clang::Sema& sema) override;

        void HandleTranslationUnit(clang::ASTContext& context) override;
    };

    class LilacASTVisitor final : public clang::RecursiveASTVisitor<LilacASTVisitor>
    {
        std::string m_OutputFilename;

    public:
        LilacASTVisitor();

        ~LilacASTVisitor();

        bool TraverseEnumDecl(clang::EnumDecl* decl);

        bool TraverseCXXRecordDecl(clang::CXXRecordDecl* decl);

        bool TraverseFunctionDecl(clang::FunctionDecl* decl);

        class EnumVisitor : public RecursiveASTVisitor<EnumVisitor>
        {
            std::function<void(clang::EnumConstantDecl*)> m_Delegate;

        public:
            EnumVisitor(std::function<void(clang::EnumConstantDecl* constant)>);

            bool TraverseEnumConstantDecl(clang::EnumConstantDecl* decl);
        };
    };
}
