using Microsoft.CodeAnalysis;

namespace Lilac.Generator;

public static class TypeHelper
{
    public static bool IsExported(this ISymbol symbol)
    {
        return symbol
            .GetAttributes()
            .Any(attr => attr.AttributeClass?.ToDisplayString() == "Lilac.Annotations.ExportAttribute");
    }

    public static string GetFullName(this ISymbol symbol)
    {
        return symbol.ContainingNamespace is null
            ? symbol.Name
            : $"{symbol.ContainingNamespace.ToDisplayString().Replace(".", "::")}::{symbol.Name}";
    }

    public static string GetManagedFullName(this ISymbol symbol)
    {
        return $"{symbol.ContainingNamespace.ToDisplayString()}.{symbol.Name}";
    }

    public static string GetNativeName(this ISymbol symbol)
    {
        var dict = new Dictionary<string, string>
        {
            ["System::Void"]    = "void",
            ["System::Boolean"] = "bool",
            ["System::SByte"]   = "std::int8_t",
            ["System::Int16"]   = "std::int16_t",
            ["System::Int32"]   = "std::int32_t",
            ["System::Int64"]   = "std::int64_t",
            ["System::IntPtr"]  = "std::intptr_t",
            ["System::Byte"]    = "std::uint8_t",
            ["System::UInt16"]  = "std::uint16_t",
            ["System::UInt32"]  = "std::uint32_t",
            ["System::UInt64"]  = "std::uint64_t",
            ["System::UIntPtr"] = "std::uintptr_t",
            ["System::Char"]    = "std::char16_t",
            ["System::Double"]  = "std::float64_t",
            ["System::Single"]  = "std::float32_t",
            ["System::Half"]    = "std::float16_t",
            ["System::String"]  = "clr::string"
        };

        var fullname = symbol.GetFullName();
        return dict.GetValueOrDefault(fullname, fullname);
    }

    public static string GetUnmanagedPackedName(this ISymbol symbol, string @default = "void*")
    {
        var dict = new Dictionary<string, string>
        {
            ["System::Void"]    = "void",
            ["System::Boolean"] = "bool",
            ["System::SByte"]   = "std::int8_t",
            ["System::Int16"]   = "std::int16_t",
            ["System::Int32"]   = "std::int32_t",
            ["System::Int64"]   = "std::int64_t",
            ["System::IntPtr"]  = "std::intptr_t",
            ["System::Byte"]    = "std::uint8_t",
            ["System::UInt16"]  = "std::uint16_t",
            ["System::UInt32"]  = "std::uint32_t",
            ["System::UInt64"]  = "std::uint64_t",
            ["System::UIntPtr"] = "std::uintptr_t",
            ["System::Char"]    = "std::char16_t",
            ["System::Double"]  = "std::float64_t",
            ["System::Single"]  = "std::float32_t",
            ["System::Half"]    = "std::float16_t",
            ["System::String"]  = "const char*"
        };

        var fullname = symbol.GetFullName();
        return dict.GetValueOrDefault(fullname, @default);
    }

    private static readonly string[] Primitives =
    [
        "System.Void",
        "System.Boolean",
        "System.SByte",
        "System.Int16",
        "System.Int32",
        "System.Int64",
        "System.IntPtr",
        "System.Byte",
        "System.UInt16",
        "System.UInt32",
        "System.UInt64",
        "System.UIntPtr",
        "System.Char",
        "System.Double",
        "System.Single",
        "System.Half",
        "System.String"
    ];

    public static bool IsPrimitives(this ISymbol symbol)
    {
        return Primitives.Contains(symbol.GetManagedFullName());
    }

    public static string GetManagedPackedName(this ISymbol symbol)
    {
        var fullname = symbol.GetManagedFullName();
        return fullname == "System.String" ? "unsafe char*" : fullname;
    }

    public static string GetUnmanagedMethodName(this IMethodSymbol symbol)
    {
        return symbol.Name switch
        {
            ".ctor" => symbol.ContainingType.Name,
            _       => symbol.Name
        };
    }

    public static string GetAssemblyQualifiedName(this INamedTypeSymbol symbol)
    {
        var fullName     = GetFullNameInternal(symbol);
        var assemblyName = symbol.ContainingAssembly?.ToDisplayString() ?? string.Empty;
        return $"{fullName}, {assemblyName}";

        static string GetFullNameInternal(INamedTypeSymbol symbol)
        {
            if (symbol.ContainingType != null)
                return $"{GetFullNameInternal(symbol.ContainingType)}+{symbol.MetadataName}";

            if (!string.IsNullOrEmpty(symbol.ContainingNamespace?.ToDisplayString()))
                return $"{symbol.ContainingNamespace.ToDisplayString()}.{symbol.MetadataName}";

            return symbol.MetadataName;
        }
    }
}