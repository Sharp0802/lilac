#include <map>
#include <sstream>
#include <stack>
#include <llvm/Support/CommandLine.h>

#include "pch.h"
#include "version.h"
#include "lilac-core/hierarchy.h"

namespace
{
    llvm::cl::OptionCategory s_Category{
        "Compile Options"
    };

    llvm::cl::SubCommand s_IB{
        "ib",
        "Compile a interop bridge"
    };

    llvm::cl::opt<std::string> s_Out{
        llvm::cl::Positional,
        llvm::cl::desc("<output>"),
        llvm::cl::cat(s_Category),
        llvm::cl::sub(s_IB),
        llvm::cl::Required
    };

    llvm::cl::opt<std::string> s_IHSource{
        llvm::cl::Positional,
        llvm::cl::desc("<IH file>"),
        llvm::cl::cat(s_Category),
        llvm::cl::sub(s_IB),
        llvm::cl::Required
    };

    llvm::cl::opt<std::string> s_LibName{
        "l",
        llvm::cl::value_desc("<library name>"),
        llvm::cl::desc("Specify library file name (e.g. mylib.dll, libmylib.so)"),
        llvm::cl::cat(s_Category),
        llvm::cl::sub(s_IB),
        llvm::cl::ValueRequired
    };
}

template<typename T>
std::string JoinString(T begin, T end, const std::string& c)
{
    std::stringstream ss;
    for (auto i = begin; i != end; ++i)
    {
        if (i != begin)
            ss << c;
        ss << *i;
    }
    return ss.str();
}

template<typename T>
std::string JoinString(T&& v, const std::string& c)
{
    return JoinString(v.begin(), v.end(), c);
}

std::string MangleFunctionNameToCSharp(std::string fn)
{
    if (!isalpha(fn[0]) && fn[0] != '_')
        fn = '_' + fn;

    for (char& ch: fn)
        if (!isalnum(ch) && ch != '_')
            ch = '_';

    return fn;
}

std::string GetDisplayNamespace(lilac::core::Hierarchy* h)
{
    lilac::core::Hierarchy* current = h;
    do
    {
        if (current->GetParent() == nullptr)
            llvm::errs() << current->ToString() << '\n';
        current = current->GetParent();
    } while (current->Kind != lilac::core::HOK_Namespace && current->Kind != lilac::core::HOK_Root);

    std::vector<std::string> stk;
    for (; current->Kind == lilac::core::HOK_Namespace; current = current->GetParent())
        stk.emplace_back(current->Name);

    return JoinString(stk.rbegin(), stk.rend(), ".");
}

std::string MangleTypeToCSharpRef(lilac::core::Hierarchy* root, std::string name, bool native)
{
    auto ref = 0;
    for (; ref < name.size(); ++ref)
    {
        if (name[name.size() - ref - 1] != '*')
            break;
    }
    name = name.substr(0, name.size() - ref);

    if (lilac::core::IsBuiltinType(name))
    {
        static std::map<std::string, std::string> s_BuiltinMap = {
            { lilac::core::VoidTy, "void" },
            { lilac::core::UTF8Ty, "byte" },
            { lilac::core::UTF16Ty, "char" },
            { lilac::core::UTF32Ty, "uint" },
            { lilac::core::FP16Ty, "System.Half" },
            { lilac::core::FP32Ty, "float" },
            { lilac::core::FP64Ty, "double" },
            { lilac::core::S8Ty, "sbyte" },
            { lilac::core::S16Ty, "short" },
            { lilac::core::S32Ty, "int" },
            { lilac::core::S64Ty, "long" },
            { lilac::core::U8Ty, "byte" },
            { lilac::core::U16Ty, "ushort" },
            { lilac::core::U32Ty, "uint" },
            { lilac::core::U64Ty, "ulong" },
            { lilac::core::BoolTy, "bool" },
        };
        name = s_BuiltinMap[name];
    }
    else
    {
        auto type = root->QueryByActualName(name);
        if (!type)
        {
            llvm::errs() << "Type name couldn't be resolved: '" << name << "'.\n";
            return "%unknown";
        }
        name = GetDisplayNamespace(type);
        if (!name.empty())
            name += '.';
        if (native && type->Kind != lilac::core::HOK_Enum)
            name += "__Impl_";
        name += type->Name;
    }

    for (auto i = 0; i < ref; ++i)
        name += '*';
    return name;
}

