using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using WindowsUserHelper.Sharp.Intern;

namespace WindowsUserHelper.Sharp
{
  public class WindowsUserHelper
  {
    public static string GetProcessUserName()
    {
      Intern.UserInformation userInformation = new Intern.UserInformation();
      IntPtr pUserInformation = userInformation.__Instance;

      NativeMethods.GetCurrentProcessUser(pUserInformation);

      if (userInformation.UserNameLength == 0) return string.Empty;

      unsafe
      {
        fixed (sbyte* userName = userInformation.UserName)
        fixed (sbyte* domainName = userInformation.DomainName)
        {
          return string.Format("{0}\\{1}",
            new string(domainName),
            new string(userName));
        }
      }
    }

    public static string GetThreadUserName(bool isImpersonating = true)
    {
      Intern.UserInformation userInformation = new Intern.UserInformation();
      IntPtr pUserInformation = userInformation.__Instance;

      NativeMethods.GetCurrentThreadUser(pUserInformation, isImpersonating ? 1 : 0);

      if (userInformation.UserNameLength == 0) return string.Empty;

      unsafe
      {
        fixed (sbyte* userName = userInformation.UserName)
        fixed (sbyte* domainName = userInformation.DomainName)
        {
          return string.Format("{0}\\{1}",
            new string(domainName),
            new string(userName));
        }
      }
    }
  }

  public class Server : IDisposable
  {
    private readonly Intern.Server _instance;

    public unsafe DisposableList<Session> Sessions
    {
      get
      {
        uint sessionsCount = 0;
        NativeMethods.EnumerateSessions(Pointer, &sessionsCount, IntPtr.Zero);
        
        Intern.Session[] sessions = new Intern.Session[sessionsCount];
        int size = Marshal.SizeOf(typeof(Intern.Session.__Internal));
        IntPtr pointer = Marshal.AllocHGlobal((int) (size * sessionsCount));
        NativeMethods.EnumerateSessions(Pointer, &sessionsCount, pointer);

        for (int i = 0; i < sessionsCount; i++)
        {
          var p = IntPtr.Add(pointer, i * size);
          var intern = (Intern.Session.__Internal)Marshal.PtrToStructure(p, typeof(Intern.Session.__Internal));
          sessions[i] = Intern.Session.__CreateInstance(intern);
        }
        
        Marshal.FreeHGlobal(pointer);
        
        DisposableList<Session> returnValues = new DisposableList<Session>(sessions.Select(s => new Session(this, s)));
        return returnValues;
      }
    }

    public unsafe DisposableList<Process> Processes
    {
      get
      {
        uint processCount = 0;
        NativeMethods.EnumerateProcesses(Pointer, IntPtr.Zero, &processCount, IntPtr.Zero);
        
        Intern.Process[] processes = new Intern.Process[processCount];
        int size = Marshal.SizeOf(typeof(Intern.Process.__Internal));
        IntPtr pointer = Marshal.AllocHGlobal((int) (size * processCount));
        NativeMethods.EnumerateProcesses(Pointer, IntPtr.Zero, &processCount, pointer);

        for (int i = 0; i < processCount; i++)
        {
          var p = IntPtr.Add(pointer, i * size);
          var intern = (Intern.Process.__Internal)Marshal.PtrToStructure(p, typeof(Intern.Process.__Internal));
          processes[i] = Intern.Process.__CreateInstance(intern);
        }
        
        Marshal.FreeHGlobal(pointer);
        
        DisposableList<Process> returnValues = new DisposableList<Process>(processes.Select(s => new Process(this, null, s)));
        return returnValues;
      }
    }
    
    internal IntPtr Pointer => _instance.__Instance;
    
    public Server()
    {
      _instance = new Intern.Server();
    }

    public Server(string name)
      : this()
    {
      Open(name);
    }

    public void Open(string name)
    {
      var ptr = _instance?.__Instance ?? IntPtr.Zero;
      NativeMethods.OpenConnection(name, ptr);
    }

