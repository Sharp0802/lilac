#include "pch.h"
#include "version.h"
#include "lilac-core/hierarchy.h"

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

    llvm::cl::opt<std::string> s_IHSource{
        llvm::cl::Positional,
        llvm::cl::desc("<IH file>"),
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

    auto hierarchy = lilac::core::Hierarchy::CreateFromFile(s_IHSource);
    if (!hierarchy)
    {
        llvm::errs() << "Couldn't read IH from file '" << s_IHSource << "'.\n";
        return 1;
    }

    llvm::LLVMContext  context;
    llvm::SMDiagnostic err;

    auto module = parseIRFile(s_IRSource, err, context);
    if (!module)
    {
        err.print(argv[0], llvm::errs());
        llvm::errs() << "Couldn't read IR from file '" << s_IRSource << "'.\n";
        return 1;
    }

    for (auto& function : module->functions())
    {
        auto* h = hierarchy->QueryByActualName(function.getName().str());
        if (!h) continue;

        size_t i = 0;
        for (auto iter = function.arg_begin(); iter != function.arg_end(); ++i, ++iter)
        {
            if (iter->hasStructRetAttr())
            {
                h->GetFunctionData().Type = lilac::core::VoidTy;

                lilac::core::Hierarchy param{
                    lilac::core::HOK_Parameter,
                    lilac::core::SRetName,
                    lilac::core::SRetName
                };
                param.GetParameterData().Index = -1;
                param.GetParameterData().Type = iter->getParamStructRetType()->getStructName().str() + "*";

                h->Members.emplace(param);
            }
        }
    }

    std::ofstream ofs(s_Out);
    ofs << hierarchy->ToString();

    return 0;
}