std::string MangleFunctionToCSharpDecl(
    lilac::core::Hierarchy* root,
    lilac::core::Hierarchy* function,
    const std::string&      libname)
{
    bool sret = false;

    std::vector<lilac::core::Hierarchy*> params;
    for (auto& member: function->Members)
        params.push_back(const_cast<lilac::core::Hierarchy*>(&member));
    std::ranges::sort(params, [](auto* a, auto* b)
    {
        return a->GetParameterData().Index < b->GetParameterData().Index;
    });
    std::vector<std::string> paramNames;
    for (auto* param: params)
    {
        auto type = MangleTypeToCSharpRef(root, param->GetParameterData().Type, true);
        auto name = param->Name;
        if (name == lilac::core::SRetName)
        {
            name = "__sret";
            type = "out " + type.replace(type.find_last_of('*'), 1, "");
            sret = true;
        }
        paramNames.push_back(std::format("{} {}", type, name));
    }

    auto        parent     = function->GetParent();
    std::string parentName = parent->Kind == lilac::core::HOK_Type
        ? parent->Name
        : "__Global";
    if (function->Kind == lilac::core::HOK_Method)
    {
        paramNames.insert(
            sret ? ++paramNames.begin() : paramNames.begin(),
            MangleTypeToCSharpRef(root, parent->ActualName, true) + "* __this"
        );
    }

    return std::format(
        R"(
namespace {} {{
    public static partial class {} {{
        [System.Runtime.InteropServices.DllImport("{}", EntryPoint="{}", ExactSpelling=true)]
        private static unsafe extern {} {}({});
    }}
}}
)",
        GetDisplayNamespace(function),
        parentName,
        libname,
        function->ActualName,
        MangleTypeToCSharpRef(root, function->GetFunctionData().Type, true),
        MangleFunctionNameToCSharp(function->Name),
        JoinString(paramNames, ", ")
    );
}

int main(int argc, char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os)
    {
        os << "lilac-msil " VERSION " " TIMESTAMP "\n";
    });
    llvm::cl::HideUnrelatedOptions(s_Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (s_IB)
    {
        std::string libname = s_IHSource.c_str();
        if (s_LibName.empty())
        {
            constexpr auto fileext =
#if _WIN32
                ".dll";
#else
                ".so";
#endif
            const auto pos = s_IHSource.find_first_of('/');
            libname = s_IHSource.substr(pos == std::string::npos ? 0 : pos, s_IHSource.find_last_of('.')) + fileext;
            llvm::outs() << "library name not specified. defaults to '" << libname << "'.\n";
        }

        auto root = lilac::core::Hierarchy::CreateFromFile(s_IHSource);
        if (!root)
        {
            std::stringstream ss;
            ss << "Couldn't read IH from file '" << s_IHSource << "'.";
            s_IHSource.error(ss.str());
            return 1;
        }

        for (auto function: root->QueryByKind(lilac::core::HOK_Function))
        {
            llvm::outs() << MangleFunctionToCSharpDecl(root.get(), function, libname);
        }
        for (auto function: root->QueryByKind(lilac::core::HOK_Method))
        {
            auto parent = function->GetParent();
            if (parent == nullptr || parent->Kind != lilac::core::HOK_Type)
            {
                llvm::errs() << "Method must has Type as parent\n";
                return 1;
            }

            llvm::outs() << MangleFunctionToCSharpDecl(root.get(), function, libname);
        }
    }

    return 0;
}
