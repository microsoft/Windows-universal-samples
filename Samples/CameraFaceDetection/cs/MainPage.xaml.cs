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
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Graphics.Display;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Phone.UI.Input;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;
using Windows.Media.FaceAnalysis;
using Windows.UI;
using System.Collections.Generic;

namespace FaceDetection
{

    public sealed partial class MainPage : Page
    {
        // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls
        private readonly DisplayInformation _displayInformation = DisplayInformation.GetForCurrentView();
        private readonly SimpleOrientationSensor _orientationSensor = SimpleOrientationSensor.GetDefault();
        private SimpleOrientation _deviceOrientation = SimpleOrientation.NotRotated;
        private DisplayOrientations _displayOrientation = DisplayOrientations.Portrait;

        // Rotation metadata to apply to the preview stream and recorded videos (MF_MT_VIDEO_ROTATION)
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        private static readonly Guid RotationKey = new Guid("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        // Folder in which the captures will be stored (initialized in SetupUiAsync)
        private StorageFolder _captureFolder = null;

        // Prevent the screen from sleeping while the camera is running
        private readonly DisplayRequest _displayRequest = new DisplayRequest();

        // For listening to media property changes
        private readonly SystemMediaTransportControls _systemMediaControls = SystemMediaTransportControls.GetForCurrentView();

        // MediaCapture and its state variables
        private MediaCapture _mediaCapture;
        private IMediaEncodingProperties _previewProperties;
        private bool _isInitialized;
        private bool _isRecording;

        // Information about the camera device
        private bool _mirroringPreview;
        private bool _externalCamera;

        private FaceDetectionEffect _faceDetectionEffect;

        #region Constructor, lifecycle and navigation

        public MainPage()
        {
            this.InitializeComponent();

            // Do not cache the state of the UI when suspending/navigating
            NavigationCacheMode = NavigationCacheMode.Disabled;

            // Useful to know when to initialize/clean up the camera
            Application.Current.Suspending += Application_Suspending;
            Application.Current.Resuming += Application_Resuming;
        }

        private async void Application_Suspending(object sender, SuspendingEventArgs e)
        {
            // Handle global application events only if this page is active
            if (Frame.CurrentSourcePageType == typeof(MainPage))
            {
                var deferral = e.SuspendingOperation.GetDeferral();

                await CleanupCameraAsync();

                await CleanupUiAsync();

                deferral.Complete();
            }
        }

        private async void Application_Resuming(object sender, object o)
        {
            // Handle global application events only if this page is active
            if (Frame.CurrentSourcePageType == typeof(MainPage))
            {
                await SetupUiAsync();

                await InitializeCameraAsync();
            }
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await SetupUiAsync();

            await InitializeCameraAsync();
        }

        protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Handling of this event is included for completenes, as it will only fire when navigating between pages and this sample only includes one page

            await CleanupCameraAsync();

            await CleanupUiAsync();
        }

        #endregion Constructor, lifecycle and navigation


        #region Event handlers

