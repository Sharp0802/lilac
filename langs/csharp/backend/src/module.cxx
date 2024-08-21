#include <iostream>

#include "pch.h"
#include "csharp/module.h"

std::string LibraryName;

std::map<int, std::string> errorMap = {
    { frxml::S_OK, "Operation completed successfully" },
    { frxml::E_NONAME, "Identifier expected" },
    { frxml::E_NOEQ, "Equal sign(=) expected" },
    { frxml::E_NOQUOTE, "Quote(\", \') expected" },
    { frxml::E_QUOTENOTCLOSED, "Quote not closed" },
    { frxml::E_INVCHAR, "Invalid character detected" },
    { frxml::E_NOTAG, "Tag expected" },
    { frxml::E_TAGNOTCLOSED, "Tag not closed" },
    { frxml::E_DUPATTR, "Duplicated attributes detected" },
    { frxml::E_ELEMNOTCLOSED, "Element not closed" },
    { frxml::E_INVETAG, "Invalid ETag" }
};

std::string IndentRaw(const std::string& src, int indentC)
{
    std::string indent(indentC, '\t');

    std::stringstream ss(src);
    std::stringstream out;

    for (std::string buffer; std::getline(ss, buffer);)
        out << indent << buffer;

    return out.str();
}

std::string ResolveTypeRef(std::string src)
{
    int refC = 0;
    for (; src[src.size() - 1 - refC] == '*'; refC++)
    {
    }
    if (refC)
        src = src.erase(src.length() - refC, refC);
    std::string ref(refC, '*');

    static std::map<std::string, std::string> builtins = {
        { "__void",  "void" },
        { "__bool",  "bool" },
        { "__u8",    "byte" },
        { "__u16",   "ushort" },
        { "__u32",   "uint" },
        { "__uptr",  "nuint" },
        { "__u64",   "ulong" },
        { "__u128",  "System.UInt128" },
        { "__s8",    "sbyte" },
        { "__s16",   "short" },
        { "__s32",   "int" },
        { "__sptr",  "nint" },
        { "__s64",   "long" },
        { "__s128",  "System.Int128" },
        { "__fp16",  "System.Half" },
        { "__fp32",  "float" },
        { "__fp64",  "double" }
    };
    if (builtins.contains(src))
        return builtins[src] + ref;

    for (char& i: src)
        if (i == '/')
            i = '.';
    return src + ref;
}

std::string CreatePInvokeStub(const frxml::dom& dom, int indentC, bool _this)
{
    std::string indent(indentC, '\t');

    std::stringstream prm;
    for (auto i = 0; i < dom.children().size(); ++i)
    {
        if (i) prm << ", ";
        auto type = ResolveTypeRef(static_cast<std::string>(dom.children()[i].attr().at("type").view()));
        if (type == "bool")
            prm << "[System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.U1)] ";
        prm << type << ' ' << dom.children()[i].attr().at("name").view();
    }

    std::stringstream ref;
    for (auto i = 0; i < dom.children().size(); ++i)
    {
        if (i) ref << ", ";
        ref << dom.children()[i].attr().at("name").view();
    }

    auto ret = ResolveTypeRef(static_cast<std::string>(dom.attr().at("return").view()));

    std::stringstream sst;
    if (_this)
        sst << indent << "fixed (void* p = __data)\n"
            << indent << "\t__PInvoke(p, " << ref.str() << ");\n";
    else
        sst << indent << "__PInvoke(" << ref.str() << ");\n\n";
    sst << indent << "[System.Runtime.InteropServices.DllImport(\""
        << LibraryName << "\", EntryPoint=\"" << dom.attr().at("mangle").view() << "\", ExactSpelling=true)]\n"
        << indent << "static extern " << ret << " __PInvoke(";
    if (_this)
        sst << "void* @this, ";
    sst << prm.str() << ");\n";
    return sst.str();
}

