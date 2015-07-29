//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.Devices.Sms;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace SmsBackgroundTask
{
    public sealed class SampleSmsBackgroundTask : IBackgroundTask
    {
        //
        // The Run method is the entry point of a background task.
        //
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("Background " + taskInstance.Task.Name + " Starting...");
            //
            // Associate a cancellation handler with the background task.
            //
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            //
            // Do the background task activity.
            //
            DisplayToast(taskInstance);

            //
            // Provide status to application via local settings storage
            //
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            settings.Values[taskInstance.Task.TaskId.ToString()] = "Completed";

            Debug.WriteLine("Background " + taskInstance.Task.Name + ("process ran"));
        }

        private void DisplayToast(IBackgroundTaskInstance taskInstance)
        {
            try
            {
                SmsMessageReceivedTriggerDetails smsDetails = taskInstance.TriggerDetails as SmsMessageReceivedTriggerDetails;              

                // Just registered for text messages
                SmsTextMessage2 smsTextMessage;
                if (smsDetails.MessageType == SmsMessageType.Text)
                {
                    smsTextMessage = smsDetails.TextMessage;

                    XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);

                    XmlNodeList stringElements = toastXml.GetElementsByTagName("text");

                    stringElements.Item(0).AppendChild(toastXml.CreateTextNode(smsTextMessage.From));

                    stringElements.Item(1).AppendChild(toastXml.CreateTextNode(smsTextMessage.Body));

                    ToastNotification notification = new ToastNotification(toastXml);
                    ToastNotificationManager.CreateToastNotifier().Show(notification);
                }
                else
                {
                    Debug.WriteLine("Invalid message type: " + smsDetails.MessageType);
                }

                // Ack the message
                smsDetails.Accept();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Error displaying toast: " + ex.Message);
            }
        }


        //
        // Handles background task cancellation.
        //
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            //
            // Indicate that the background task is canceled.
            //
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            settings.Values[sender.Task.TaskId.ToString()] = "Canceled";

            Debug.WriteLine("Background " + sender.Task.Name + " Cancel Requested...");
        }
    }
}
