#pragma once
#include "exportattr.h"

namespace lilac::cxx
{
    [[nodiscard]]
    std::string GetName(const clang::NamedDecl* decl);

    [[nodiscard]]
    std::string GetDisplayName(const clang::NamedDecl* decl);

    [[nodiscard]]
    std::string GetTypeString(clang::QualType decl);

    [[nodiscard]]
    clang::FunctionDecl* FindMethod(const clang::CXXRecordDecl* decl, const std::string& name);
}
