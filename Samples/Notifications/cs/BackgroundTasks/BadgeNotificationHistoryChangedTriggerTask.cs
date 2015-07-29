using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace BackgroundTasks
{
    public sealed class BadgeNotificationHistoryChangedTriggerTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            var details = taskInstance.TriggerDetails as ToastNotificationHistoryChangedTriggerDetail;
            if (details == null)
                return;

            //Update the badge by taking the counter and deleting one
            IReadOnlyList<ToastNotification> TNList = ToastNotificationManager.History.GetHistory();

            BadgeNumericNotificationContent badgeContent = new BadgeNumericNotificationContent((uint)TNList.Count);

            // Send the notification to the application’s tile. Name taken from ScenarioPages.Toasts.HistoryChangedTrigger.BadgeControl
            BadgeUpdateManager.CreateBadgeUpdaterForSecondaryTile("ScenarioBadgeControl").Update(badgeContent.CreateNotification());

            // We send back the change type, the UI listens to the progress and parses the change type
            taskInstance.Progress = (uint)details.ChangeType;
        }
    }
    /// <summary>
    /// Notification content object to display a number on a tile's badge.
    /// </summary>
    public sealed class BadgeNumericNotificationContent
    {

        private uint m_Number = 0;
        /// <summary>
        /// Constructor to create a numeric badge content object with a number.
        /// </summary>
        /// <param name="number">
        /// The number that will appear on the badge.  If the number is 0, the badge
        /// will be removed.  The largest value that will appear on the badge is 99.
        /// Numbers greater than 99 are allowed, but will be displayed as "99+".
        /// </param>
        public BadgeNumericNotificationContent(uint number)
        {
            m_Number = number;
        }

        /// <summary>
        /// The number that will appear on the badge.  If the number is 0, the badge
        /// will be removed.  The largest value that will appear on the badge is 99.
        /// Numbers greater than 99 are allowed, but will be displayed as "99+".
        /// </summary>
        public uint Number
        {
            get { return m_Number; }
            set { m_Number = value; }
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public string GetContent()
        {
            return String.Format("<badge version='{0}' value='{1}'/>", Util.NOTIFICATION_CONTENT_VERSION, m_Number);
        }

        /// <summary>
        /// Retrieves the notification Xml content as a string.
        /// </summary>
        /// <returns>The notification Xml content as a string.</returns>
        public override string ToString()
        {
            return GetContent();
        }
        /// <summary>
        /// Creates a WinRT BadgeNotification object based on the content.
        /// </summary>
        /// <returns>A WinRT BadgeNotification object based on the content.</returns>
        public BadgeNotification CreateNotification()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(GetContent());
            return new BadgeNotification(xmlDoc);
        }
    }
    internal static class Util
    {
        public const int NOTIFICATION_CONTENT_VERSION = 1;

        public static string HttpEncode(string value)
        {
            return value.Replace("&", "&amp;").Replace("<", "&lt;").Replace(">", "&gt;").Replace("\"", "&quot;").Replace("'", "&apos;");
        }
    }
}
