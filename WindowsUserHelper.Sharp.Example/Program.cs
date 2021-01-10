using System;

namespace WindowsUserHelper.Sharp.Example
{
  class Program
  {
    static void Main()
    {
      string userName = Helper.GetProcessUserName();
      Console.WriteLine(userName);

      Helper.ImpersonateUser("Test", "adf", () =>
      {
        string un = Helper.GetThreadUserName();
        Console.WriteLine(un);
      });

      Console.ReadLine();
    }
  }
}
