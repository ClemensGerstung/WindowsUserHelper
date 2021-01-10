# Windows User Helper

Reimplementation of the [QUser](https://docs.microsoft.com/de-de/windows-server/administration/windows-commands/quser) and [Logoff](https://docs.microsoft.com/de-de/windows-server/administration/windows-commands/logoff) tools.  
It also provides a functionality to get the current process and thread user (i.e. if you're using `runas` to get the correct username) and also to impersonate another user.  

## Why?

Because I can and I need the functionality.  

## Why In C++ and Binding to C#?

Because it's easier to implement this stuff in C++ without marshalling everything and letting the C# code look like 💩.
And the C# binding are created by [CppSharp](https://github.com/mono/CppSharp).  

## Building

The C#.NET project must be either build in `x86` or `x64` because loading the DLLs will fail otherwise.  
Also when building `x64` with MSVC the path to `wtsapi32.lib` and `credui.lib` **must** be set maually.  