using System;
using System.Collections.Generic;
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
	}
}
