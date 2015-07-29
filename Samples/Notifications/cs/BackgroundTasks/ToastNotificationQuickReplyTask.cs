using BackgroundTasks.Helpers;
using Windows.ApplicationModel.Background;
using Windows.UI.Notifications;

namespace BackgroundTasks
{
    public sealed class ToastNotificationQuickReplyTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            var details = taskInstance.TriggerDetails as ToastNotificationActionTriggerDetail;
            if (details == null)
            {
                BackgroundTaskStorage.PutError("TriggerDetails was not ToastNotificationActionTriggerDetail.");
                return;
            }

            string arguments = details.Argument;

            if (arguments == null || !arguments.Equals("quickReply"))
            {
                BackgroundTaskStorage.PutError($"Expected arguments to be 'quickReply' but was '{arguments}'.");
                return;
            }

            BackgroundTaskStorage.PutAnswer(BackgroundTaskStorage.ConvertValueSetToApplicationDataCompositeValue(details.UserInput));

            //object obj;

            //details.UserInput.TryGetValue("message", out obj);

            //string message = obj as string;

            //if (message == null)
            //{
            //    BackgroundTaskStorage.PutError("Expected there to be a UserInput value at 'message', but there was none.");
            //    return;
            //}

            //BackgroundTaskStorage.PutAnswer(message);
        }
    }
}
