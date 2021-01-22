using CppSharp;
using CppSharp.AST;
using CppSharp.Generators;
using System;
using System.IO;
using System.Linq;
using System.Text;

namespace WindowsUserHelper.Sharp.Generator
{
  class Program
  {
    static void Main()
    {
      ConsoleDriver.Run(new Generator());

      Console.WriteLine("> Waiting for your input...");
      Console.ReadLine();
    }
  }

  public class Generator : ILibrary
  {
    public void Postprocess(Driver driver, ASTContext ctx)
    {

      
    }

    public void Preprocess(Driver driver, ASTContext ctx)
    {
    }

    public void Setup(Driver driver)
    {
      string projectRoot = Environment.GetEnvironmentVariable("WUH_PROJECT_ROOT");

      var options = driver.Options;
      options.Encoding = Encoding.UTF8;
      options.OutputDir = Path.Combine(projectRoot, "WindowsUserHelper.Sharp");
      options.GeneratorKind = GeneratorKind.CSharp;
      
      var module = options.AddModule("WindowsUserHelper");
      module.OutputNamespace = "WindowsUserHelper.Sharp.Intern";
      
      module.IncludeDirs.Add(Path.Combine(projectRoot, "WindowsUserHelper"));
      module.Headers.Add("WindowsUserHelper.h");
      //module.Headers.Add("SessionState.hpp");
    }

    public void SetupPasses(Driver driver)
    {
    }
  }
}
