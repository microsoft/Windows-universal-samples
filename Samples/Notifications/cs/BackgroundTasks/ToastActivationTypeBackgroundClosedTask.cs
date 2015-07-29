using BackgroundTasks.Helpers;
using Windows.ApplicationModel.Background;
using Windows.UI.Notifications;

namespace BackgroundTasks
{
    public sealed class ToastActivationTypeBackgroundClosedTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            var details = taskInstance.TriggerDetails as ToastNotificationActionTriggerDetail;
            if (details == null)
            {
                ToastHelper.PopToast("ERROR", "TriggerDetails was not ToastNotificationActionTriggerDetail.");
                return;
            }

            string arguments = details.Argument;

            if (arguments == null || !arguments.Equals("quickReply"))
            {
                ToastHelper.PopToast("ERROR", $"Expected arguments to be 'quickReply' but was '{arguments}'.");
                return;
            }

            var result = details.UserInput;

            if (result.Count != 1)
                ToastHelper.PopToast("ERROR", "ERROR: Expected 1 user input value, but there were " + result.Count);

            else if (!result.ContainsKey("message"))
                ToastHelper.PopToast("ERROR", "ERROR: Expected a user input value for 'message', but there was none.");

            else if (!(result["message"] as string).Equals("Windows 10"))
                ToastHelper.PopToast("ERROR", "ERROR: User input value for 'message' was not 'Windows 10'");

            else
            {
                ToastHelper.PopToast("SUCCESS", "This scenario successfully completed. Please mark it as passed.");
            }
        }
    }
}