    public void Dispose()
    {
      if (_instance == null) return;

      NativeMethods.CloseConnection(_instance.__Instance);
      _instance.Dispose();
    }
  }

  public class Session : IDisposable
  {
    private readonly Intern.Session _instance;
    private readonly Server _server;

    internal IntPtr Pointer => _instance.__Instance;
    
    public string UserName
    {
      get
      {
        unsafe
        {
          fixed (sbyte* value = _instance.UserName)
          {
            return new string(value);
          }
        }
      }
    }
    
    public string DomainName
    {
      get
      {
        unsafe
        {
          fixed (sbyte* value = _instance.DomainName)
          {
            return new string(value);
          }
        }
      }
    }
    
    public string HostName
    {
      get
      {
        unsafe
        {
          fixed (sbyte* value = _instance.HostName)
          {
            return new string(value);
          }
        }
      }
    }
    
    public string SessionName
    {
      get
      {
        unsafe
        {
          fixed (sbyte* value = _instance.SessionName)
          {
            return new string(value);
          }
        }
      }
    }

    public int SessionId => (int) _instance.SessionId;

    public unsafe DisposableList<Process> Processes
    {
      get
      {
        uint processCount = 0;
        NativeMethods.EnumerateProcesses(_server.Pointer, Pointer, &processCount, IntPtr.Zero);
        
        Intern.Process[] processes = new Intern.Process[processCount];
        int size = Marshal.SizeOf(typeof(Intern.Process.__Internal));
        IntPtr pointer = Marshal.AllocHGlobal((int) (size * processCount));
        NativeMethods.EnumerateProcesses(_server.Pointer, Pointer, &processCount, pointer);

        for (int i = 0; i < processCount; i++)
        {
          var p = IntPtr.Add(pointer, i * size);
          var intern = (Intern.Process.__Internal)Marshal.PtrToStructure(p, typeof(Intern.Process.__Internal));
          processes[i] = Intern.Process.__CreateInstance(intern);
        }
        
        Marshal.FreeHGlobal(pointer);
        
        DisposableList<Process> returnValues = new DisposableList<Process>(processes.Select(s => new Process(_server, this, s)));
        return returnValues;
      }
    }
    
    internal Session(Server server)
    {
      _server = server;
      _instance = new Intern.Session();
    }

    internal Session(Server server, Intern.Session session)
    {
      _server = server;
      _instance = session;
    }

    public void Logoff()
    {
      NativeMethods.LogoffSession(_server.Pointer, Pointer);
    }
    
    public void Dispose()
    {
      _instance?.Dispose();
    }
  }

  public class Process : IDisposable
  {
    private readonly Intern.Process _instance;
    private readonly Session _session;
    private readonly Server _server;

    public int SessionId => (int) _instance.SessionId;
    public int ProcessId => (int) _instance.ProcessId;
    public int NumberOfThreads => (int) _instance.NumberOfThreads;
    public int PagefileUsage => (int) _instance.PagefileUsage;
    public int WorkingSetSize => (int) _instance.WorkingSetSize;
    
    public string ProcessName
    {
      get
      {
        unsafe
        {
          fixed (sbyte* value = _instance.ProcessName)
          {
            return new string(value);
          }
        }
      }
    }
    
    internal Process(Server server, Session session)
    {
      _server = server;
      _session = session;
      _instance = new Intern.Process();
    }
    
    internal Process(Server server, Session session, Intern.Process process)
    {
      _server = server;
      _session = session;
      _instance = process;
    }

    public void Terminate()
    {
      NativeMethods.TerminateRemoteProcess(_server.Pointer, _instance.__Instance);
    }

    public void Refresh()
    {
      NativeMethods.UpdateProcessInformation(_server.Pointer, _instance.__Instance);
    }
    
    public void Dispose()
    {
      _instance?.Dispose();
    }
  }
}