        /// <summary>
        /// In the event of the app being minimized this method handles media property change events. If the app receives a mute
        /// notification, it is no longer in the foregroud.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void SystemMediaControls_PropertyChanged(SystemMediaTransportControls sender, SystemMediaTransportControlsPropertyChangedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                // Only handle this event if this page is currently being displayed
                if (args.Property == SystemMediaTransportControlsProperty.SoundLevel && Frame.CurrentSourcePageType == typeof(MainPage))
                {
                    // Check to see if the app is being muted. If so, it is being minimized.
                    // Otherwise if it is not initialized, it is being brought into focus.
                    if (sender.SoundLevel == SoundLevel.Muted)
                    {
                        await CleanupCameraAsync();
                    }
                    else if (!_isInitialized)
                    {
                        await InitializeCameraAsync();
                    }
                }
            });
        }

        /// <summary>
        /// Occurs each time the simple orientation sensor reports a new sensor reading.
        /// </summary>
        /// <param name="sender">The event source.</param>
        /// <param name="args">The event data.</param>
        private async void OrientationSensor_OrientationChanged(SimpleOrientationSensor sender, SimpleOrientationSensorOrientationChangedEventArgs args)
        {
            if (args.Orientation != SimpleOrientation.Faceup && args.Orientation != SimpleOrientation.Facedown)
            {
                // Only update the current orientation if the device is not parallel to the ground. This allows users to take pictures of documents (FaceUp)
                // or the ceiling (FaceDown) in portrait or landscape, by first holding the device in the desired orientation, and then pointing the camera
                // either up or down, at the desired subject.
                //Note: This assumes that the camera is either facing the same way as the screen, or the opposite way. For devices with cameras mounted
                //      on other panels, this logic should be adjusted.
                _deviceOrientation = args.Orientation;

                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateButtonOrientation());
            }
        }

        /// <summary>
        /// This event will fire when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the SetupUiAsync() method cannot be not honored.
        /// </summary>
        /// <param name="sender">The event source.</param>
        /// <param name="args">The event data.</param>
        private async void DisplayInformation_OrientationChanged(DisplayInformation sender, object args)
        {
            _displayOrientation = sender.CurrentOrientation;

            if (_previewProperties != null)
            {
                await SetPreviewRotationAsync();
            }

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateButtonOrientation());
        }

        private async void PhotoButton_Click(object sender, RoutedEventArgs e)
        {
            await TakePhotoAsync();
        }

        private async void VideoButton_Click(object sender, RoutedEventArgs e)
        {
            if (!_isRecording)
            {
                await StartRecordingAsync();
            }
            else
            {
                await StopRecordingAsync();
            }

            // After starting or stopping video recording, update the UI to reflect the MediaCapture state
            UpdateCaptureControls();
        }

        private async void FaceDetectionButton_Click(object sender, RoutedEventArgs e)
        {
            if (_faceDetectionEffect == null || !_faceDetectionEffect.Enabled)
            {
                // Clear any rectangles that may have been left over from a previous instance of the effect
                FacesCanvas.Children.Clear();

                await CreateFaceDetectionEffectAsync();
            }
            else
            {
                await CleanUpFaceDetectionEffectAsync();
            }

            UpdateCaptureControls();
        }

        private async void HardwareButtons_CameraPressed(object sender, CameraEventArgs e)
        {
            await TakePhotoAsync();
        }

        private async void MediaCapture_RecordLimitationExceeded(MediaCapture sender)
        {
            // This is a notification that recording has to stop, and the app is expected to finalize the recording

            await StopRecordingAsync();

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateCaptureControls());
        }

        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);

            await CleanupCameraAsync();

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateCaptureControls());
        }

        private async void FaceDetectionEffect_FaceDetected(FaceDetectionEffect sender, FaceDetectedEventArgs args)
        {
            // Ask the UI thread to render the face bounding boxes
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => HighlightDetectedFaces(args.ResultFrame.DetectedFaces));
        }

        #endregion Event handlers


        #region MediaCapture methods

        /// <summary>
        /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
        /// </summary>
        /// <returns></returns>
        private async Task InitializeCameraAsync()
        {
            Debug.WriteLine("InitializeCameraAsync");

            if (_mediaCapture == null)
            {
                // Attempt to get the front camera if one is available, but use any camera device if not
                var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Front);

                if (cameraDevice == null)
                {
                    Debug.WriteLine("No camera device found!");
                    return;
                }

                // Create MediaCapture and its settings
                _mediaCapture = new MediaCapture();

                // Register for a notification when video recording has reached the maximum time and when something goes wrong
                _mediaCapture.RecordLimitationExceeded += MediaCapture_RecordLimitationExceeded;
                _mediaCapture.Failed += MediaCapture_Failed;

                var settings = new MediaCaptureInitializationSettings { VideoDeviceId = cameraDevice.Id };

                // Initialize MediaCapture
                try
                {
                    await _mediaCapture.InitializeAsync(settings);
                    _isInitialized = true;
                }
                catch (UnauthorizedAccessException)
                {
                    Debug.WriteLine("The app was denied access to the camera");
                }

                // If initialization succeeded, start the preview
                if (_isInitialized)
                {
                    // Figure out where the camera is located
                    if (cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown)
                    {
                        // No information on the location of the camera, assume it's an external camera, not integrated on the device
                        _externalCamera = true;
                    }
                    else
                    {
                        // Camera is fixed on the device
                        _externalCamera = false;

                        // Only mirror the preview if the camera is on the front panel
                        _mirroringPreview = (cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
                    }

                    await StartPreviewAsync();

                    UpdateCaptureControls();
                }
            }
        }

        /// <summary>
        /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
        /// </summary>
        /// <returns></returns>
        private async Task StartPreviewAsync()
        {
            // Prevent the device from sleeping while the preview is running
            _displayRequest.RequestActive();

            // Set the preview source in the UI and mirror it if necessary
            PreviewControl.Source = _mediaCapture;
            PreviewControl.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            // Start the preview
            await _mediaCapture.StartPreviewAsync();
            _previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);

            // Initialize the preview to the current orientation
            if (_previewProperties != null)
            {
                _displayOrientation = _displayInformation.CurrentOrientation;

                await SetPreviewRotationAsync();
            }
        }

        /// <summary>
        /// Gets the current orientation of the UI in relation to the device (when AutoRotationPreferences cannot be honored) and applies a corrective rotation to the preview
        /// </summary>
        private async Task SetPreviewRotationAsync()
        {
            // Only need to update the orientation if the camera is mounted on the device
            if (_externalCamera) return;

            // Calculate which way and how far to rotate the preview
            int rotationDegrees = ConvertDisplayOrientationToDegrees(_displayOrientation);

            // The rotation direction needs to be inverted if the preview is being mirrored
            if (_mirroringPreview)
            {
                rotationDegrees = (360 - rotationDegrees) % 360;
            }

            // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
            var props = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);
            props.Properties.Add(RotationKey, rotationDegrees);
            await _mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, props, null);
        }

        /// <summary>
        /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
        /// </summary>
        /// <returns></returns>
        private async Task StopPreviewAsync()
        {
            // Stop the preview
            _previewProperties = null;
            await _mediaCapture.StopPreviewAsync();

            // Use the dispatcher because this method is sometimes called from non-UI threads
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Cleanup the UI
                PreviewControl.Source = null;

                // Allow the device screen to sleep now that the preview is stopped
                _displayRequest.RequestRelease();
            });
        }

        /// <summary>
        /// Adds face detection to the preview stream, registers for its events, enables it, and gets the FaceDetectionEffect instance
        /// </summary>
        /// <returns></returns>
        private async Task CreateFaceDetectionEffectAsync()
        {
            // Create the definition, which will contain some initialization settings
            var definition = new FaceDetectionEffectDefinition();

            // To ensure preview smoothness, do not delay incoming samples
            definition.SynchronousDetectionEnabled = false;

            // In this scenario, choose detection speed over accuracy
            definition.DetectionMode = FaceDetectionMode.HighPerformance;

            // Add the effect to the preview stream
            _faceDetectionEffect = (FaceDetectionEffect)await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview);

            // Register for face detection events
            _faceDetectionEffect.FaceDetected += FaceDetectionEffect_FaceDetected;

            // Choose the shortest interval between detection events
            _faceDetectionEffect.DesiredDetectionInterval = TimeSpan.FromMilliseconds(33);

            // Start detecting faces
            _faceDetectionEffect.Enabled = true;
        }

        /// <summary>
        ///  Disables and removes the face detection effect, and unregisters the event handler for face detection
        /// </summary>
        /// <returns></returns>
        private async Task CleanUpFaceDetectionEffectAsync()
        {
            // Disable detection
            _faceDetectionEffect.Enabled = false;

            // Unregister the event handler
            _faceDetectionEffect.FaceDetected -= FaceDetectionEffect_FaceDetected;

            // Remove the effect (see ClearEffectsAsync method to remove all effects from a stream)
            await _mediaCapture.RemoveEffectAsync(_faceDetectionEffect);

            // Clear the member variable that held the effect instance
            _faceDetectionEffect = null;
        }

        /// <summary>
        /// Takes a photo to a StorageFile and adds rotation metadata to it
        /// </summary>
        /// <returns></returns>
        private async Task TakePhotoAsync()
        {
            // While taking a photo, keep the video button enabled only if the camera supports simultaneously taking pictures and recording video
            VideoButton.IsEnabled = _mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported;

            // Make the button invisible if it's disabled, so it's obvious it cannot be interacted with
            VideoButton.Opacity = VideoButton.IsEnabled ? 1 : 0;

            var stream = new InMemoryRandomAccessStream();

            Debug.WriteLine("Taking photo...");
            await _mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream);

            try
            {
                var file = await _captureFolder.CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption.GenerateUniqueName);

                Debug.WriteLine("Photo taken! Saving to " + file.Path);

                var photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation());

                await ReencodeAndSavePhotoAsync(stream, file, photoOrientation);

                Debug.WriteLine("Photo saved!");
            }
            catch (Exception ex)
            {
                // File I/O errors are reported as exceptions
                Debug.WriteLine("Exception when taking a photo: " + ex.ToString());
            }

            // Done taking a photo, so re-enable the button
            VideoButton.IsEnabled = true;
            VideoButton.Opacity = 1;
        }

        /// <summary>
        /// Records an MP4 video to a StorageFile and adds rotation metadata to it
        /// </summary>
        /// <returns></returns>
        private async Task StartRecordingAsync()
        {
            try
            {
                // Create storage file for the capture
                var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);

                var encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);

                // Calculate rotation angle, taking mirroring into account if necessary
                var rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation());
                encodingProfile.Video.Properties.Add(RotationKey, PropertyValue.CreateInt32(rotationAngle));

                Debug.WriteLine("Starting recording to " + videoFile.Path);

                await _mediaCapture.StartRecordToStorageFileAsync(encodingProfile, videoFile);
                _isRecording = true;

                Debug.WriteLine("Started recording!");
            }
            catch (Exception ex)
            {
                // File I/O errors are reported as exceptions
                Debug.WriteLine("Exception when starting video recording: " + ex.ToString());
            }
        }

        /// <summary>
        /// Stops recording a video
        /// </summary>
        /// <returns></returns>
        private async Task StopRecordingAsync()
        {
            Debug.WriteLine("Stopping recording...");

            _isRecording = false;
            await _mediaCapture.StopRecordAsync();

            Debug.WriteLine("Stopped recording!");
        }

        /// <summary>
        /// Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
        /// </summary>
        /// <returns></returns>
        private async Task CleanupCameraAsync()
        {
            Debug.WriteLine("CleanupCameraAsync");

            if (_isInitialized)
            {
                // If a recording is in progress during cleanup, stop it to save the recording
                if (_isRecording)
                {
                    await StopRecordingAsync();
                }

                if (_faceDetectionEffect != null)
                {
                    await CleanUpFaceDetectionEffectAsync();
                }

                if (_previewProperties != null)
                {
                    // The call to stop the preview is included here for completeness, but can be
                    // safely removed if a call to MediaCapture.Dispose() is being made later,
                    // as the preview will be automatically stopped at that point
                    await StopPreviewAsync();
                }

                _isInitialized = false;
            }

            if (_mediaCapture != null)
            {
                _mediaCapture.RecordLimitationExceeded -= MediaCapture_RecordLimitationExceeded;
                _mediaCapture.Failed -= MediaCapture_Failed;
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }
        }

        #endregion MediaCapture methods


        #region Helper functions

        /// <summary>
        /// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
        /// </summary>
        /// <returns></returns>
        private async Task SetupUiAsync()
        {
            // Attempt to lock page to landscape orientation to prevent the CaptureElement from rotating, as this gives a better experience
            DisplayInformation.AutoRotationPreferences = DisplayOrientations.Landscape;

            // Hide the status bar
            if (ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                await Windows.UI.ViewManagement.StatusBar.GetForCurrentView().HideAsync();
            }

            // Populate orientation variables with the current state
            _displayOrientation = _displayInformation.CurrentOrientation;
            if (_orientationSensor != null)
            {
                _deviceOrientation = _orientationSensor.GetCurrentOrientation();
            }
            
            RegisterEventHandlers();

            var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
            // Fall back to the local app storage if the Pictures Library is not available
            _captureFolder = picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
        }

        /// <summary>
        /// Unregisters event handlers for hardware buttons and orientation sensors, allows the StatusBar (on Phone) to show, and removes the page orientation lock
        /// </summary>
        /// <returns></returns>
        private async Task CleanupUiAsync()
        {
            UnregisterEventHandlers();

            // Show the status bar
            if (ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                await Windows.UI.ViewManagement.StatusBar.GetForCurrentView().ShowAsync();
            }

            // Revert orientation preferences
            DisplayInformation.AutoRotationPreferences = DisplayOrientations.None;
        }

        /// <summary>
        /// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
        /// </summary>
        private void UpdateCaptureControls()
        {
            // The buttons should only be enabled if the preview started sucessfully
            PhotoButton.IsEnabled = _previewProperties != null;
            VideoButton.IsEnabled = _previewProperties != null;
            FaceDetectionButton.IsEnabled = _previewProperties != null;

            // Update the face detection icon depending on whether the effect exists or not
            FaceDetectionDisabledIcon.Visibility = (_faceDetectionEffect == null || !_faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;
            FaceDetectionEnabledIcon.Visibility = (_faceDetectionEffect != null && _faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;

            // Hide the face detection canvas and clear it
            FacesCanvas.Visibility = (_faceDetectionEffect != null && _faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;

            // Update recording button to show "Stop" icon instead of red "Record" icon when recording
            StartRecordingIcon.Visibility = _isRecording ? Visibility.Collapsed : Visibility.Visible;
            StopRecordingIcon.Visibility = _isRecording ? Visibility.Visible : Visibility.Collapsed;

            // If the camera doesn't support simultaneosly taking pictures and recording video, disable the photo button on record
            if (_isInitialized && !_mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported)
            {
                PhotoButton.IsEnabled = !_isRecording;

                // Make the button invisible if it's disabled, so it's obvious it cannot be interacted with
                PhotoButton.Opacity = PhotoButton.IsEnabled ? 1 : 0;
            }
        }

        /// <summary>
        /// Registers event handlers for hardware buttons and orientation sensors, and performs an initial update of the UI rotation
        /// </summary>
        private void RegisterEventHandlers()
        {
            if (ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                HardwareButtons.CameraPressed += HardwareButtons_CameraPressed;
            }

            // If there is an orientation sensor present on the device, register for notifications
            if (_orientationSensor != null)
            {
                _orientationSensor.OrientationChanged += OrientationSensor_OrientationChanged;

                // Update orientation of buttons with the current orientation
                UpdateButtonOrientation();
            }

            _displayInformation.OrientationChanged += DisplayInformation_OrientationChanged;
            _systemMediaControls.PropertyChanged += SystemMediaControls_PropertyChanged;
        }

        /// <summary>
        /// Unregisters event handlers for hardware buttons and orientation sensors
        /// </summary>
        private void UnregisterEventHandlers()
        {
            if (ApiInformation.IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
            {
                HardwareButtons.CameraPressed -= HardwareButtons_CameraPressed;
            }

            if (_orientationSensor != null)
            {
                _orientationSensor.OrientationChanged -= OrientationSensor_OrientationChanged;
            }

            _displayInformation.OrientationChanged -= DisplayInformation_OrientationChanged;
            _systemMediaControls.PropertyChanged -= SystemMediaControls_PropertyChanged;
        }

        /// <summary>
        /// Attempts to find and return a device mounted on the panel specified, and on failure to find one it will return the first device listed
        /// </summary>
        /// <param name="desiredPanel">The desired panel on which the returned device should be mounted, if available</param>
        /// <returns></returns>
        private static async Task<DeviceInformation> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
        {
            // Get available devices for capturing pictures
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);

            // Get the desired camera by panel
            DeviceInformation desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);

            // If there is no device mounted on the desired panel, return the first device found
            return desiredDevice ?? allVideoDevices.FirstOrDefault();
        }

        /// <summary>
        /// Applies the given orientation to a photo stream and saves it as a StorageFile
        /// </summary>
        /// <param name="stream">The photo stream</param>
        /// <param name="file">The StorageFile in which the photo stream will be saved</param>
        /// <param name="photoOrientation">The orientation metadata to apply to the photo</param>
        /// <returns></returns>
        private static async Task ReencodeAndSavePhotoAsync(IRandomAccessStream stream, StorageFile file, PhotoOrientation photoOrientation)
        {
            using (var inputStream = stream)
            {
                var decoder = await BitmapDecoder.CreateAsync(inputStream);

                using (var outputStream = await file.OpenAsync(FileAccessMode.ReadWrite))
                {
                    var encoder = await BitmapEncoder.CreateForTranscodingAsync(outputStream, decoder);

                    var properties = new BitmapPropertySet { { "System.Photo.Orientation", new BitmapTypedValue(photoOrientation, PropertyType.UInt16) } };

                    await encoder.BitmapProperties.SetPropertiesAsync(properties);
                    await encoder.FlushAsync();
                }
            }
        }

        #endregion Helper functions


        #region Rotation helpers

        /// <summary>
        /// Calculates the current camera orientation from the device orientation by taking into account whether the camera is external or facing the user
        /// </summary>
        /// <returns>The camera orientation in space, with an inverted rotation in the case the camera is mounted on the device and is facing the user</returns>
        private SimpleOrientation GetCameraOrientation()
        {
            if (_externalCamera)
            {
                // Cameras that are not attached to the device do not rotate along with it, so apply no rotation
                return SimpleOrientation.NotRotated;
            }

            var result = _deviceOrientation;

            // Account for the fact that, on portrait-first devices, the camera sensor is mounted at a 90 degree offset to the native orientation
            if (_displayInformation.NativeOrientation == DisplayOrientations.Portrait)
            {
                switch (result)
                {
                    case SimpleOrientation.Rotated90DegreesCounterclockwise:
                        result = SimpleOrientation.NotRotated;
                        break;
                    case SimpleOrientation.Rotated180DegreesCounterclockwise:
                        result = SimpleOrientation.Rotated90DegreesCounterclockwise;
                        break;
                    case SimpleOrientation.Rotated270DegreesCounterclockwise:
                        result = SimpleOrientation.Rotated180DegreesCounterclockwise;
                        break;
                    case SimpleOrientation.NotRotated:
                        result = SimpleOrientation.Rotated270DegreesCounterclockwise;
                        break;
                }
            }

            // If the preview is being mirrored for a front-facing camera, then the rotation should be inverted
            if (_mirroringPreview)
            {
                // This only affects the 90 and 270 degree cases, because rotating 0 and 180 degrees is the same clockwise and counter-clockwise
                switch (result)
                {
                    case SimpleOrientation.Rotated90DegreesCounterclockwise:
                        return SimpleOrientation.Rotated270DegreesCounterclockwise;
                    case SimpleOrientation.Rotated270DegreesCounterclockwise:
                        return SimpleOrientation.Rotated90DegreesCounterclockwise;
                }
            }

            return result;
        }

        /// <summary>
        /// Converts the given orientation of the device in space to the corresponding rotation in degrees
        /// </summary>
        /// <param name="orientation">The orientation of the device in space</param>
        /// <returns>An orientation in degrees</returns>
        private static int ConvertDeviceOrientationToDegrees(SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return 90;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    return 180;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return 270;
                case SimpleOrientation.NotRotated:
                default:
                    return 0;
            }
        }

        /// <summary>
        /// Converts the given orientation of the app on the screen to the corresponding rotation in degrees
        /// </summary>
        /// <param name="orientation">The orientation of the app on the screen</param>
        /// <returns>An orientation in degrees</returns>
        private static int ConvertDisplayOrientationToDegrees(DisplayOrientations orientation)
        {
            switch (orientation)
            {
                case DisplayOrientations.Portrait:
                    return 90;
                case DisplayOrientations.LandscapeFlipped:
                    return 180;
                case DisplayOrientations.PortraitFlipped:
                    return 270;
                case DisplayOrientations.Landscape:
                default:
                    return 0;
            }
        }

        /// <summary>
        /// Converts the given orientation of the device in space to the metadata that can be added to captured photos
        /// </summary>
        /// <param name="orientation">The orientation of the device in space</param>
        /// <returns></returns>
        private static PhotoOrientation ConvertOrientationToPhotoOrientation(SimpleOrientation orientation)
        {
            switch (orientation)
            {
                case SimpleOrientation.Rotated90DegreesCounterclockwise:
                    return PhotoOrientation.Rotate90;
                case SimpleOrientation.Rotated180DegreesCounterclockwise:
                    return PhotoOrientation.Rotate180;
                case SimpleOrientation.Rotated270DegreesCounterclockwise:
                    return PhotoOrientation.Rotate270;
                case SimpleOrientation.NotRotated:
                default:
                    return PhotoOrientation.Normal;
            }
        }

        /// <summary>
        /// Uses the current device orientation in space and page orientation on the screen to calculate the rotation
        /// transformation to apply to the controls
        /// </summary>
        /// <returns>An angle in degrees to rotate the controls so they remain upright to the user regardless of device and page
        /// orientation</returns>
        private void UpdateButtonOrientation()
        {
            int device = ConvertDeviceOrientationToDegrees(_deviceOrientation);
            int display = ConvertDisplayOrientationToDegrees(_displayOrientation);

            if (_displayInformation.NativeOrientation == DisplayOrientations.Portrait)
            {
                device -= 90;
            }

            // Combine both rotations and make sure that 0 <= result < 360
            var angle = (360 + display + device) % 360;

            // Rotate the buttons in the UI to match the rotation of the device
            var transform = new RotateTransform { Angle = angle };

            // The RenderTransform is safe to use (i.e. it won't cause layout issues) in this case, because these buttons have a 1:1 aspect ratio
            PhotoButton.RenderTransform = transform;
            VideoButton.RenderTransform = transform;
            FaceDetectionButton.RenderTransform = transform;
        }

        /// <summary>
        /// Uses the current display orientation to calculate the rotation transformation to apply to the face detection bounding box canvas
        /// and mirrors it if the preview is being mirrored
        /// </summary>
        private void SetFacesCanvasRotation()
        {
            // Calculate how much to rotate the canvas
            int rotationDegrees = ConvertDisplayOrientationToDegrees(_displayOrientation);

            // The rotation direction needs to be inverted if the preview is being mirrored, just like in SetPreviewRotationAsync
            if (_mirroringPreview)
            {
                rotationDegrees = (360 - rotationDegrees) % 360;
            }

            // Apply the rotation
            var transform = new RotateTransform { Angle = rotationDegrees };
            FacesCanvas.RenderTransform = transform;

            var previewArea = GetPreviewStreamRectInControl(_previewProperties as VideoEncodingProperties, PreviewControl);

            // For portrait mode orientations, swap the width and height of the canvas after the rotation, so the control continues to overlap the preview
            if (_displayOrientation == DisplayOrientations.Portrait || _displayOrientation == DisplayOrientations.PortraitFlipped)
            {
                FacesCanvas.Width = previewArea.Height;
                FacesCanvas.Height = previewArea.Width;

                // The position of the canvas also needs to be adjusted, as the size adjustment affects the centering of the control
                Canvas.SetLeft(FacesCanvas, previewArea.X - (previewArea.Height - previewArea.Width) / 2);
                Canvas.SetTop(FacesCanvas, previewArea.Y - (previewArea.Width - previewArea.Height) / 2);
            }
            else
            {
                FacesCanvas.Width = previewArea.Width;
                FacesCanvas.Height = previewArea.Height;

                Canvas.SetLeft(FacesCanvas, previewArea.X);
                Canvas.SetTop(FacesCanvas, previewArea.Y);
            }

            // Also mirror the canvas if the preview is being mirrored
            FacesCanvas.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
        }

        #endregion Rotation helpers


        #region Face detection helpers

        /// <summary>
        /// Iterates over all detected faces, creating and adding Rectangles to the FacesCanvas as face bounding boxes
        /// </summary>
        /// <param name="faces">The list of detected faces from the FaceDetected event of the effect</param>
        private void HighlightDetectedFaces(IReadOnlyList<DetectedFace> faces)
        {
            // Remove any existing rectangles from previous events
            FacesCanvas.Children.Clear();

            // For each detected face
            for (int i = 0; i < faces.Count; i++)
            {
                // Face coordinate units are preview resolution pixels, which can be a different scale from our display resolution, so a conversion may be necessary
                Rectangle faceBoundingBox = ConvertPreviewToUiRectangle(faces[i].FaceBox);

                // Set bounding box stroke properties
                faceBoundingBox.StrokeThickness = 2;

                // Highlight the first face in the set
                faceBoundingBox.Stroke = (i == 0 ? new SolidColorBrush(Colors.Blue) : new SolidColorBrush(Colors.DeepSkyBlue));

                // Add grid to canvas containing all face UI objects
                FacesCanvas.Children.Add(faceBoundingBox);
            }

            // Update the face detection bounding box canvas orientation
            SetFacesCanvasRotation();
        }

        /// <summary>
        /// Takes face information defined in preview coordinates and returns one in UI coordinates, taking
        /// into account the position and size of the preview control.
        /// </summary>
        /// <param name="faceBoxInPreviewCoordinates">Face coordinates as retried from the FaceBox property of a DetectedFace, in preview coordinates.</param>
        /// <returns>Rectangle in UI (CaptureElement) coordinates, to be used in a Canvas control.</returns>
        private Rectangle ConvertPreviewToUiRectangle(BitmapBounds faceBoxInPreviewCoordinates)
        {
            var result = new Rectangle();
            var previewStream = _previewProperties as VideoEncodingProperties;

            // If there is no available information about the preview, return an empty rectangle, as re-scaling to the screen coordinates will be impossible
            if (previewStream == null) return result;

            // Similarly, if any of the dimensions is zero (which would only happen in an error case) return an empty rectangle
            if (previewStream.Width == 0 || previewStream.Height == 0) return result;

            double streamWidth = previewStream.Width;
            double streamHeight = previewStream.Height;

            // For portrait orientations, the width and height need to be swapped
            if (_displayOrientation == DisplayOrientations.Portrait || _displayOrientation == DisplayOrientations.PortraitFlipped)
            {
                streamHeight = previewStream.Width;
                streamWidth = previewStream.Height;
            }

            // Get the rectangle that is occupied by the actual video feed
            var previewInUI = GetPreviewStreamRectInControl(previewStream, PreviewControl);

            // Scale the width and height from preview stream coordinates to window coordinates
            result.Width = (faceBoxInPreviewCoordinates.Width / streamWidth) * previewInUI.Width;
            result.Height = (faceBoxInPreviewCoordinates.Height / streamHeight) * previewInUI.Height;

            // Scale the X and Y coordinates from preview stream coordinates to window coordinates
            var x = (faceBoxInPreviewCoordinates.X / streamWidth) * previewInUI.Width;
            var y = (faceBoxInPreviewCoordinates.Y / streamHeight) * previewInUI.Height;
            Canvas.SetLeft(result, x);
            Canvas.SetTop(result, y);

            return result;
        }

        /// <summary>
        /// Calculates the size and location of the rectangle that contains the preview stream within the preview control, when the scaling mode is Uniform
        /// </summary>
        /// <param name="previewResolution">The resolution at which the preview is running</param>
        /// <param name="previewControl">The control that is displaying the preview using Uniform as the scaling mode</param>
        /// <returns></returns>
        public Rect GetPreviewStreamRectInControl(VideoEncodingProperties previewResolution, CaptureElement previewControl)
        {
            var result = new Rect();

            // In case this function is called before everything is initialized correctly, return an empty result
            if (previewControl == null || previewControl.ActualHeight < 1 || previewControl.ActualWidth < 1 ||
                previewResolution == null || previewResolution.Height == 0 || previewResolution.Width == 0)
            {
                return result;
            }

            var streamWidth = previewResolution.Width;
            var streamHeight = previewResolution.Height;

            // For portrait orientations, the width and height need to be swapped
            if (_displayOrientation == DisplayOrientations.Portrait || _displayOrientation == DisplayOrientations.PortraitFlipped)
            {
                streamWidth = previewResolution.Height;
                streamHeight = previewResolution.Width;
            }

            // Start by assuming the preview display area in the control spans the entire width and height both (this is corrected in the next if for the necessary dimension)
            result.Width = previewControl.ActualWidth;
            result.Height = previewControl.ActualHeight;

            // If UI is "wider" than preview, letterboxing will be on the sides
            if ((previewControl.ActualWidth / previewControl.ActualHeight > streamWidth / (double)streamHeight))
            {
                var scale = previewControl.ActualHeight / streamHeight;
                var scaledWidth = streamWidth * scale;

                result.X = (previewControl.ActualWidth - scaledWidth) / 2.0;
                result.Width = scaledWidth;
            }
            else // Preview stream is "wider" than UI, so letterboxing will be on the top+bottom
            {
                var scale = previewControl.ActualWidth / streamWidth;
                var scaledHeight = streamHeight * scale;

                result.Y = (previewControl.ActualHeight - scaledHeight) / 2.0;
                result.Height = scaledHeight;
            }

            return result;
        }

        #endregion

    }
}