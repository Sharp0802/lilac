using System.CodeDom.Compiler;
using Microsoft.CodeAnalysis;

namespace Lilac.Generator.Exporters;

public class EnumExporter(INamedTypeSymbol symbol) : IExporter
{
    public void GenerateUnmanagedCode(IndentedTextWriter source, IndentedTextWriter header)
    {
        header.WriteLine($"namespace {symbol.ContainingNamespace.ToDisplayString().Replace(".", "::")}");
        header.WriteLine("{");
        header.Indent++;
        header.WriteLine($"enum {symbol.Name} : {symbol.EnumUnderlyingType!.ToDisplayString()}");
        header.WriteLine("{");
        header.Indent++;
            
        foreach (var entry in symbol.GetMembers().OfType<IFieldSymbol>()) 
            header.WriteLine($"{entry.Name} = {entry.ConstantValue!}");
                
        header.Indent--;
        header.WriteLine("};");
        header.Indent--;
        header.WriteLine("}");
    }

    public void GenerateManagedCode(IndentedTextWriter builder)
    {
    }
}