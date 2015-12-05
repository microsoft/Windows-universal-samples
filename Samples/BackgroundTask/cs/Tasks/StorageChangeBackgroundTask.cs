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
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.System.Threading;

//
// The namespace for the background tasks.
//
namespace Tasks
{
    //
    // A background task always implements the IBackgroundTask interface.
    //
    public sealed class StorageChangeBackgroundTask : IBackgroundTask
    {
        BackgroundTaskCancellationReason _cancelReason = BackgroundTaskCancellationReason.Abort;
        volatile bool _cancelRequested = false;
        BackgroundTaskDeferral _deferral = null;
        ThreadPoolTimer _periodicTimer = null;
        uint _progress = 0;
        IBackgroundTaskInstance _taskInstance = null;

        //
        // The Run method is the entry point of a background task.
        //
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("Background " + taskInstance.Task.Name + " Starting...");

            //
            // Query BackgroundWorkCost
            // Guidance: If BackgroundWorkCost is high, then perform only the minimum amount
            // of work in the background task and return immediately.
            //
            var cost = BackgroundWorkCost.CurrentBackgroundWorkCost;
            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["BackgroundWorkCost"] = cost.ToString();

            //
            // Associate a cancellation handler with the background task.
            //
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            //
            // Get the deferral object from the task instance, and take a reference to the taskInstance;
            //
            _deferral = taskInstance.GetDeferral();
            _taskInstance = taskInstance;

            _periodicTimer = ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler(PeriodicTimerCallback), TimeSpan.FromSeconds(1));

//            await ScanLibraryFilesAsync();
        }

        //
        // Handles background task cancellation.
        //
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            //
            // Indicate that the background task is canceled.
            //
            _cancelRequested = true;
            _cancelReason = reason;

            Debug.WriteLine("Background " + sender.Task.Name + " Cancel Requested...");
        }

        //
        // Simulate the background task activity.
        //
        private void PeriodicTimerCallback(ThreadPoolTimer timer)
        {
            if ((_cancelRequested == false) && (_progress < 100))
            {
                _progress += 10;
                _taskInstance.Progress = _progress;
            }
            else
            {
                _periodicTimer.Cancel();

                var settings = ApplicationData.Current.LocalSettings;
                var key = _taskInstance.Task.Name;

                //
                // Write to LocalSettings to indicate that this background task ran.
                //
                settings.Values[key] = (_progress < 100) ? "Canceled with reason: " + _cancelReason.ToString() : "Completed";
                Debug.WriteLine("Background " + _taskInstance.Task.Name + settings.Values[key]);

                //
                // Indicate that the background task has completed.
                //
                _deferral.Complete();
            }
        }

        private async Task ScanLibraryFilesAsync()
        {
            var appSettings = ApplicationData.Current.LocalSettings;
            if (!appSettings.Values.ContainsKey("LastSearchTime"))
            {
                //We haven't done a first run crawl, so lets just bail out
                return;
            }
            DateTimeOffset lastSearchTime = (DateTimeOffset)appSettings.Values["LastSearchTime"];

            var supportedExtensions = new List<String>()
            {
                ".bmp",
                ".jpg",
                ".tif",
                ".png"
            };

            StorageFolder photos = KnownFolders.PicturesLibrary;
            QueryOptions option = new QueryOptions(CommonFileQuery.OrderByDate,
                                                    supportedExtensions);
            //This is important because we are going to use indexer only properties for the query
            //later
            option.IndexerOption = IndexerOption.OnlyUseIndexer;
            option.FolderDepth = FolderDepth.Shallow;
            //Set the filter to things that have changed since the lastSearchTime. Note that the 
            //time must be formatted as Zulu time as shown here.
            string timeFilter = "System.Search.GatherTime:>=" +
                              lastSearchTime.ToString("yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z");
            option.ApplicationSearchFilter += timeFilter;


            StorageFileQueryResult resultSet = photos.CreateFileQueryWithOptions(option);
            lastSearchTime = DateTimeOffset.UtcNow;

            uint currentIndex = 0;
            const uint stepSize = 10;
            IReadOnlyList<StorageFile> files = await resultSet.GetFilesAsync(currentIndex,
                stepSize);
            currentIndex += stepSize;

            for (; files.Count != 0; files =
                await resultSet.GetFilesAsync(currentIndex, stepSize),
                currentIndex += stepSize)
            {
                foreach (StorageFile file in files)
                {
                    //Note we are assuming that the database will handle any concurrency issues
                    // do something(file);
                }
            }
            //Update the last time that we searched for files
            appSettings.Values["LastSearchTime"] = DateTimeOffset.UtcNow;
        }
    }
}
