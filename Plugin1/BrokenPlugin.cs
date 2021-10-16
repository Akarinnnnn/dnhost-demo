using Plugins;
using System;
using System.Runtime.CompilerServices;

namespace SimplePlugin
{
	public class BrokenPlugin : IPlugin
	{
		public void AnyAction()
		{
			throw new NotImplementedException();
		}

		public void ConsumeString(string s)
		{
			throw new NotImplementedException();
		}
	}
}
