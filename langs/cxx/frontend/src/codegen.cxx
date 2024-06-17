#include "codegen.h"

namespace lilac::cxx
{
    LilacCodeGen::LilacCodeGen(
        LilacContext& context,
        llvm::Module& module)
        : m_Context(context),
          m_Module(module)
    {
    }

    bool LilacCodeGen::Execute()
    {
        return true;
    }
}
