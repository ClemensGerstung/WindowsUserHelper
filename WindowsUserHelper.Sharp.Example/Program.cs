using System;
using System.Threading;

namespace WindowsUserHelper.Sharp.Example
{
  class Program
  {
    static void Main()
    {
      string userName = WindowsUserHelper.GetProcessUserName();
      Console.WriteLine(userName);

      

        using (Server server = new Server("localhost"))
        using(var sessions = server.Sessions)
        using(var processes = server.Processes)
        {
          foreach (var session in sessions)
          {
            using (var sessionProcesses = session.Processes)
            {
            }
          }
        }
      
      
      Console.ReadLine();
    }
  }
}