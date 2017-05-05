using System.Reflection;
using System.Speech.Synthesis;
using System.Threading;

namespace CTS.Elara
{
    public class ElaraService
    {
        private static readonly log4net.ILog _logger =
                 log4net.LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private readonly SpeechSynthesizer speechSynthesizer;

        public ElaraService()
        {
            speechSynthesizer = new SpeechSynthesizer();
            speechSynthesizer.SetOutputToDefaultAudioDevice();
        }


        public void Start()
        {
            _logger.Debug("Elara Service Started my man!");

            speechSynthesizer.Speak("Elara Service Started!");

        }

        public void Stop()
        {
            _logger.Info("Elara Service Stop requested!");

            int n = 3;
            while (n > 0)
            {
                _logger.InfoFormat($"Stoping Elara Service in {n} seconds...");
                n--;
                Thread.Sleep(1000);
            }

        }

    }
}
