#include "irreader.h"

#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

namespace lilac::core
{
    std::shared_ptr<llvm::Module> IRReader::Read(
        const std::string& executable,
        const std::string& filepath)
    {
        llvm::LLVMContext  context;
        llvm::SMDiagnostic err;

        auto module = parseIRFile(filepath, err, context);
        if (!module)
        {
            err.print(executable.data(), llvm::errs());
            return nullptr;
        }

        return std::move(module);
    }
}
