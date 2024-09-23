using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace Lilac.Generator;

public static partial class FileEmitter
{
    [Flags]
    private enum FileFlags
    {
        ReadOnly = 0,
        WriteOnly = 1,
        ReadWrite   = 2,
        Create  = 64
    }

    [LibraryImport("c", EntryPoint = "open", SetLastError = true, StringMarshalling = StringMarshalling.Utf8)]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    private static unsafe partial int Open([MarshalAs(UnmanagedType.LPUTF8Str)] string path, FileFlags flags);

    [LibraryImport("c", EntryPoint = "write", SetLastError = true)]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    private static unsafe partial int Write(int fd, void* buffer, nuint size);

    [LibraryImport("c", EntryPoint = "close", SetLastError = true)]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    private static unsafe partial int Close(int fd);

    [LibraryImport("kernel32", EntryPoint = "WriteFileA", SetLastError = true)]
    private static unsafe partial int WriteFile(
        void*             handle,
        void*             buffer,
        int               numBytesToWrite,
        int*              numBytesWritten,
        NativeOverlapped* lpOverlapped);

    [LibraryImport(
        "kernel32",
        EntryPoint = "CreateFileW",
        SetLastError = true,
        StringMarshalling = StringMarshalling.Utf16)]
    private static partial IntPtr CreateFileW(
        [MarshalAs(UnmanagedType.LPWStr)] string filename,
        FileAccess                               access,
        FileShare                                share,
        IntPtr                                   securityAttributes,
        FileMode                                 creationDisposition,
        FileAttributes                           flagsAndAttributes,
        IntPtr                                   templateFile);

    [LibraryImport("kernel32", EntryPoint = "CloseHandleA", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static unsafe partial bool CloseHandle(IntPtr handle);
    
    [LibraryImport("c", EntryPoint = "getcwd", SetLastError = true)]
    private static unsafe partial byte* GetCurrentDirectory(byte* buf, nuint size);
    
    [LibraryImport("kernel32", EntryPoint = "GetCurrentDirectoryW", SetLastError = true)]
    private static unsafe partial uint GetCurrentDirectory(uint nBufferLength, byte* lpBuffer);

    public static string GetCurrentDirectory()
    {
        var buffer = new byte[65535];
        unsafe
        {
            fixed (byte* p = buffer)
            {
                if (OperatingSystem.IsWindows())
                    GetCurrentDirectory((uint)buffer.Length, p);
                else
                    GetCurrentDirectory(p, (nuint)buffer.Length);
                
                return Marshal.PtrToStringUTF8((IntPtr)p, buffer.Length);
            }
        }
    }
    
    public static void Write(string path, string text)
    {
        var windows = OperatingSystem.IsWindows();
        
        var handle = windows
            ? CreateFileW(
                path,
                FileAccess.Write,
                FileShare.None,
                0,
                FileMode.OpenOrCreate,
                FileAttributes.None,
                0)
            : Open(path, FileFlags.WriteOnly | FileFlags.Create);
        if (handle < 0)
            Console.WriteLine($"Failed to open file: {Marshal.GetLastWin32Error()}");

        unsafe
        {
            var buffer = Encoding.UTF8.GetBytes(text);
            var len    = buffer.Length;
            fixed (byte* p = buffer)
            {
                if (windows)
                    WriteFile((void*)handle, p, len, &len, null);
                else
                    Write((int)handle, p, (UIntPtr)len);
            }
        }

        if (windows)
            CloseHandle(handle);
        else
            Close((int)handle);
    }
}