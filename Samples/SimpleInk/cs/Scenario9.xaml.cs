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
using System.Collections.Generic;
using System.Linq;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// This page demonstrates the usage of the InkPresenter APIs. It shows the following functionality:
    /// - Stroke Id and StrokeStartedTime/StrokeDuration properties for each stroke collected.
    /// - Loading and saving ink files with stroke timestamps.
    /// - Using stroke timestamps to replay ink.
    /// </summary>
    public sealed partial class Scenario9 : Page
    {
        private MainPage rootPage = MainPage.Current;

        const int FPS = 60;

        DateTimeOffset beginTimeOfRecordedSession;
        DateTimeOffset endTimeOfRecordedSession;
        TimeSpan durationOfRecordedSession;
        DateTime beginTimeOfReplay;

        DispatcherTimer inkReplayTimer;

        InkStrokeBuilder strokeBuilder;
        IReadOnlyList<InkStroke> strokesToReplay;

        public Scenario9()
        {
            this.InitializeComponent();

            inkCanvas.InkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen | CoreInputDeviceTypes.Touch;
            inkCanvas.InkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            StopReplay();
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs e)
        {
            IReadOnlyList<InkStroke> strokes = e.Strokes;
            string log = $"Strokes collected: {strokes.Count}";
            foreach (InkStroke stroke in strokes)
            {
                log += $"\nId: {stroke.Id}, StartedTime: {stroke.StrokeStartedTime}, Duration: {stroke.StrokeDuration}";
            }
            rootPage.NotifyUser(log, NotifyType.StatusMessage);
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelperFunctions.UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
        }

        private void OnReset(object sender, RoutedEventArgs e)
        {
            StopReplay();
            inkCanvas.InkPresenter.StrokeContainer.Clear();
            rootPage.NotifyUser("Cleared Canvas", NotifyType.StatusMessage);
        }

        private void OnReplay(object sender, RoutedEventArgs e)
        {
            if (strokeBuilder == null)
            {
                strokeBuilder = new InkStrokeBuilder();
                inkReplayTimer = new DispatcherTimer();
                inkReplayTimer.Interval = new TimeSpan(TimeSpan.TicksPerSecond / FPS);
                inkReplayTimer.Tick += InkReplayTimer_Tick;
            }

            strokesToReplay = inkCanvas.InkPresenter.StrokeContainer.GetStrokes();

            ReplayButton.IsEnabled = false;
            inkCanvas.InkPresenter.IsInputEnabled = false;

            // Calculate the beginning of the earliest stroke and the end of the latest stroke.
            // This establishes the time period during which the strokes were collected.
            beginTimeOfRecordedSession = DateTimeOffset.MaxValue;
            endTimeOfRecordedSession = DateTimeOffset.MinValue;
            foreach (InkStroke stroke in strokesToReplay)
            {
                DateTimeOffset? startTime = stroke.StrokeStartedTime;
                TimeSpan? duration = stroke.StrokeDuration;
                if (startTime.HasValue && duration.HasValue)
                {
                    if (beginTimeOfRecordedSession > startTime.Value)
                    {
                        beginTimeOfRecordedSession = startTime.Value;
                    }
                    if (endTimeOfRecordedSession < startTime.Value + duration.Value)
                    {
                        endTimeOfRecordedSession = startTime.Value + duration.Value;
                    }
                }
            }

            // If we found at least one stroke with a timestamp, then we can replay.
            if (beginTimeOfRecordedSession != DateTimeOffset.MaxValue)
            {
                durationOfRecordedSession = endTimeOfRecordedSession - beginTimeOfRecordedSession;

                ReplayProgress.Maximum = durationOfRecordedSession.TotalMilliseconds;
                ReplayProgress.Value = 0.0;
                ReplayProgress.Visibility = Visibility.Visible;

                beginTimeOfReplay = DateTime.Now;
                inkReplayTimer.Start();

                rootPage.NotifyUser("Replay started.", NotifyType.StatusMessage);
            }
            else
            {
                // There was nothing to replay. Either there were no strokes at all,
                // or none of the strokes had timestamps.
                StopReplay();
            }
        }


        private void StopReplay()
        {
            inkReplayTimer?.Stop();

            ReplayButton.IsEnabled = true;
            inkCanvas.InkPresenter.IsInputEnabled = true;
            ReplayProgress.Visibility = Visibility.Collapsed;

        }

        private void InkReplayTimer_Tick(object sender, object e)
        {
            var currentTimeOfReplay = DateTimeOffset.Now;
            TimeSpan timeElapsedInReplay = currentTimeOfReplay - beginTimeOfReplay;

            ReplayProgress.Value = timeElapsedInReplay.TotalMilliseconds;

            DateTimeOffset timeEquivalentInRecordedSession = beginTimeOfRecordedSession + timeElapsedInReplay;
            inkCanvas.InkPresenter.StrokeContainer = GetCurrentStrokesView(timeEquivalentInRecordedSession);

            if (timeElapsedInReplay > durationOfRecordedSession)
            {
                StopReplay();
                rootPage.NotifyUser("Replay finished.", NotifyType.StatusMessage);
            }
        }

        private InkStrokeContainer GetCurrentStrokesView(DateTimeOffset time)
        {
            var inkStrokeContainer = new InkStrokeContainer();

            // The purpose of this sample is to demonstrate the timestamp usage,
            // not the algorithm. (The time complexity of the code is O(N^2).)
            foreach (InkStroke stroke in strokesToReplay)
            {
                InkStroke s = GetPartialStroke(stroke, time);
                if (s != null)
                {
                    inkStrokeContainer.AddStroke(s);
                }
            }

            return inkStrokeContainer;
        }

        private InkStroke GetPartialStroke(InkStroke stroke, DateTimeOffset time)
        {
            DateTimeOffset? startTime = stroke.StrokeStartedTime;
            TimeSpan? duration = stroke.StrokeDuration;
            if (!startTime.HasValue || !duration.HasValue)
            {
                // If a stroke does not have valid timestamp, then treat it as
                // having been drawn before the playback started.
                // We must return a clone of the stroke, because a single stroke cannot
                // exist in more than one container.
                return stroke.Clone();
            }

            if (time < startTime.Value)
            {
                // Stroke has not started
                return null;
            }

            if (time >= startTime.Value + duration.Value)
            {
                // Stroke has already ended.
                // We must return a clone of the stroke, because a single stroke cannot exist in more than one container.
                return stroke.Clone();
            }

            // Stroke has started but not yet ended.
            // Create a partial stroke on the assumption that the ink points are evenly distributed in time.
            IReadOnlyList<InkPoint> points = stroke.GetInkPoints();
            var portion = (time - startTime.Value).TotalMilliseconds / duration.Value.TotalMilliseconds;
            var count = (int)((points.Count - 1) * portion) + 1;
            return strokeBuilder.CreateStrokeFromInkPoints(points.Take(count), System.Numerics.Matrix3x2.Identity, startTime, time - startTime);
        }
    }
}
