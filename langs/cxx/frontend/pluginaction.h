#pragma once

#include "pch.h"

namespace lilac::cxx
{
    frxml::dom& GetDOMRoot();

    class LilacAction final : public clang::ASTFrontendAction
    {
    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;
    };

    class LilacASTConsumer final : public clang::SemaConsumer
    {
        clang::Sema* m_Sema = nullptr;
        frxml::dom& m_Root;

    public:
        explicit LilacASTConsumer(frxml::dom& dom);

        void InitializeSema(clang::Sema& sema) override;

        void HandleTranslationUnit(clang::ASTContext& context) override;
    };

    class LilacASTVisitor final : public clang::RecursiveASTVisitor<LilacASTVisitor>
    {
        using Level = clang::DiagnosticsEngine::Level;

        clang::Sema&              m_Sema;
        clang::DiagnosticsEngine& m_Diag;

        frxml::dom& m_Root;

        frxml::dom* GetNamespaceDOM(clang::NamedDecl* decl) const;

        bool IsDuplicated(clang::NamedDecl* decl, const std::string& tag);

    public:
        LilacASTVisitor(clang::Sema& sema, frxml::dom& dom);

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

        class CXXRecordVisitor : public RecursiveASTVisitor<CXXRecordVisitor>
        {
            std::function<void(clang::CXXMethodDecl*)> m_Delegate;

        public:
            CXXRecordVisitor(std::function<void(clang::CXXMethodDecl*)> delegate);

            bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl);
        };
    };
}
