# Windows User Helper

Reimplementation of the [QUser](https://docs.microsoft.com/de-de/windows-server/administration/windows-commands/quser) and [Logoff](https://docs.microsoft.com/de-de/windows-server/administration/windows-commands/logoff) tools.  
It also provides a functionality to get the current process and thread user (i.e. if you're using `runas` to get the correct username) and also to impersonate another user.  

## Why?

Because I can and I need the functionality.  

## Why In C++ and Binding to C#?

Because it's easier to implement this stuff in C++ without marshalling everything and letting the C# code look like 💩.
And the C# ~~binding~~ classes are created by [CppSharp](https://github.com/mono/CppSharp) and the Methods are linked manually.  
The linkage of the methods is done like in [NNanomsg's `Interop.cs`](https://github.com/mhowlett/NNanomsg/blob/master/NNanomsg/Interop.cs).  

## Building

The C#.NET project must be either build in `x86` or `x64` because loading the DLLs will fail otherwise.  
Also when building `x64` with MSVC the path to `wtsapi32.lib` and `credui.lib` **must** be set manually.

## Usage

### Getting the username of current process

```c#
string userName = WindowsUserHelper.GetProcessUserName();
```

### Getting the username of current thread

> **Note:** this can only be used if the current thread impersonated, i.e. if calling this directly nothing will happen

```c#
string userName = WindowsUserHelper.GetThreadUserName(true);
```

### Connecting to a Windows computer to get processes or sessions

```c#
using(Server server = new Server("localhost"))
{
    // ...
}
```

### Getting all sessions of a Windows computer

```c#
using(Server server = new Server("localhost"))
using(var sessions = server.Sessions)
{
  // ...
}
```

### Getting all processes of a Windows computer

```c#
using(Server server = new Server("localhost"))
using(var processes = server.Processes)
{
  // ...
}
```

### Getting processes of all sessions of a Windows computer

```c#
using(Server server = new Server("localhost"))
using(var sessions = server.Sessions)
{
  foreach (var session in sessions)
  {
    using (var sessionProcesses = session.Processes)
    {
      // ...
    }
  }
}
```