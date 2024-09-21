using System.CodeDom.Compiler;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using Microsoft.CodeAnalysis;

namespace Lilac.Generator;

public class ParameterPack
{
    private readonly IMethodSymbol _symbol;

    public string Name { get; }

    public ParameterPack(IMethodSymbol symbol)
    {
        _symbol = symbol;

        var builder = new StringBuilder();
        foreach (var parameter in _symbol.Parameters)
            builder.Append(parameter.Type.ToDisplayString());

        var mangled = HashString(builder.ToString());

        Name = $"__argpack__{symbol.GetUnmanagedMethodName()}_{mangled}";
    }

    private static string HashString(string data)
    {
        return Convert.ToHexString(MD5.HashData(Encoding.UTF8.GetBytes(data)).Take(4).ToArray());
    }

    public void GenerateManagedStub(IndentedTextWriter builder)
    {
        builder.WriteLine("[global::System.Runtime.InteropServices(LayoutKind.Sequential)]");
        builder.WriteLine($"public struct {Name}");
        builder.WriteLine("{");
        builder.Indent++;

        if (!_symbol.IsStatic) 
            builder.WriteLine("public System.IntPtr __this;");

        foreach (var parameter in _symbol.Parameters)
        {
            var isPrimitive = parameter.Type.IsPrimitives();
            
            var type = isPrimitive ? parameter.Type.GetManagedPackedName() : "System.IntPtr";
            var name = !isPrimitive || type == "unsafe char*" ? $"__unsafe_{parameter.Name}" : parameter.Name;
            
            builder.WriteLine($"public {type} {name};");
        }

        if (!_symbol.ReturnsVoid) 
            builder.WriteLine($"public {_symbol.ReturnType.GetManagedPackedName()} __return;");

        foreach (var parameter in _symbol.Parameters)
        {
            if (parameter.Type.GetFullName() == "System::String")
            {
                builder.WriteLine();
                builder.WriteLine($"public string {parameter.Name}");
                builder.WriteLine("{");
                builder.Indent++;
                builder.WriteLine("get");
                builder.WriteLine("{");
                builder.Indent++;
                builder.WriteLine($"return System.Runtime.InteropServices.Marshal.PtrToStringUTF8(__unsafe_{parameter.Name});");
                builder.Indent--;
                builder.WriteLine("}");
                builder.Indent--;
                builder.WriteLine("}");
            }
            else if (!parameter.Type.IsPrimitives())
            {
                var fullname = parameter.Type.GetManagedFullName();
                
                builder.WriteLine();
                builder.WriteLine($"public {fullname} {parameter.Name}");
                builder.WriteLine("{");
                builder.Indent++;
                builder.WriteLine("get");
                builder.WriteLine("{");
                builder.Indent++;
                builder.WriteLine($"return ({fullname})System.Runtime.InteropServices.GCHandle.FromIntPtr(__unsafe_{parameter.Name}).Target;");
                builder.Indent--;
                builder.WriteLine("}");
                builder.Indent--;
                builder.WriteLine("}");
            }
        }
        
        builder.Indent--;
        builder.WriteLine("}");
    }

    public void GenerateUnmanagedStub(IndentedTextWriter builder)
    {
        builder.WriteLine($"struct {Name}");
        builder.WriteLine("{");
        builder.Indent++;

        if (!_symbol.IsStatic) 
            builder.WriteLine("void* __this;");

        foreach (var parameter in _symbol.Parameters) 
            builder.WriteLine($"{parameter.Type.GetUnmanagedPackedName()} {parameter.Name};");

        if (!_symbol.ReturnsVoid) 
            builder.WriteLine($"{_symbol.ReturnType.GetUnmanagedPackedName()} __return;");
        
        builder.Indent--;
        builder.WriteLine("};");
    }
}