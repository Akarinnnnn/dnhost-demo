using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ApiTest
{
	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	internal struct GetFxrParams
	{
		public nuint size;
		[MarshalAs(UnmanagedType.LPWStr)]
		public string AsmPath;
		[MarshalAs(UnmanagedType.LPWStr)]
		public string DnRoot;

		public void SetSize() { size = (nuint)UIntPtr.Size * 3; }
		public void Reset()
		{
			AsmPath = null;
			DnRoot = null;
		}
	};

	internal readonly struct HGetFxrParams
	{
		[MarshalAs(UnmanagedType.SysInt)]
		public readonly IntPtr p;
		public unsafe HGetFxrParams(ref GetFxrParams managedp)
		{
			p = (IntPtr)Unsafe.AsPointer(ref managedp);
		}

		public static HGetFxrParams Null = new();
	}

	internal static class NetHost
	{
		private const string name = "nethost";
		private const CallingConvention callconv = CallingConvention.StdCall;
		private const CharSet chart = CharSet.Unicode;
		[DllImport(name, CallingConvention = callconv, CharSet = chart,
			EntryPoint = "get_hostfxr_path", ExactSpelling = true)]
		public extern static int GetHostFxrPath(
			[MarshalAs(UnmanagedType.LPArray)]
			char[] buffer,
			ref nuint size,
			HGetFxrParams fxrparams
			);
		public static string GetFxr(ref char[] pathbuff, ref nuint size, HGetFxrParams h)
		{
#pragma warning disable CS0652 // 与整数常量比较无意义；该常量不在类型的范围之内
			if (GetHostFxrPath(pathbuff, ref size, h) == unchecked(0x80008098))
#pragma warning restore CS0652 // HRESULT
			{
				pathbuff = new char[size];
				Debug.Assert(NetHost.GetHostFxrPath(pathbuff, ref size, h) == 0);
			}
			string fxrpath = pathbuff.AsSpan().Slice(0, (int)size).ToString();
			return fxrpath;
		}

	}

}
