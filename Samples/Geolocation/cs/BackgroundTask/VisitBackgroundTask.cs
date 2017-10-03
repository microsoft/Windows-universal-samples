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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.ApplicationModel.Background;
using Windows.Data.Json;
using Windows.Data.Xml.Dom;
using Windows.Devices.Geolocation;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.Storage;
using Windows.UI.Notifications;

namespace BackgroundTask
{
    public sealed class VisitBackgroundTask : IBackgroundTask
    {
        private ObservableCollection<string> _visitBackgroundEvents = new ObservableCollection<string>();
        private const int maxEventDescriptors = 42;                     // Value determined by how many max length event descriptors (91 chars) 
                                                                        // stored as a JSON string can fit in 8K (max allowed for local settings)

        void IBackgroundTask.Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();
            GeovisitTriggerDetails triggerDetails = (GeovisitTriggerDetails)taskInstance.TriggerDetails;

            // Handle Visit reports
            GetVisitReports(triggerDetails);
            deferral.Complete();
        }

        private void GetVisitReports(GeovisitTriggerDetails triggerDetails)
        {
            // First fill the _visitBackgroundEvents with previous existing reports saved in local settings.
            _visitBackgroundEvents.Clear();
            FillReportCollectionWithExistingReports();

            // Read reports from the triggerDetails.
            IReadOnlyList<Geovisit> reports = triggerDetails.ReadReports();

            string visitItemEvent = null;
            int numReports = reports.Count;

            DateTimeFormatter formatterLongTime = new DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", new[] { "en-US" }, "US", CalendarIdentifiers.Gregorian, ClockIdentifiers.TwentyFourHour);

            foreach (Geovisit report in reports)
            {
                visitItemEvent = formatterLongTime.Format(report.Timestamp);

                visitItemEvent += " " + report.StateChange.ToString();

                // Check if the report has a valid position.
                if (report.Position != null)
                {
                    visitItemEvent += " (" +
                    report.Position.Coordinate.Point.Position.Latitude.ToString() + "," +
                    report.Position.Coordinate.Point.Position.Longitude.ToString() +
                    ")";
                }

                // Now add each new visit report to _VisitBackgroundEvents.
                AddVisitReport(visitItemEvent);
            }

            if (numReports != 0)
            {
                // Save all the reports back to the local settings.
                SaveExistingReports();

                // NOTE: Other notification mechanisms can be used here, such as Badge and/or Tile updates.
                DoToast(numReports, visitItemEvent);
            }

        }

        /// <summary>
        /// Helper method to pop a toast
        /// </summary>
        private void DoToast(int numEventsOfInterest, string eventName)
        {
            // pop a toast for each visit event
            ToastNotifier ToastNotifier = ToastNotificationManager.CreateToastNotifier();

            // Create a two line toast and add audio reminder

            // Here the xml that will be passed to the 
            // ToastNotification for the toast is retrieved
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);

            // Set both lines of text
            XmlNodeList toastNodeList = toastXml.GetElementsByTagName("text");
            toastNodeList.Item(0).AppendChild(toastXml.CreateTextNode("Geolocation Sample"));

            if (1 == numEventsOfInterest)
            {
                toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(eventName));
            }
            else
            {
                string secondLine = "There are " + numEventsOfInterest + " new visit events";
                toastNodeList.Item(1).AppendChild(toastXml.CreateTextNode(secondLine));
            }

            // now create a xml node for the audio source
            IXmlNode toastNode = toastXml.SelectSingleNode("/toast");
            XmlElement audio = toastXml.CreateElement("audio");
            audio.SetAttribute("src", "ms-winsoundevent:Notification.SMS");

            ToastNotification toast = new ToastNotification(toastXml);
            ToastNotifier.Show(toast);
        }

        private void FillReportCollectionWithExistingReports()
        {
            var settings = ApplicationData.Current.LocalSettings;
            if (settings.Values.ContainsKey("BackgroundVisitEventCollection"))
            {
                string visitEvent = settings.Values["BackgroundVisitEventCollection"].ToString();

                if (0 != visitEvent.Length)
                {
                    var events = JsonValue.Parse(visitEvent).GetArray();

                    // NOTE: the events are accessed in reverse order
                    // because the events were added to JSON from
                    // newer to older.  AddVisitReport() adds
                    // each new entry to the beginning of the collection
                    // and throws away the last entry if the collection gets too large.
                    for (int pos = events.Count - 1; pos >= 0; pos--)
                    {
                        var element = events.GetStringAt((uint)pos);
                        AddVisitReport(element);
                    }
                }
            }
        }

        private void SaveExistingReports()
        {
            var jsonArray = new JsonArray();

            foreach (var eventDescriptor in _visitBackgroundEvents)
            {
                jsonArray.Add(JsonValue.CreateStringValue(eventDescriptor.ToString()));
            }

            string jsonString = jsonArray.Stringify();

            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["BackgroundVisitEventCollection"] = jsonString;
        }

        private void AddVisitReport(string eventDescription)
        {
            if (_visitBackgroundEvents.Count == maxEventDescriptors)
            {
                _visitBackgroundEvents.RemoveAt(maxEventDescriptors - 1);
            }

            _visitBackgroundEvents.Insert(0, eventDescription);
        }

    }
}
