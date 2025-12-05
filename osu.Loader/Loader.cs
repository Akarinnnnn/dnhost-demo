using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Diagnostics;

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
		args = args.Concat(["osu://b/1", "-devserver", "localhost"]).ToArray();
		args[0] = Path.GetDirectoryName(osu)!;
		args[0] = Path.Combine(args[0], "osu!.exe");

		// hack executable path
		var execpath = typeof(Application).GetField("s_executablePath", BindingFlags.Static | BindingFlags.NonPublic);
		if (execpath == null) return 7;

		execpath.SetValue(null, args[0]);

		return AppDomain.CurrentDomain.ExecuteAssembly(osu, args);
	}
}
