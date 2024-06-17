#pragma once

#include "pch.h"

namespace lilac::cxx
{
    struct LilacContext
    {
        std::vector<clang::FunctionDecl*> Functions;
        std::vector<clang::TagDecl*>      Types;
    };
}
