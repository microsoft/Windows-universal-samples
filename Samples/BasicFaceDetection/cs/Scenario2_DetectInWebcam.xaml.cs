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
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.FaceAnalysis;
using Windows.Media.MediaProperties;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Page for demonstrating FaceDetection on a webcam snapshot.
    /// </summary>
    public sealed partial class Scenario2_DetectInWebcam : Page
    {
        /// <summary>
        /// Reference back to the "root" page of the app.
        /// </summary>
        private MainPage rootPage = MainPage.Current;

        /// <summary>
        /// Holds the current scenario state value.
        /// </summary>
        private ScenarioState currentState;

        /// <summary>
        /// References a MediaCapture instance; is null when not in Streaming state.
        /// </summary>
        private MediaCapture mediaCapture;

        /// <summary>
        /// Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
        /// </summary>
        private VideoEncodingProperties videoProperties;

        /// <summary>
        /// References a FaceDetector instance.
        /// </summary>
        private FaceDetector faceDetector;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario2_DetectInWebcam"/> class.
        /// </summary>
        public Scenario2_DetectInWebcam()
        {
            this.InitializeComponent();

            this.currentState = ScenarioState.Idle;
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
            Streaming,

            /// <summary>
            /// Snapshot image has been captured and is being displayed along with detected faces; webcam is not active.
            /// </summary>
            Snapshot
        }

        /// <summary>
        /// Called when we navigate to this page.
        /// </summary>
        /// <param name="e">Event data</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            App.Current.Suspending += this.OnSuspending;
        }

        /// <summary>
        /// Called when we navigate away from this page.
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
        /// Initializes a new MediaCapture instance and starts the Preview streaming to the CamPreview UI element.
        /// </summary>
        /// <returns>Async Task object returning true if initialization and streaming were successful and false if an exception occurred.</returns>
        private async Task<bool> StartWebcamStreamingAsync()
        {
            bool successful = false;

            this.faceDetector = await FaceDetector.CreateAsync();

            try
            {
                this.mediaCapture = new MediaCapture();

                // For this scenario, we only need Video (not microphone) so specify this in the initializer.
                // NOTE: the appxmanifest only declares "webcam" under capabilities and if this is changed to include
                // microphone (default constructor) you must add "microphone" to the manifest or initialization will fail.
                MediaCaptureInitializationSettings settings = new MediaCaptureInitializationSettings();
                settings.StreamingCaptureMode = StreamingCaptureMode.Video;
                await this.mediaCapture.InitializeAsync(settings);
                this.mediaCapture.CameraStreamStateChanged += this.MediaCapture_CameraStreamStateChanged;

                // Cache the media properties as we'll need them later.
                var deviceController = this.mediaCapture.VideoDeviceController;
                this.videoProperties = deviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;

                // Immediately start streaming to our CaptureElement UI.
                // NOTE: CaptureElement's Source must be set before streaming is started.
                this.CamPreview.Source = this.mediaCapture;
                await this.mediaCapture.StartPreviewAsync();

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
            if (this.mediaCapture != null)
            {
                if (this.mediaCapture.CameraStreamState == Windows.Media.Devices.CameraStreamState.Streaming)
                {
                    await this.mediaCapture.StopPreviewAsync();
                }

                this.mediaCapture.Dispose();
            }

            this.CamPreview.Source = null;
            this.mediaCapture = null;
        }

        /// <summary>
        /// Captures a single frame from the running webcam stream and executes the FaceDetector on the image. If successful calls SetupVisualization to display the results.
        /// </summary>
        /// <returns>Async Task object returning true if the capture was successful and false if an exception occurred.</returns>
        private async Task<bool> TakeSnapshotAndFindFacesAsync()
        {
            if (this.currentState != ScenarioState.Streaming)
            {
                return false;
            }

            bool successful = false;

            try
            {
                // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
                // GetPreviewFrame will convert the native webcam frame into this format.
                const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat.Nv12;
                using (VideoFrame previewFrame = new VideoFrame(InputPixelFormat, (int)this.videoProperties.Width, (int)this.videoProperties.Height))
                {
                    await this.mediaCapture.GetPreviewFrameAsync(previewFrame);

                    // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
                    if (FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat))
                    {
                        IList<DetectedFace> faces = await this.faceDetector.DetectFacesAsync(previewFrame.SoftwareBitmap);

                        // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to wb's buffer.
                        // Note that WriteableBitmap doesn't support NV12 and we have to convert it to 32-bit BGRA.
                        WriteableBitmap displaySource;
                        using (SoftwareBitmap convertedSource = SoftwareBitmap.Convert(previewFrame.SoftwareBitmap, BitmapPixelFormat.Bgra8))
                        {
                            displaySource = new WriteableBitmap(convertedSource.PixelWidth, convertedSource.PixelHeight);
                            convertedSource.CopyToBuffer(displaySource.PixelBuffer);
                        }

                        // Create our display using the available image and face results.
                        this.SetupVisualization(displaySource, faces);

                        successful = true;
                    }
                    else
                    {
                        this.rootPage.NotifyUser("PixelFormat '" + InputPixelFormat.ToString() + "' is not supported by FaceDetector", NotifyType.ErrorMessage);
                    }
                }
            }
            catch (Exception ex)
            {
                this.rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                successful = false;
            }

            return successful;
        }

        /// <summary>
        /// Takes the webcam image and FaceDetector results and assembles the visualization onto the Canvas.
        /// </summary>
        /// <param name="displaySource">Bitmap object holding the image we're going to display</param>
        /// <param name="foundFaces">List of detected faces; output from FaceDetector</param>
        private void SetupVisualization(WriteableBitmap displaySource, IList<DetectedFace> foundFaces)
        {
            this.SnapshotCanvas.Background = new ImageBrush() { ImageSource = displaySource, Stretch = Stretch.Fill };
            MainPage.HighlightFaces(displaySource, foundFaces, this.SnapshotCanvas, this.HighlightedFaceBox);
        }

        /// <summary>
        /// Manages the scenario's internal state. Invokes the internal methods and updates the UI according to the
        /// passed in state value. Handles failures and resets the state if necessary.
        /// </summary>
        /// <param name="newState">State to switch to</param>
        private async Task ChangeScenarioStateAsync(ScenarioState newState)
        {
            switch (newState)
            {
                case ScenarioState.Idle:

                    this.CameraSnapshotButton.IsEnabled = false;
                    await this.ShutdownWebcamAsync();

                    this.SnapshotCanvas.Background = null;
                    this.SnapshotCanvas.Children.Clear();
                    this.CameraStreamingButton.Content = "Start Streaming";
                    this.CameraSnapshotButton.Content = "Take Snapshot";
                    this.currentState = newState;
                    break;

                case ScenarioState.Streaming:

                    if (!await this.StartWebcamStreamingAsync())
                    {
                        await this.ChangeScenarioStateAsync(ScenarioState.Idle);
                        break;
                    }

                    this.SnapshotCanvas.Background = null;
                    this.SnapshotCanvas.Children.Clear();
                    this.CameraSnapshotButton.IsEnabled = true;
                    this.CameraStreamingButton.Content = "Stop Streaming";
                    this.CameraSnapshotButton.Content = "Take Snapshot";
                    this.currentState = newState;
                    break;

                case ScenarioState.Snapshot:

                    if (!await this.TakeSnapshotAndFindFacesAsync())
                    {
                        await this.ChangeScenarioStateAsync(ScenarioState.Idle);
                        break;
                    }

                    await this.ShutdownWebcamAsync();

                    this.CameraSnapshotButton.IsEnabled = true;
                    this.CameraStreamingButton.Content = "Start Streaming";
                    this.CameraSnapshotButton.Content = "Clear Display";
                    this.currentState = newState;
                    break;
            }
        }

        /// <summary>
        /// Handles MediaCapture changes by shutting down streaming and returning to Idle state.
        /// </summary>
        /// <param name="sender">The source of the event, i.e. our MediaCapture object</param>
        /// <param name="args">Event data</param>
        private async void MediaCapture_CameraStreamStateChanged(MediaCapture sender, object args)
        {
            // MediaCapture is not Agile and so we cannot invoke its methods on this caller's thread
            // and instead need to schedule the state change on the UI thread.
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                await ChangeScenarioStateAsync(ScenarioState.Idle);
            });
        }

        /// <summary>
        /// Handles "streaming" button clicks to start/stop webcam streaming.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        private async void CameraStreamingButton_Click(object sender, RoutedEventArgs e)
        {
            this.rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
            if (this.currentState == ScenarioState.Streaming)
            {
                await this.ChangeScenarioStateAsync(ScenarioState.Idle);
            }
            else
            {
                await this.ChangeScenarioStateAsync(ScenarioState.Streaming);
            }
        }

        /// <summary>
        /// Handles "snapshot" button clicks to take a snapshot or clear the current display.
        /// </summary>
        /// <param name="sender">Button user clicked</param>
        /// <param name="e">Event data</param>
        private async void CameraSnapshotButton_Click(object sender, RoutedEventArgs e)
        {
            this.rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
            if (this.currentState == ScenarioState.Streaming)
            {
                await this.ChangeScenarioStateAsync(ScenarioState.Snapshot);
            }
            else
            {
                await this.ChangeScenarioStateAsync(ScenarioState.Idle);
            }
        }

        /// <summary>
        /// Updates any existing face bounding boxes in response to changes in the size of the Canvas.
        /// </summary>
        /// <param name="sender">Canvas whose size has changed</param>
        /// <param name="e">Event data</param>
        private void SnapshotCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // If the Canvas is resized we must recompute a new scaling factor and
            // apply it to each face box.
            ImageBrush brush = (ImageBrush)this.SnapshotCanvas.Background;
            if (brush != null)
            {
                MainPage.RepositionFaces((WriteableBitmap)brush.ImageSource, this.SnapshotCanvas);
            }
        }
    }
}
