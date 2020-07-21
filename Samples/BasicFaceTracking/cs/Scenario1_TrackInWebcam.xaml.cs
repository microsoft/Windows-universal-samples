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
using System.Threading;
using System.Threading.Tasks;

using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.FaceAnalysis;
using Windows.Media.MediaProperties;
using Windows.System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

namespace SDKTemplate
{
    /// <summary>
    /// Page for demonstrating FaceTracking.
    /// </summary>
    public sealed partial class Scenario1_TrackInWebcam : Page
    {
        /// <summary>
        /// Reference back to the "root" page of the app.
        /// </summary>
        private MainPage rootPage = MainPage.Current;

        /// <summary>
        /// Holds the current scenario state value.
        /// </summary>
        private ScenarioState currentState = ScenarioState.Idle;

        /// <summary>
        /// References a MediaCapture instance; is null when not in Streaming state.
        /// </summary>
        private MediaCapture mediaCapture;

        /// <summary>
        /// Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
        /// </summary>
        private VideoEncodingProperties videoProperties;

        /// <summary>
        /// References a FaceTracker instance.
        /// </summary>
        private FaceTracker faceTracker;

        /// <summary>
        /// A periodic timer to execute FaceTracker on preview frames
        /// </summary>
        private ThreadPoolTimer frameProcessingTimer;

        /// <summary>
        /// Flag to ensure FaceTracking logic only executes one at a time
        /// </summary>
        private int busy = 0;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_TrackInWebcam"/> class.
        /// </summary>
        public Scenario1_TrackInWebcam()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Values for identifying and controlling scenario states.
        /// </summary>
        private enum ScenarioState
        {
            /// <summary>
            /// Display is blank - default state.
            /// </summary>
            Idle,

            /// <summary>
            /// Webcam is actively engaged and a live video stream is displayed.
            /// </summary>
            Streaming
        }

        /// <summary>
        /// Responds when we navigate to this page.
        /// </summary>
        /// <param name="e">Event data</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            App.Current.Suspending += this.OnSuspending;
        }

