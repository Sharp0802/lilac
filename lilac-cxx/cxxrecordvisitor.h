#pragma once

#include "pch.h"
#include "visitor.h"
#include "lilac-core/hierarchy.h"

namespace lilac::cxx
{
    template<>
    class Visitor<clang::CXXRecordDecl> : public clang::RecursiveASTVisitor<Visitor<clang::CXXRecordDecl>>
    {
        clang::CXXRecordDecl* m_Decl;
        std::vector<clang::CXXMethodDecl*> m_Methods;

        core::Hierarchy m_Hierarchy;

    public:
        explicit Visitor(clang::CXXRecordDecl* decl);

        bool VisitMSPropertyDecl(clang::MSPropertyDecl* decl);

        bool VisitCXXMethodDecl(clang::CXXMethodDecl* decl);

        [[nodiscard]]
        core::Hierarchy GetHierarchy() const;
    };
}
