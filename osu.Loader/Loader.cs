using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;

namespace osu.Loader;

public static class Loader
{
	[UnmanagedCallersOnly(CallConvs = new Type[] { typeof(CallConvCdecl) })]
	public static unsafe int LoadAndExecute(int argc, char** argv, char* osuExe, int asmPathLen)
	{
		var osu = new string(osuExe, 0, asmPathLen);
		string[] args = new string[argc];
		
		for (int i = 0; i < argc; i++)
			args[i] = new string(argv[i]);

		// hack executable path
		var execpath = typeof(Application).GetField("s_executablePath", BindingFlags.Static | BindingFlags.NonPublic);
		if (execpath == null) return 7;

		execpath.SetValue(null, osu);

		return AppDomain.CurrentDomain.ExecuteAssembly(osu, args);
	}
}
