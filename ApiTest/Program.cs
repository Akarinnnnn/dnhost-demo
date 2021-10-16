using System;
using System.Reflection;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Loader;

namespace ApiTest
{
	class Program
	{
		static void Main(string[] args)
		{
			Assembly asm = AssemblyLoadContext.Default.LoadFromAssemblyPath(AppDomain.CurrentDomain.BaseDirectory + "ManagedEntry.dll");
			Type entry5_t = asm.ExportedTypes.First((t) => t.FullName == "ManagedEntry.Entry5");
			MethodInfo entryPoint = entry5_t.GetMethod("CdeclEntryPoint", BindingFlags.Static | BindingFlags.Public);
			object[] paramList = new object[] { 1, 2 };
			object ret_boxed = entryPoint.Invoke(null, paramList);
			int ret = (int)ret_boxed;
			Console.Write("returned: ");
			Console.WriteLine(ret);
		}
	}
}