        /// <summary>
        /// Responds when we navigate away from this page.
        /// </summary>
        /// <param name="e">Event data</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            App.Current.Suspending -= this.OnSuspending;
        }

        /// <summary>
        /// Responds to App Suspend event to stop/release MediaCapture object if it's running and return to Idle state.
        /// </summary>
        /// <param name="sender">The source of the Suspending event</param>
        /// <param name="e">Event data</param>
        private async void OnSuspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            if (this.currentState == ScenarioState.Streaming)
            {
                var deferral = e.SuspendingOperation.GetDeferral();
                try
                {
                    await this.ChangeScenarioStateAsync(ScenarioState.Idle);
                }
                finally
                {
                    deferral.Complete();
                }
            }
        }

        /// <summary>
        /// Creates the FaceTracker object which we will use for face detection and tracking.
        /// Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
        /// </summary>
        /// <returns>Async Task object returning true if initialization and streaming were successful and false if an exception occurred.</returns>
        private async Task<bool> StartWebcamStreamingAsync()
        {
            bool successful = false;

            faceTracker = await FaceTracker.CreateAsync();

            try
            {
                this.mediaCapture = new MediaCapture();

                // For this scenario, we only need Video (not microphone) so specify this in the initializer.
                // NOTE: the appxmanifest only declares "webcam" under capabilities and if this is changed to include
                // microphone (default constructor) you must add "microphone" to the manifest or initialization will fail.
                MediaCaptureInitializationSettings settings = new MediaCaptureInitializationSettings();
                settings.StreamingCaptureMode = StreamingCaptureMode.Video;
                await this.mediaCapture.InitializeAsync(settings);
                this.mediaCapture.Failed += this.MediaCapture_CameraStreamFailed;

                // Cache the media properties as we'll need them later.
                var deviceController = this.mediaCapture.VideoDeviceController;
                this.videoProperties = deviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;

                // Immediately start streaming to our CaptureElement UI.
                // NOTE: CaptureElement's Source must be set before streaming is started.
                this.CamPreview.Source = this.mediaCapture;
                await this.mediaCapture.StartPreviewAsync();

                // Run the timer at 66ms, which is approximately 15 frames per second.
                TimeSpan timerInterval = TimeSpan.FromMilliseconds(66);
                this.frameProcessingTimer = ThreadPoolTimer.CreatePeriodicTimer(ProcessCurrentVideoFrame, timerInterval);
                successful = true;
            }
            catch (System.UnauthorizedAccessException)
            {
                // If the user has disabled their webcam this exception is thrown; provide a descriptive message to inform the user of this fact.
                this.rootPage.NotifyUser("Webcam is disabled or access to the webcam is disabled for this app.\nEnsure Privacy Settings allow webcam usage.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                this.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }

            return successful;
        }

        /// <summary>
        /// Safely stops webcam streaming (if running) and releases MediaCapture object.
        /// </summary>
        private async Task ShutdownWebcamAsync()
        {
            if(this.frameProcessingTimer != null)
            {
                this.frameProcessingTimer.Cancel();
            }

            if (this.mediaCapture != null)
            {
                if (this.mediaCapture.CameraStreamState == Windows.Media.Devices.CameraStreamState.Streaming)
                {
                    try
                    {
                        await this.mediaCapture.StopPreviewAsync();
                    }
                    catch(Exception)
                    {
                        ;   // Since we're going to destroy the MediaCapture object there's nothing to do here
                    }
                }
                this.mediaCapture.Dispose();
            }

            this.frameProcessingTimer = null;
            this.CamPreview.Source = null;
            this.mediaCapture = null;
            this.CameraStreamingButton.IsEnabled = true;
        }

        /// <summary>
        /// This method is invoked by a ThreadPoolTimer to execute the FaceTracker and Visualization logic.
        /// </summary>
        /// <param name="timer">Timer object invoking this call</param>
        private async void ProcessCurrentVideoFrame(ThreadPoolTimer timer)
        {
            if (this.currentState != ScenarioState.Streaming)
            {
                return;
            }

            // If busy is already 1, then the previous frame is still being processed,
            // in which case we skip the current frame.
            if (Interlocked.CompareExchange(ref busy, 1, 0) != 0)
            {
                return;
            }

            await ProcessCurrentVideoFrameAsync();
            Interlocked.Exchange(ref busy, 0);
        }

        /// <summary>
        /// This method is called to execute the FaceTracker and Visualization logic at each timer tick.
        /// </summary>
        /// <remarks>
        /// Keep in mind this method is called from a Timer and not synchronized with the camera stream. Also, the processing time of FaceTracker
        /// will vary depending on the size of each frame and the number of faces being tracked. That is, a large image with several tracked faces may
        /// take longer to process.
        /// </remarks>
        private async Task ProcessCurrentVideoFrameAsync()
        {
            // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
            // GetPreviewFrame will convert the native webcam frame into this format.
            const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat.Nv12;
            using (VideoFrame previewFrame = new VideoFrame(InputPixelFormat, (int)this.videoProperties.Width, (int)this.videoProperties.Height))
            {
                try
                {
                    await this.mediaCapture.GetPreviewFrameAsync(previewFrame);
                }
                catch (UnauthorizedAccessException)
                {
                    // Lost access to the camera.
                    AbandonStreaming();
                    return;
                }
                catch (Exception)
                {
                    this.rootPage.NotifyUser($"PreviewFrame with format '{InputPixelFormat}' is not supported by your Webcam", NotifyType.ErrorMessage);
                    return;
                }

                // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
                if (!FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat))
                {
                    this.rootPage.NotifyUser($"PixelFormat '{previewFrame.SoftwareBitmap.BitmapPixelFormat}' is not supported by FaceDetector", NotifyType.ErrorMessage);
                    return;
                }

                IList<DetectedFace> faces;
                try
                {
                    faces = await this.faceTracker.ProcessNextFrameAsync(previewFrame);
                }
                catch (Exception ex)
                {
                    this.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                    return;
                }

                // Create our visualization using the frame dimensions and face results but run it on the UI thread.
                var previewFrameSize = new Windows.Foundation.Size(previewFrame.SoftwareBitmap.PixelWidth, previewFrame.SoftwareBitmap.PixelHeight);
                var ignored = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    this.SetupVisualization(previewFrameSize, faces);
                });
            }
        }

        /// <summary>
        /// Takes the webcam image and FaceTracker results and assembles the visualization onto the Canvas.
        /// </summary>
        /// <param name="framePizelSize">Width and height (in pixels) of the video capture frame</param>
        /// <param name="foundFaces">List of detected faces; output from FaceTracker</param>
        private void SetupVisualization(Windows.Foundation.Size framePixelSize, IList<DetectedFace> foundFaces)
        {
            this.VisualizationCanvas.Children.Clear();

            if (this.currentState == ScenarioState.Streaming && framePixelSize.Width != 0.0 && framePixelSize.Height != 0.0)
            {
                double widthScale = this.VisualizationCanvas.ActualWidth / framePixelSize.Width;
                double heightScale = this.VisualizationCanvas.ActualHeight / framePixelSize.Height;

                foreach (DetectedFace face in foundFaces)
                {
                    // Create a rectangle element for displaying the face box but since we're using a Canvas
                    // we must scale the rectangles according to the frames's actual size.
                    Rectangle box = new Rectangle()
                    {
                        Width = face.FaceBox.Width * widthScale,
                        Height = face.FaceBox.Height * heightScale,
                        Margin = new Thickness(face.FaceBox.X * widthScale, face.FaceBox.Y * heightScale, 0, 0),
                        Style = HighlightedFaceBoxStyle
                    };
                    this.VisualizationCanvas.Children.Add(box);
                }
            }
        }

        /// <summary>
        /// Manages the scenario's internal state. Invokes the internal methods and updates the UI according to the
        /// passed in state value. Handles failures and resets the state if necessary.
        /// </summary>
        /// <param name="newState">State to switch to</param>
        private async Task ChangeScenarioStateAsync(ScenarioState newState)
        {
            // Disable UI while state change is in progress
            this.CameraStreamingButton.IsEnabled = false;

            switch (newState)
            {
                case ScenarioState.Idle:

                    this.currentState = newState;
                    await this.ShutdownWebcamAsync();

                    this.VisualizationCanvas.Children.Clear();
                    this.CameraStreamingButton.Content = "Start Streaming";
                    break;

                case ScenarioState.Streaming:

                    if (!await this.StartWebcamStreamingAsync())
                    {
                        await this.ChangeScenarioStateAsync(ScenarioState.Idle);
                        break;
                    }

                    this.VisualizationCanvas.Children.Clear();
                    this.CameraStreamingButton.Content = "Stop Streaming";
                    this.currentState = newState;
                    this.CameraStreamingButton.IsEnabled = true;
                    break;
            }
        }

        private void AbandonStreaming()
        {
            // MediaCapture is not Agile and so we cannot invoke its methods on this caller's thread
            // and instead need to schedule the state change on the UI thread.
            var ignored = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                await ChangeScenarioStateAsync(ScenarioState.Idle);
            });
        }

        /// <summary>
        /// Handles MediaCapture stream failures by shutting down streaming and returning to Idle state.
        /// </summary>
        /// <param name="sender">The source of the event, i.e. our MediaCapture object</param>
        /// <param name="args">Event data</param>
        private void MediaCapture_CameraStreamFailed(MediaCapture sender, object args)
        {
            AbandonStreaming();
        }

        /// <summary>
        /// Handles "streaming" button clicks to start/stop webcam streaming.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        private async void CameraStreamingButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.currentState == ScenarioState.Streaming)
            {
                this.rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
                await this.ChangeScenarioStateAsync(ScenarioState.Idle);
            }
            else
            {
                this.rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
                await this.ChangeScenarioStateAsync(ScenarioState.Streaming);
            }
        }
    }
}
