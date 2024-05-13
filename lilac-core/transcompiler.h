#pragma once

#include <llvm/IR/Module.h>
#include <memory>

namespace lilac::core
{
    enum class TranscompilerID
    {
        MSIL
    };

    template<TranscompilerID TCID>
    class BackendImpl;

    template<TranscompilerID TCID>
    struct BackendOptions;

    class BackendBase
    {
    public:
        virtual ~BackendBase() = default;

        virtual bool Process(std::ofstream& ofs) = 0;
    };

    using Backend = std::shared_ptr<BackendBase>;

    template<TranscompilerID TCID>
    Backend CreateBackend(
        std::shared_ptr<llvm::Module> module,
        BackendOptions<TCID> options);

    template<TranscompilerID TCID>
    class FrontendImpl;

    template<TranscompilerID TCID>
    struct FrontendOptions;

    class FrontendBase
    {
    public:
        virtual ~FrontendBase();

        virtual bool Process(std::ofstream& ofs) = 0;
    };

    using Frontend = std::shared_ptr<FrontendBase>;

    template<TranscompilerID TCID>
    Frontend CreateFrontend();
}
