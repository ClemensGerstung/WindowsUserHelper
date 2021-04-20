using System;
using System.Collections.Generic;

namespace WindowsUserHelper.Sharp
{
  public class DisposableList<T> : List<T>, IDisposable
   where T : IDisposable
  {
    public DisposableList() : base()
    {
      
    }
    
    public DisposableList(IEnumerable<T> items)
      :base(items)
    {
    }
    
    public void Dispose()
    {
      foreach (var disposable in this)
      {
        disposable?.Dispose();
      }
    }
  }
}