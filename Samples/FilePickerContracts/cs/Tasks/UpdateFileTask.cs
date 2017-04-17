using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Storage.Provider;
using Windows.UI.Notifications;

namespace FilePickerContractsTasks
{
    public sealed class UpdateFileTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            ShowToast("SampleFileProviderBackgroundTask.Run fired ");

            var taskDeferral = taskInstance.GetDeferral();

            CachedFileUpdaterTriggerDetails cachedFileUpdaterTriggerDetails = (CachedFileUpdaterTriggerDetails) taskInstance.TriggerDetails;

            FileUpdateRequest fileUpdateRequest = cachedFileUpdaterTriggerDetails.UpdateRequest;

            if (cachedFileUpdaterTriggerDetails.UpdateRequest.File.Name.Contains("ConflictingFile"))
            {
                fileUpdateRequest.UserInputNeededMessage = "Need to Launch the app to resolve an update conflict.";
                fileUpdateRequest.Status = FileUpdateStatus.UserInputNeeded;
            }
            else
            {
                fileUpdateRequest.Status = FileUpdateStatus.Complete;
            }

            taskDeferral.Complete();
        }

        private void ShowToast(string message)
        {
            var toastXmlString = string.Format("<toast><visual version='1'><binding template='ToastText01'><text id='1'>{0}</text></binding></visual></toast>", message);
            var xmlDoc = new Windows.Data.Xml.Dom.XmlDocument();
            xmlDoc.LoadXml(toastXmlString);
            var toast = new ToastNotification(xmlDoc);
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }
    }
}
