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
      Intern.Wuh.UserInformation userInformation = new Intern.Wuh.UserInformation();
      Intern.Wuh.WindowsUserHelper.GetCurrentProcessUser(userInformation);

      if (userInformation.UserNameLength == 0) return string.Empty;

      unsafe
      {
        fixed (sbyte* userName = userInformation.UserName)
        fixed (sbyte* domainName = userInformation.DomainName)
        {
          return string.Format("{0}\\{1}",
                               new string(userName),
                               new string(domainName));
        }
      }
    }

    public static string GetThreadUserName(bool isImpersonating = true)
    {
      Intern.Wuh.UserInformation userInformation = new Intern.Wuh.UserInformation();
      Intern.Wuh.WindowsUserHelper.GetCurrentThreadUser(userInformation, isImpersonating ? 1 : 0);

      if (userInformation.UserNameLength == 0) return string.Empty;

      unsafe
      {
        fixed (sbyte* userName = userInformation.UserName)
        fixed (sbyte* domainName = userInformation.DomainName)
        {
          return string.Format("{0}\\{1}",
                               new string(userName),
                               new string(domainName));
        }
      }
    }

    public static void ImpersonateUser(string message, string caption, Action action)
    {
      //unsafe
      //{
      //  void* handle = null;
      //  Intern.Wuh.Exports.ImpersonateUser(message, caption, &handle);

      //  action();

      //  Win32Native.CloseHandle(handle);
      //}
    }
  }
}
