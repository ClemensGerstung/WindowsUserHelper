using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WindowsUserHelper.Sharp
{
  internal static class Win32Native
  {
    [DllImport("kernel32.dll")]
    public static unsafe extern void CloseHandle(void* handle);
  }

  public static class Helper
  {
    public static string GetProcessUserName()
    {
      return Intern.User.Process.UserHelper.GetCurrentProcessUser();
    }

    public static string GetThreadUserName()
    {
      // provide "true" (1)
      return Intern.User.Process.UserHelper.GetCurrentThreadUser(1);
    }

    public static void ImpersonateUser(string message, string caption, Action action)
    {
      unsafe
      {
        void* handle = null;
        Intern.User.Impersonate.UserHelper.ImpersonateUser(message, caption, &handle);

        action();

        Win32Native.CloseHandle(handle);
      }
    }
  }
}
