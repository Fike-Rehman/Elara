using System.Reflection;

namespace CTS.Elara
{
    using System.Threading;
    using System.Threading.Tasks;

    public class ElaraService
    {
        private static readonly log4net.ILog _logger =
                 log4net.LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        public void Start()
        {
            _logger.Error("Elara Service Started my man!");

        }

        public void Stop()
        {
            _logger.Info("Elara service stop requested!");

            

            var n = 3;
            while (n > 0)
            {
                _logger.Debug($"\rStopping application in {n} seconds");
                Thread.Sleep(1000);
                n--;
            }

        }

    }
}
