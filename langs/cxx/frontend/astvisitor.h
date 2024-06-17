#pragma once

#include "pch.h"
#include "astvisitor.h"
#include "context.h"

namespace lilac::cxx
{
    class LilacASTVisitor final
        : public clang::RecursiveASTVisitor<LilacASTVisitor>
    {
        LilacContext* m_Context = nullptr;
        clang::Sema*  m_Sema    = nullptr;

    public:
        explicit LilacASTVisitor(LilacContext& context);

        /**************************************************************
         * clang::Sema doesn't initialized when LilacASTVisitor       *
         * initializing because clang::Sema is initialized in         *
         * InitializeSema() in LilacASTConsumer (derived from         *
         * clang::SemaConsumer)                                       *
         **************************************************************/
        void SetSema(clang::Sema& sema);

        /**************************************************************
         * Stubs from clang::RecursiveASTVisitor                      *
         **************************************************************/
        bool VisitFunctionDecl(clang::FunctionDecl* decl);

        bool VisitEnumDecl(clang::EnumDecl* decl);
    };
}
