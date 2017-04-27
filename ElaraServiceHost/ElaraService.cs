using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace CTS.Elara
{
    public class ElaraService
    {
        private static readonly log4net.ILog _logger =
                 log4net.LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        public void Start()
        {
            _logger.Debug("Elara Service Started my man!");

        }

        public void Stop()
        {

        }

    }
}
