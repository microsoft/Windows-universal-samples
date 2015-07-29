using System.Text;
using Windows.Data.Xml.Dom;
using Windows.Foundation.Collections;
using Windows.Foundation.Metadata;
using Windows.UI.Notifications;

namespace BackgroundTasks.Helpers
{
    public static class ToastHelper
    {
        public static ToastNotification PopToast(string title, string content)
        {
            return PopToast(title, content, null, null);
        }

        public static ToastNotification PopToast(string title, string content, string tag, string group)
        {
            string xml = $@"<toast activationType='foreground'>
                                            <visual>
                                                <binding template='ToastGeneric'>
                                                </binding>
                                            </visual>
                                        </toast>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            var binding = doc.SelectSingleNode("//binding");

            var el = doc.CreateElement("text");
            el.InnerText = title;

            binding.AppendChild(el);

            el = doc.CreateElement("text");
            el.InnerText = content;
            binding.AppendChild(el);

            return PopCustomToast(doc, tag, group);
        }

        public static ToastNotification PopCustomToast(string xml)
        {
            return PopCustomToast(xml, null, null);
        }

        public static ToastNotification PopCustomToast(string xml, string tag, string group)
        {
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);


            return PopCustomToast(doc, tag, group);
        }

        [DefaultOverloadAttribute]
        public static ToastNotification PopCustomToast(XmlDocument doc, string tag, string group)
        {
            var toast = new ToastNotification(doc);

            if (tag != null)
                toast.Tag = tag;

            if (group != null)
                toast.Group = group;

            ToastNotificationManager.CreateToastNotifier().Show(toast);

            return toast;
        }

        public static string ToString(ValueSet valueSet)
        {
            StringBuilder builder = new StringBuilder();

            foreach (var pair in valueSet)
            {
                if (builder.Length != 0)
                    builder.Append('\n');

                builder.Append(pair.Key);
                builder.Append(": ");
                builder.Append(pair.Value);
            }

            return builder.ToString();
        }
    }
}
