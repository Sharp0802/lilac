#pragma once

#include "pch.h"
#include "context.h"

namespace lilac::cxx
{
    class LilacCodeGen final
    {
        LilacContext& m_Context;
        llvm::Module& m_Module;

    public:
        LilacCodeGen(LilacContext& context, llvm::Module& module);

        bool Execute();
    };
}