void Traverse(std::ostream& ost, const frxml::dom& parent, const frxml::dom& dom, int indentC)
{
    std::string indent(indentC, '\t');

    if (dom.tag().view() == "namespace")
    {
        ost << indent << "public static class " << dom.attr().at("name").view() << '\n'
            << indent << "{\n";
        for (auto i = 0; i < dom.children().size(); ++i)
        {
            if (i) ost << '\n';
            Traverse(ost, dom, dom.children()[i], indentC + 1);
        }
        ost << indent << "}\n";
    }
    else if (dom.tag().view() == "enum")
    {
        ost << indent << "public enum " << dom.attr().at("name").view() << " : "
            << ResolveTypeRef(static_cast<std::string>(dom.attr().at("type").view())) << '\n'
            << indent << "{\n";
        for (auto& child: dom.children())
            Traverse(ost, dom, child, indentC + 1);
        ost << indent << "}\n";
    }
    else if (dom.tag().view() == "constant")
    {
        ost << indent << dom.attr().at("name").view() << " = " << dom.attr().at("value").view() << ",\n";
    }
    else if (dom.tag().view() == "ctor")
    {
        const auto type = parent.attr().at("name").view();

        std::stringstream params;
        for (auto i = 0; i < dom.children().size(); ++i)
        {
            if (i) params << ", ";
            Traverse(params, dom, dom.children()[i], 0);
        }

        ost << indent << "public " << type << "(" << params.str() << ");\n"
            << indent << "{\n"
            << CreatePInvokeStub(dom, indentC + 1, true)
            << indent << "}\n";
    }
    else if (dom.tag().view() == "dtor")
    {
        const auto type = parent.attr().at("name").view();

        ost << indent << "public ~" << type << "()\n"
            << indent << "{\n"
            << CreatePInvokeStub(dom, indentC + 1, true)
            << indent << "}\n";
    }
    else if (dom.tag().view() == "method")
    {
        std::stringstream params;
        for (auto i = 0; i < dom.children().size(); ++i)
        {
            if (i) params << ", ";
            Traverse(params, dom, dom.children()[i], 0);
        }

        const auto ret  = dom.attr().at("return").view();
        const auto name = dom.attr().at("name").view();

        ost << indent << "public " << ret << ' ' << name << '(' << params.str() << ");\n"
            << indent << "{\n"
            << CreatePInvokeStub(dom, indentC + 1, true)
            << indent << "}\n";
    }
    else if (dom.tag().view() == "function")
    {
        std::stringstream params;
        for (auto i = 0; i < dom.children().size(); ++i)
        {
            if (i) params << ", ";
            Traverse(params, dom, dom.children()[i], 0);
        }

        const auto ret  = ResolveTypeRef(static_cast<std::string>(dom.attr().at("return").view()));
        const auto name = dom.attr().at("name").view();

        ost << indent << "public static " << ret << ' ' << name << '(' << params.str() << ");\n"
            << indent << "{\n"
            << CreatePInvokeStub(dom, indentC + 1, false)
            << indent << "}\n";
    }
    else if (dom.tag().view() == "param")
    {
        ost << indent
            << ResolveTypeRef(static_cast<std::string>(dom.attr().at("type").view()))
            << ' ' << dom.attr().at("name").view();
    }
    else if (dom.tag().view() == "record")
    {
        ost << "public struct " << dom.attr().at("name").view() << '\n'
            << "{\n"
            << "\tprivate byte __data[" << dom.attr().at("size").view() << "];\n";

        for (auto i = 0; i < dom.children().size(); ++i)
        {
            if (i) ost << '\n';
            Traverse(ost, dom, dom.children()[i], indentC + 1);
        }

        ost << "}\n";
    }
    else
    {
        std::cerr << "Unknown tag '" << dom.tag().view() << "'\n";
    }
}

MODULE(csharp)
{
    LibraryName = libPath;

    std::stringstream input;
    try
    {
        std::ifstream ifs(xmlPath);
        input << ifs.rdbuf();
    }
    catch (const std::ifstream::failure& e)
    {
        std::cerr << "Error occurred while opening file: " << e.what() << std::endl;
        return 1;
    }

    const std::string& source = input.str();

    frxml::doc doc(source);
    if (!doc)
    {
        const char* begin  = doc.error().source;
        size_t      length = 1;

        for (auto i = 0; i < 5 && begin > &source[0]; ++i)
            begin--;
        for (auto i = 0; i < 5 && begin + length < &source[source.size() - 1]; ++i)
            length++;

        std::cerr
            << "Error occurred while parsing XML: "
            << errorMap[doc.error().code]
            << " (" << std::string_view(begin, length) << ")\n";
    }

    try
    {
        std::ofstream ofs(outPath);
        for (auto& child: doc.root().children())
        {
            ofs << '\n';
            Traverse(ofs, doc.root(), child, 0);
        }
    }
    catch (const std::ofstream::failure& e)
    {
        std::cerr << "Error occurred while opening file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
