//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.Data.Json;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace BackgroundTask
{
    static class Helpers
    {
        /// <summary>
        /// Look up a string in the saved LocalSettings, or a default value if no string is found.
        /// </summary>
        /// <param name="key">The key to look up</param>
        /// <param name="defaultValue">The fallback value to use if the key is not found or the value is not a string</param>
        /// <returns></returns>
        public static string LookupSavedString(string key, string defaultValue = "")
        {
            var values = Windows.Storage.ApplicationData.Current.LocalSettings.Values;
            object o;
            values.TryGetValue(key, out o);
            string s = o as string;
            return (s == null) ? defaultValue : s;
        }

        /// <summary>
        /// Looks up a JsonArray from the saved LocalSettings, or an empty JsonArray if not found.
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public static JsonArray LoadSavedJson(string key)
        {
            string json = LookupSavedString(key);
            if (string.IsNullOrEmpty(json))
            {
                // Not present. Return an empty array.
                return new JsonArray();
            }
            else
            {
                return JsonValue.Parse(json).GetArray();
            }
        }


        /// <summary>
        /// Helper method to display a toast notification.
        /// </summary>
        public static void DisplayToast(string message)
        {
            ToastNotifier ToastNotifier = ToastNotificationManager.CreateToastNotifier();

            // Create a two line toast and add audio reminder

            // Here the xml that will be passed to the
            // ToastNotification for the toast is retrieved
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);

            // Set both lines of text
            XmlNodeList toastNodeList = toastXml.GetElementsByTagName("text");
            toastNodeList.Item(0).AppendChild(toastXml.CreateTextNode("Geolocation Sample"));
            toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(message));

            // now create a xml node for the audio source
            IXmlNode toastNode = toastXml.SelectSingleNode("/toast");
            XmlElement audio = toastXml.CreateElement("audio");
            audio.SetAttribute("src", "ms-winsoundevent:Notification.SMS");

            ToastNotification toast = new ToastNotification(toastXml);
            ToastNotifier.Show(toast);
        }
    }
}
