using Topshelf;

namespace CTS.Elara
{
    class Program
    {
        static void Main(string[] args)
        {
            HostFactory.Run(config =>
            {
                config.UseLog4Net("log4net.config", true);

                config.Service<ElaraService>(svr =>
                {
                    svr.ConstructUsing(name => new ElaraService());
                    svr.WhenStarted(es => es.Start());
                    svr.WhenStopped(es => es.Stop());
                });

                config.SetDescription("Elara Service hosts a gateway that enables communications between Elara Arduino board and the Internet");
                config.SetDisplayName("Elara Service");
                config.SetServiceName("ElaraService");

                config.RunAsLocalSystem();
                config.StartAutomaticallyDelayed();
            });
        }
    }
}
