using System;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace ManagedEntry
{
	[UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
	internal unsafe delegate void HostNotifyException(int errcode, char* message, char* desc);


	internal class Plugins
	{

	}


	///	<summary>entry5 = entry for .NET 5</summary>
	public class Entry5
	{
		[UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
		public static int CdeclEntryPoint(int l, int r)
		{
			AppDomain.CurrentDomain.UnhandledException += ManagedExceptionUnhandled;
			Console.WriteLine("Invoke managed plugin");
			return l + r;
		}

		/// <summary>
		/// 通知宿主发生异常
		/// </summary>
		/// <remarks>
		/// void (__cdecl* unhandled_exception_listener)(HRESULT hr, const wchar_t* message, const wchar_t* desc)
		/// </remarks>
		internal static HostNotifyException notifyHost;

		/// <param name="functionpointer">
		/// void (__cdecl* unhandled_exception_listener)(HRESULT hr, const wchar_t* message, const wchar_t* desc)
		/// </param>
		/// <returns>0成功，其它失败</returns>
		[UnmanagedCallersOnly(CallConvs = new[] {typeof(CallConvCdecl)})]
		public static unsafe int SubscribeUnhandledException(delegate* unmanaged[Cdecl]<int, char*, char*, void> functionpointer)
		{
			try
			{
				// Suppose `static class System.Runtime.Hosting` api family for hosting
				var dlg = Marshal.GetDelegateForFunctionPointer<HostNotifyException>((IntPtr)functionpointer);
				notifyHost += dlg;
				return 0;
			}
			catch
			{
				return 1;
			}
		}

		private const int E_FAIL = -2147418113;
		private const string msg_not_clr_ex = "¿不是标准CLR异常";
		private const string desc_not_clr_ex =
			"这种情况退了算了\n" +
			"怕是哪里的代码有问题\n" +
			"回头再查吧";
		private unsafe static void ManagedExceptionUnhandled(object sender, UnhandledExceptionEventArgs e)
		{
			if (e.ExceptionObject is Exception clrException)
				fixed (char* msg = clrException.Message, desc = clrException.ToString())
					notifyHost(clrException.HResult, msg, desc);
			else
				fixed (char* msg = msg_not_clr_ex, desc = desc_not_clr_ex)
					notifyHost(E_FAIL, msg, desc);
		}
	}
}
