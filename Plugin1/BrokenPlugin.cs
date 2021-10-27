using Plugins;
using System;
using System.ComponentModel.Composition;

namespace SimplePlugin
{
	[Export(typeof(IPlugin))]
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
