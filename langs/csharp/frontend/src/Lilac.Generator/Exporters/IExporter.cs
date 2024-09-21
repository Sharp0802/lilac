using System.CodeDom.Compiler;
using System.Runtime.InteropServices.JavaScript;
using System.Xml.Linq;
using Microsoft.CodeAnalysis;

namespace Lilac.Generator.Exporters;

public interface IExporter
{
    public void GenerateUnmanagedCode(IndentedTextWriter source, IndentedTextWriter header);
    public void GenerateManagedCode(IndentedTextWriter   builder);

    public static IExporter Create(ISymbol symbol)
    {
        return symbol switch
        {
            IMethodSymbol m                                => new MethodExporter(m),
            INamedTypeSymbol { TypeKind: TypeKind.Enum } t => new EnumExporter(t),
            INamedTypeSymbol t                             => new TypeExporter(t),
            _ => throw new NotSupportedException(
                $"{symbol.GetType().Name}({symbol.ToDisplayString()}) couldn't be exported")
        };
    }
}