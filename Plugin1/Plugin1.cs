using Plugins;
using System;

namespace SimplePlugin
{
	public class Plugin1 : IPlugin
	{
		string inputstr;
		public void AnyAction()
		{
			Console.Write(inputstr);
			Console.WriteLine(inputstr);
		}

		public void ConsumeString(string s)
		{
			inputstr = s;
			Console.WriteLine(s);
		}
	}
}
