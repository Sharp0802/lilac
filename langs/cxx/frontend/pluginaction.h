/*
 * Copyright (C) 2024  Yeong-won Seo
 *
 * This file is part of LILAC.
 *
 * LILAC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3, or (at your option) any later
 * version.
 *
 * LILAC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "pch.h"

namespace lilac::cxx
{
    frxml::dom& GetDOMRoot();

    /**
     * @brief A frontend action that creates lilac::cxx::LilacASTConsumer to traverse AST
     */
    class LilacAction final : public clang::ASTFrontendAction
    {
    public:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;
    };

    /**
     * @brief An ASTConsumer that creates lilac::cxx::LilacASTVisitor to traverse AST
     *
     * An ASTConsumer that creates lilac::cxx::LilacASTVisitor to traverse AST.
     * It injects DOM object, for interface serializing,
     * and clang::Sema because clang::RecursiveVisitor cannot retrieve
     * clang::Sema from compilation itself.
     */
    class LilacASTConsumer final : public clang::SemaConsumer
    {
        clang::Sema* m_Sema = nullptr;
        frxml::dom&  m_Root;

    public:
        explicit LilacASTConsumer(frxml::dom& dom);

        void InitializeSema(clang::Sema& sema) override;

        void HandleTranslationUnit(clang::ASTContext& context) override;
    };

    /**
     * @brief A recursive AST visitor that serializes C/C++ interface data into DOM object
     *
     * A recursive AST visitor that serializes C/C++ interface data into DOM object.
     * DOM object and clang::Sema should be injected to serialize data properly.
     */
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


        /**
         * @brief A recursive AST visitor that traverses AST of an enumeration.
         *
         * A recursive AST visitor that traverses AST of an enumeration.
         * Using passed callback, A caller can retrieve enumeration constant
         * data of the enumeration.
         */
        class EnumVisitor : public RecursiveASTVisitor<EnumVisitor>
        {
            std::function<void(clang::EnumConstantDecl*)> m_Delegate;

        public:
            EnumVisitor(std::function<void(clang::EnumConstantDecl* constant)>);

            bool TraverseEnumConstantDecl(clang::EnumConstantDecl* decl);
        };

        /**
         * @brief A recursive AST visitor that traverses AST of an enumeration.
         *
         * A recursive AST visitor that traverses AST of an enumeration.
         * Using passed callback, A caller can retrieve method data of the enumeration.
         */
        class CXXRecordVisitor : public RecursiveASTVisitor<CXXRecordVisitor>
        {
            std::function<void(clang::CXXMethodDecl*)> m_Delegate;

        public:
            CXXRecordVisitor(std::function<void(clang::CXXMethodDecl*)> delegate);

            bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl);
        };
    };
}
