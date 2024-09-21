using System.CodeDom.Compiler;
using System.Xml.Linq;
using Microsoft.CodeAnalysis;

namespace Lilac.Generator.Exporters;

public class TypeExporter(INamedTypeSymbol symbol) : IExporter
{
    public void GenerateUnmanagedCode(IndentedTextWriter source, IndentedTextWriter header)
    {
        // TODO : Handle pinned-memory leak
        // TODO : Impl implicit conversion from void*
        
        header.WriteLine($"namespace {symbol.ContainingNamespace.GetFullName()}");
        header.WriteLine("{");
        header.Indent++;
        
        header.WriteLine($"class {symbol.Name}");
        header.WriteLine("{");
        header.Indent++;
        
        header.WriteLine("void* __this;");
        header.WriteLine();
        
        header.Indent--;
        header.WriteLine("public:");
        header.Indent++;
        
        var methods = symbol.GetMembers().OfType<IMethodSymbol>().Where(method => method.IsExported()).ToArray();
        for (var i = 0; i < methods.Length; i++)
        {
            var method = methods[i];
            if (i > 0)
            {
                source.WriteLine();
                header.WriteLine();
                header.WriteLine();
            }
            
            new MethodExporter(method).GenerateUnmanagedCode(source, header);
        }

        header.Indent--;
        header.WriteLine("};");

        header.Indent--;
        header.WriteLine("}");
    }

    public void GenerateManagedCode(IndentedTextWriter builder)
    {
        builder.WriteLine($"namespace {symbol.ContainingNamespace.GetManagedFullName()}");
        builder.WriteLine("{");
        builder.Indent++;

        var access = symbol.DeclaredAccessibility switch
        {
            Accessibility.Private              => "private",
            Accessibility.ProtectedAndInternal => "private protected",
            Accessibility.Protected            => "protected",
            Accessibility.Internal             => "internal",
            Accessibility.ProtectedOrInternal  => "protected internal",
            Accessibility.Public               => "public",
            _                                  => throw new ArgumentOutOfRangeException()
        };
        
        builder.WriteLine($"{access} partial class {symbol.Name}");
        builder.WriteLine("{");
        builder.Indent++;

        var methods = symbol.GetMembers().OfType<IMethodSymbol>().Where(method => method.IsExported()).ToArray();
        for (var i = 0; i < methods.Length; i++)
        {
            var method = methods[i];
            if (i > 0)
            {
                builder.WriteLine();
                builder.WriteLine();
            }
            IExporter.Create(method).GenerateManagedCode(builder);
        }

        builder.Indent--;
        builder.WriteLine("}");

        builder.Indent--;
        builder.WriteLine("}");
    }
}