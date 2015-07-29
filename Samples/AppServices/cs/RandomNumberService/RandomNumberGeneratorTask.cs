using System;
using Windows.ApplicationModel.AppService;
using Windows.ApplicationModel.Background;
using Windows.Foundation.Collections;

namespace RandomNumberService
{
    public sealed class RandomNumberGeneratorTask : IBackgroundTask
    {
        BackgroundTaskDeferral serviceDeferral;
        AppServiceConnection connection;
        Random randomNumberGenerator;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            //Take a service deferral so the service isn't terminated
            serviceDeferral = taskInstance.GetDeferral();

            taskInstance.Canceled += OnTaskCanceled;

            //Initialize the random number generator
            randomNumberGenerator = new Random((int)DateTime.Now.Ticks);

            var details = taskInstance.TriggerDetails as AppServiceTriggerDetails;
            connection = details.AppServiceConnection;

            //Listen for incoming app service requests
            connection.RequestReceived += OnRequestReceived;
        }

        private void OnTaskCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            if (serviceDeferral != null)
            {
                //Complete the service deferral
                serviceDeferral.Complete();
                serviceDeferral = null;
            }
        }

        async void OnRequestReceived(AppServiceConnection sender, AppServiceRequestReceivedEventArgs args)
        {
            //Get a deferral so we can use an awaitable API to respond to the message
            var messageDeferral = args.GetDeferral();

            try
            {
                var input = args.Request.Message;
                int minValue = (int)input["minvalue"];
                int maxValue = (int)input["maxvalue"];

                //Create the response
                var result = new ValueSet();
                result.Add("result", randomNumberGenerator.Next(minValue, maxValue));

                //Send the response
                await args.Request.SendResponseAsync(result);

            }
            finally
            {
                //Complete the message deferral so the platform knows we're done responding
                messageDeferral.Complete();
            }
        }
    }
}
