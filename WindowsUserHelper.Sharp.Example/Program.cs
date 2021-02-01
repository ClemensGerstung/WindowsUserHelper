using System;

namespace WindowsUserHelper.Sharp.Example
{
  class Program
  {
    static void Main()
    {
      string userName = Helper.GetProcessUserName();
      Console.WriteLine(userName);

      Helper.ImpersonateUser("Please Provide Credentials", 
        "Test", 
        () => {
        userName = Helper.GetThreadUserName(true);
        Console.WriteLine(userName);
      });

      Console.ReadLine();
    }
  }
}
