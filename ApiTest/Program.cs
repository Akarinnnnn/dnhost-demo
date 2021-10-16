using System;
using System.Diagnostics;

namespace ApiTest
{
	class Program
	{
		private static string GetFxr(ref char[] pathbuff, ref nuint size, HGetFxrParams h)
		{
#pragma warning disable CS0652 // 与整数常量比较无意义；该常量不在类型的范围之内
			if (NetHost.GetHostFxrPath(pathbuff, ref size, h) == unchecked(0x80008098))
#pragma warning restore CS0652 // HRESULT
			{
				pathbuff = new char[size];
				Debug.Assert(NetHost.GetHostFxrPath(pathbuff, ref size, h) == 0);
			}
			string fxrpath = pathbuff.AsSpan().Slice(0, (int)size).ToString();
			return fxrpath;
		}

		static void Main(string[] args)
		{
			char[] pathbuff = new char[260];
			nuint size = 260;
			string fxrpath = GetFxr(ref pathbuff, ref size, HGetFxrParams.Null);
			Console.WriteLine(fxrpath);
			GetFxrParams fxrParams = new();
			fxrParams.SetSize();
			fxrParams.AsmPath = @"C:\Users\DSTMo\AppData\Local\osulazer\app-2021.927.0\";
			fxrpath = GetFxr(ref pathbuff, ref size, new(ref fxrParams));
			Console.WriteLine(fxrpath);
			fxrParams.Reset();
			fxrParams.DnRoot = @"C:\Program Files\dotnet\";
			Console.WriteLine(fxrpath);
		}
	}
}
