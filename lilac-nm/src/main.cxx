#include "pch.h"

#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"

#include "langs/cxx/frontend/exportattr.h"

namespace
{
    llvm::cl::OptionCategory s_Category{
        "Link Options"
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
        os << "lilac-nm (https://github.com/Sharp0802/lilac)\n";
    });
    llvm::cl::HideUnrelatedOptions(s_Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    llvm::LLVMContext  context;
    llvm::SMDiagnostic err;

    auto module = parseIRFile(s_IRSource, err, context);
    if (!module)
    {
        err.print(argv[0], llvm::errs());
        llvm::errs()
            << "Couldn't read IR from file '" << s_IRSource << "'.\n";
        return 1;
    }

    for (
        auto* ca = llvm::cast<llvm::ConstantArray>(
            module->getGlobalVariable("llvm.global.annotations")->getOperand(0));
        auto& use: ca->operands())
    {
        const auto* cs = llvm::cast<llvm::ConstantStruct>(use.get());
        if (cs->getNumOperands() < 2)
            continue;

        if (const auto* fn = llvm::dyn_cast<llvm::Function>(cs->getOperand(0)))
        {
            unsigned i;
            for (i = 1; i < cs->getNumOperands(); ++i)
            {
                auto* var = llvm::dyn_cast<llvm::GlobalVariable>(cs->getOperand(i));
                if (!var)
                    continue;
                if (llvm::dyn_cast<llvm::ConstantDataArray>(var->getInitializer())->getAsCString()
                    == lilac::cxx::ExportAttrInfo::AttrMangling)
                    break;
            }
            if (i < cs->getNumOperands())
                llvm::errs() << fn->getName() << "\n";
        }
    }

    return 0;
}
