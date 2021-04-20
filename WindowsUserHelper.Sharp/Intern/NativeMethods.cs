using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace WindowsUserHelper.Sharp.Intern
{
  internal static class NativeMethods
  {
    private static readonly IntPtr __libraryAddress = IntPtr.Zero;

    [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
    private static extern IntPtr LoadLibrary(string lpFileName);

    [DllImport("kernel32.dll")]
    private static extern IntPtr GetProcAddress(IntPtr hModule, string procname);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void GetCurrentProcessUserDelegate(IntPtr pUserInformation);

    public static GetCurrentProcessUserDelegate GetCurrentProcessUser;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void GetCurrentThreadUserDelegate(IntPtr pUserInformation, int isImpersonating);

    public static GetCurrentThreadUserDelegate GetCurrentThreadUser;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OpenConnectionDelegate([MarshalAs(UnmanagedType.LPUTF8Str)] string name, IntPtr pServer);

    public static OpenConnectionDelegate OpenConnection;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void CloseConnectionDelegate(IntPtr server);

    public static CloseConnectionDelegate CloseConnection;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public unsafe delegate void EnumerateSessionsDelegate(IntPtr server, uint* pSessionsCount, IntPtr sessions);

    public static EnumerateSessionsDelegate EnumerateSessions;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public unsafe delegate void EnumerateProcessesDelegate(IntPtr server, IntPtr pSession,
      uint* pProcessesCount, IntPtr pProcesses);

    public static EnumerateProcessesDelegate EnumerateProcesses;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogoffSessionDelegate(IntPtr server, IntPtr session);

    public static LogoffSessionDelegate LogoffSession;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void TerminateRemoteProcessDelegate(IntPtr server, IntPtr process);

    public static TerminateRemoteProcessDelegate TerminateRemoteProcess;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void UpdateProcessInformationDelegate(IntPtr server, IntPtr process);

    public static UpdateProcessInformationDelegate UpdateProcessInformation;

    /// <summary>
    /// </summary>
    /// <remarks>Mostly stolen from here: https://github.com/mhowlett/NNanomsg/blob/master/NNanomsg/Interop.cs</remarks>
    static NativeMethods()
    {
      var libFile = "WindowsUserHelper.dll";
      var rootDirectory = AppDomain.CurrentDomain.BaseDirectory;
      var assemblyDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

      var paths = new[]
      {
        Path.Combine(assemblyDirectory, "bin", Environment.Is64BitProcess ? "x64" : "x86", libFile),
        Path.Combine(assemblyDirectory, Environment.Is64BitProcess ? "x64" : "x86", libFile),
        Path.Combine(assemblyDirectory, libFile),

        Path.Combine(rootDirectory, "bin", Environment.Is64BitProcess ? "x64" : "x86", libFile),
        Path.Combine(rootDirectory, Environment.Is64BitProcess ? "x64" : "x86", libFile),
        Path.Combine(rootDirectory, libFile)
      };

      foreach (var path in paths)
      {
        if (path == null)
          continue;

        if (File.Exists(path))
        {
          var addr = LoadLibrary(path);
          if (addr == IntPtr.Zero)
            throw new Exception("LoadLibrary failed: " + path);

          __libraryAddress = addr;
          break;
        }
      }

      if (__libraryAddress == IntPtr.Zero)
        throw new Exception("Could not find any lib");

      GetCurrentProcessUser = GetFunctionPointer<GetCurrentProcessUserDelegate>(nameof(GetCurrentProcessUser));
      GetCurrentThreadUser = GetFunctionPointer<GetCurrentThreadUserDelegate>(nameof(GetCurrentThreadUser));
      OpenConnection =  GetFunctionPointer<OpenConnectionDelegate>(nameof(OpenConnection));
      CloseConnection = GetFunctionPointer<CloseConnectionDelegate>(nameof(CloseConnection));
      EnumerateSessions = GetFunctionPointer<EnumerateSessionsDelegate>(nameof(EnumerateSessions));
      EnumerateProcesses = GetFunctionPointer<EnumerateProcessesDelegate>(nameof(EnumerateProcesses));
      LogoffSession = GetFunctionPointer<LogoffSessionDelegate>(nameof(LogoffSession));
      TerminateRemoteProcess = GetFunctionPointer<TerminateRemoteProcessDelegate>(nameof(TerminateRemoteProcess));
      UpdateProcessInformation = GetFunctionPointer<UpdateProcessInformationDelegate>(nameof(UpdateProcessInformation));
    }

    private static T GetFunctionPointer<T>(string name)
    {
      return Marshal.GetDelegateForFunctionPointer<T>(GetProcAddress(__libraryAddress, name));
    }
  }
}