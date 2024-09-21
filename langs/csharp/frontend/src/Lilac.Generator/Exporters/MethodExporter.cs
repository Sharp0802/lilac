using System.CodeDom.Compiler;
using System.Text;
using System.Xml.Linq;
using Microsoft.CodeAnalysis;

namespace Lilac.Generator.Exporters;

public class MethodExporter(IMethodSymbol symbol) : IExporter
{
    private string GetMangling()
    {
        return $"__interop_{symbol.GetUnmanagedMethodName()}";
    }
    
    public void GenerateUnmanagedCode(IndentedTextWriter source, IndentedTextWriter header)
    {
        // Emit parameter pack definition
        var argsPack = new ParameterPack(symbol);
        argsPack.GenerateUnmanagedStub(header);
        header.WriteLine();
        
        // Emit method declaration
        var sb = new StringBuilder();
        sb.Append(symbol.ReturnType.GetNativeName()).Append(' ');
        var namespaceOffset = sb.Length;

        sb.Append(symbol.GetUnmanagedMethodName()).Append('(');
        for (var i = 0; i < symbol.Parameters.Length; i++)
        {
            var parameter = symbol.Parameters[i];
            if (i > 0)
                sb.Append(", ");
            sb.Append($"{parameter.Type.GetNativeName()} {parameter.Name}");
        }

        sb.Append(");");
        header.WriteLine(sb.ToString());
        
        // Emit method definition
        sb.Insert(namespaceOffset, $"{symbol.ContainingType.GetNativeName()}::");
        sb.Remove(sb.Length - 1, 1);
        source.WriteLine(sb.ToString());

        source.WriteLine("{");

        source.Indent++;
        
        // Pack arguments into parameter pack
        sb.Clear();
        sb.Append($"{argsPack.Name} __argpack(");
        if (!symbol.IsStatic)
        {
            sb.Append("__this");
            if (symbol.Parameters.Any())
                sb.Append(", ");
        }
        for (var i = 0; i < symbol.Parameters.Length; i++)
        {
            var parameter = symbol.Parameters[i];
            if (i > 0)
                sb.Append(", ");
            sb.Append($"{parameter.Name}");
        }
        sb.AppendLine(");");
        source.WriteLine(sb.ToString());
        
        // Inquire fp for mehtod
        source.WriteLine("int (*fp)(void*, std::int32_t) = reinterpret_cast<decltype(fp)>(g_dhost->LoadMethod(");
        source.Indent++;
        source.WriteLine($"\"{symbol.ContainingType.GetAssemblyQualifiedName()}\",");
        source.WriteLine($"\"{GetMangling()}\"));");
        source.Indent--;
        
        source.WriteLine();
        source.WriteLine("fp(&__argpack, sizeof(__argpack));");
        if (symbol.Name == ".ctor") 
            source.WriteLine("__this = __argpack.__this;");
        if (!symbol.ReturnsVoid)
            source.WriteLine("return __argpack.__return;");
        
        source.Indent--;
        source.WriteLine("}");
    }

    public void GenerateManagedCode(IndentedTextWriter builder)
    {
        var argsPack = new ParameterPack(symbol);
        argsPack.GenerateManagedStub(builder);
        builder.WriteLine();
        
        builder.WriteLine("[System.Runtime.InteropServices.UnmanagedCallersOnly]");
        builder.WriteLine($"public static unsafe int {GetMangling()}({argsPack.Name}* argpack, int size)");
        builder.WriteLine("{");
        builder.Indent++;

        if (symbol.Name == ".ctor")
        {
            var args = string.Join(", ", symbol.Parameters.Select(parameter => $"argpack->{parameter.Name}"));
            builder.WriteLine($"var obj = new {symbol.ContainingType.Name}({args})");
            builder.WriteLine("var handle = System.Runtime.InteropServices.GCHandle.Alloc(obj, System.Runtime.InteropServices.GCHandleType.Pinned);");
            builder.WriteLine("argpack->__this = handle.AddrOfPinnedObject();");
        }
        else
        {
            var args = string.Join(", ", symbol.Parameters.Select(parameter => $"argpack->{parameter.Name}"));
            builder.WriteLine("var handle = System.Runtime.InteropServices.GCHandle.FromIntPtr(argpack->__this);");
            builder.WriteLine($"argpack->__return = (({symbol.ContainingType.Name})handle.Target).{symbol.Name}({args});");
        }
        
        builder.WriteLine("return 0;");
        
        builder.Indent--;
        builder.WriteLine("}");
    }
}