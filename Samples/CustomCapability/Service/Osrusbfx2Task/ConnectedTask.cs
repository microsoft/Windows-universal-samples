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

using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace Osrusbfx2Task
{
    public sealed class ConnectedTask : IBackgroundTask
    {
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            string xml = $@"<toast activationType='foreground'>
                              <visual>
                                <binding template='ToastGeneric'>
                                  <text>CustomCapability Sample</text>
                                  <text>Osrfx2usb device connected.</text>
                                </binding>
                              </visual>
                            </toast>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            var toast = new ToastNotification(doc);
            ToastNotificationManager.CreateToastNotifier().Show(toast);
        }
    }
}
