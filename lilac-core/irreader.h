#pragma once

#include <llvm/IR/Module.h>

#include <memory>

namespace lilac::core
{
    class IRReader
    {
    public:
        static std::shared_ptr<llvm::Module> Read(
            const std::string& executable,
            const std::string& filepath);
    };
}
