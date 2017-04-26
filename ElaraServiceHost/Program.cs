using Topshelf;

namespace CTS.Elara
{
    class Program
    {
        static void Main(string[] args)
        {
            HostFactory.Run(config =>
            {
                config.Service<ElaraService>(svr =>
                {
                    svr.ConstructUsing(name => new ElaraService());
                    svr.WhenStarted(tc => tc.Start());
                    svr.WhenStopped(tc => tc.Stop());
                });

                config.RunAsLocalSystem();

                config.SetDescription("Elara Service hosts a gateway that enables communications between Elara Arduino board and the Internet");
                config.SetDisplayName("Elara Service");
                config.SetServiceName("ElaraService");
            });
        }
    }
}
