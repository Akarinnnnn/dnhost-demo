namespace osu.Loader;

public static class Program
{
	public static void Main(string[] args)
	{
		MessageBox.Show("This is a loader library. It cannot be executed directly." + Environment.NewLine +
			"It's used by native loader 'osu.coreclr.exe'.", null, MessageBoxButtons.OK);
	}
}