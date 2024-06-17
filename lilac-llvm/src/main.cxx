#include "pch.h"
#include "version.h"
#include "lilac-core/hierarchy.h"

#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"

namespace
{
    llvm::cl::OptionCategory s_Category{
        "Link Options"
    };

    llvm::cl::opt<std::string> s_Out{
        llvm::cl::Positional,
        llvm::cl::desc("<output>"),
        llvm::cl::cat(s_Category),
        llvm::cl::Required
    };

    llvm::cl::opt<std::string> s_IRSource{
        llvm::cl::Positional,
        llvm::cl::desc("<IR file>"),
        llvm::cl::cat(s_Category),
        llvm::cl::Required
    };
}

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os)
    {
        os << "lilac-llvm " VERSION " " TIMESTAMP "\n";
    });
    llvm::cl::HideUnrelatedOptions(s_Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    llvm::LLVMContext  context;
    llvm::SMDiagnostic err;

    auto module = parseIRFile(s_IRSource, err, context);
    if (!module)
    {
        err.print(argv[0], llvm::errs());
        llvm::errs() << "Couldn't read IR from file '" << s_IRSource << "'.\n";
        return 1;
    }

    for (auto& global: module->globals())
    {
        if (global.getName() == "llvm.global.annotations")
        {
            auto* ca = llvm::cast<llvm::ConstantArray>(global.getOperand(0));
            for (auto& use: ca->operands())
            {
                auto* cs = llvm::cast<llvm::ConstantStruct>(use.get());
                if (cs->getNumOperands() < 2)
                    continue;

                auto* fn = llvm::dyn_cast<llvm::Function>(cs->getOperand(0));
                if (!fn)
                    continue;

                llvm::errs() << fn->getName();
                for (auto i = 1; i < cs->getNumOperands(); ++i)
                {
                    auto* var   = llvm::dyn_cast<llvm::GlobalVariable>(cs->getOperand(i));
                    if (!var)
                        continue;

                    auto  annot = llvm::dyn_cast<llvm::ConstantDataArray>(var->getInitializer())->getAsCString();

                    llvm::errs() << "," << annot;
                }
                llvm::errs() << "\n";
            }
        }
    }

    std::ofstream ofs(s_Out);

    return 0;
}
