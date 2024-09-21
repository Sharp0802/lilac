using System.Runtime.InteropServices;
using Lilac.Annotations;

namespace Lilac.Test;

[Export]
public enum CsEnum
{
    A,
    B,
    C = -1
}

[Export]
public class CsModule
{
    [Export]
    public CsModule(string a)
    {
    }
    
    [Export]
    public string ModuleEntryPoint(string str)
    {
        Console.WriteLine($"{str} - printed from C#");
        return str;
    }

    ~CsModule()
    {
        GCHandle.Alloc(this, GCHandleType.Pinned).Free();
    }
}