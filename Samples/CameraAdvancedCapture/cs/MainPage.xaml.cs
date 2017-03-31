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
using Windows.Media.Capture;
using Windows.Media.Core;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.Phone.UI.Input;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace CameraAdvancedCapture
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
        private bool _isInitialized;
        private bool _isPreviewing;

        // Holds the index of the current Advanced Capture mode (-1 for no Advanced Capture active)
        private int _advancedCaptureMode = -1;

        // Information about the camera device
        private bool _mirroringPreview;
        private bool _externalCamera;

        // The value at which the certainty from the HDR analyzer maxes out in the graphical representation
        private const double CERTAINTY_CAP = 0.7;

        // Advanced Capture and Scene Analysis instances
        private AdvancedPhotoCapture _advancedCapture;
        private SceneAnalysisEffect _sceneAnalysisEffect;


        /// <summary>
        /// Helper class to contain the information that describes an Advanced Capture
        /// </summary>
        public class AdvancedCaptureContext
        {
            public string CaptureFileName;
            public PhotoOrientation CaptureOrientation;
        }


        #region Constructor, lifecycle and navigation

        public MainPage()
        {
            this.InitializeComponent();

            // Do not cache the state of the UI when suspending/navigating
            NavigationCacheMode = NavigationCacheMode.Disabled;

            // Useful to know when to initialize/clean up the camera
            Application.Current.Suspending += Application_Suspending;
            Application.Current.Resuming += Application_Resuming;

            // Set the maximum on the progress bar
            HdrImpactBar.Maximum = CERTAINTY_CAP;
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
            // Handling of this event is included for completeness, as it will only trigger when navigating between pages and this sample only includes one page

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

                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateControlOrientation());
            }
        }

        /// <summary>
        /// This event will be raised when the page is rotated, when the DisplayInformation.AutoRotationPreferences value set in the SetupUiAsync() method
        /// cannot be not honored.
        /// </summary>
        /// <param name="sender">The event source.</param>
        /// <param name="args">The event data.</param>
        private async void DisplayInformation_OrientationChanged(DisplayInformation sender, object args)
        {
            _displayOrientation = sender.CurrentOrientation;

            if (_isPreviewing)
            {
                await SetPreviewRotationAsync();
            }

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateControlOrientation());
        }

        /// <summary>
        /// This event will be raised every time the effect processes a preview frame, to deliver information about the current scene in the camera preview.
        /// </summary>
        /// <param name="sender">The effect raising the event.</param>
        /// <param name="args">The event data.</param>
        private async void SceneAnalysisEffect_SceneAnalyzed(SceneAnalysisEffect sender, SceneAnalyzedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Update the graphical representation of how much detail could be recovered through an HDR capture, ranging
                // from 0 ("everything is within the dynamic range of the camera") to CERTAINTY_CAP ("at this point the app
                // strongly recommends an HDR capture"), where CERTAINTY_CAP can be any number between 0 and 1 that the app chooses.
                HdrImpactBar.Value = Math.Min(CERTAINTY_CAP, args.ResultFrame.HighDynamicRange.Certainty);
            });
        }

        /// <summary>
        /// This event will be raised only on devices that support returning a reference photo, which is a normal exposure of the scene
        /// without processing, also referred to as "EV0".
        /// </summary>
        /// <param name="sender">The object raising this event.</param>
        /// <param name="args">The event data.</param>
        private async void AdvancedCapture_OptionalReferencePhotoCaptured(AdvancedPhotoCapture sender, OptionalReferencePhotoCapturedEventArgs args)
        {
            // Retrieve the context (i.e. the information about the capture this event belongs to)
            var context = args.Context as AdvancedCaptureContext;
            
            // Add "_Reference" at the end of the filename to create when saving the reference photo
            var referenceName = context.CaptureFileName.Replace(".jpg", "_Reference.jpg");

            // Hold a reference to the frame before the async file creation operation so it's not cleaned up before ReencodeAndSavePhotoAsync
            using (var frame = args.Frame)
            {
                var file = await _captureFolder.CreateFileAsync(referenceName, CreationCollisionOption.GenerateUniqueName);
                Debug.WriteLine("AdvancedCapture_OptionalReferencePhotoCaptured for " + context.CaptureFileName + ". Saving to " + file.Path);

                await ReencodeAndSavePhotoAsync(frame, file, context.CaptureOrientation);
            }
        }

        /// <summary>
        /// This event will be raised when all the necessary captures are completed, and at this point the camera is technically ready
        /// to capture again while the processing takes place.
        /// </summary>
        /// <param name="sender">The object raising this event.</param>
        /// <param name="args">The event data.</param>
        private void AdvancedCapture_AllPhotosCaptured(AdvancedPhotoCapture sender, object args)
        {
            Debug.WriteLine("AdvancedCapture_AllPhotosCaptured");
        }

        private async void PhotoButton_Click(object sender, RoutedEventArgs e)
        {
            await TakeAdvancedCapturePhotoAsync();
        }

        private async void HardwareButtons_CameraPressed(object sender, CameraEventArgs e)
        {
            await TakeAdvancedCapturePhotoAsync();
        }

        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);

            await CleanupCameraAsync();

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateUi());
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
                // Attempt to get the back camera if one is available, but use any camera device if not
                var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Back);

                if (cameraDevice == null)
                {
                    Debug.WriteLine("No camera device found!");
                    return;
                }

                // Create MediaCapture and its settings
                _mediaCapture = new MediaCapture();

                // Register for a notification when something goes wrong
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

                // If initialization succeeded, start the preview, kick off scene analysis, and enable Advanced Capture
                if (_isInitialized)
                {
                    // Figure out where the camera is located to account for mirroring and later adjust rotation accordingly
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

                    // On successful preview, add a Scene Analysis effect to the pipeline and enter Advanced Capture mode
                    if (_isPreviewing)
                    {
                        await CreateSceneAnalysisEffectAsync();

                        await EnableAdvancedCaptureAsync();
                    }

                }

                // Update the controls to reflect the current state of the app
                UpdateUi();
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
            _isPreviewing = true;

            // Initialize the preview to the current orientation
            if (_isPreviewing)
            {
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
            props.Properties[RotationKey] = rotationDegrees;
            await _mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, props, null);
        }

        /// <summary>
        /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
        /// </summary>
        /// <returns></returns>
        private async Task StopPreviewAsync()
        {
            // Stop the preview
            _isPreviewing = false;
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
        /// Adds scene analysis to the video preview stream, registers for its event, enables it, and gets the effect instance
        /// </summary>
        /// <returns></returns>
        private async Task CreateSceneAnalysisEffectAsync()
        {
            // Create the definition, which will contain some initialization settings
            var definition = new SceneAnalysisEffectDefinition();

            // Add the effect to the video preview stream
            _sceneAnalysisEffect = (SceneAnalysisEffect)await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview);

            Debug.WriteLine("SA effect added to pipeline");

            // Subscribe to notifications about scene information
            _sceneAnalysisEffect.SceneAnalyzed += SceneAnalysisEffect_SceneAnalyzed;

            // Enable HDR analysis
            _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = true;
        }

        /// <summary>
        /// Disables and removes the scene analysis effect, and unregisters the event handler for the SceneAnalyzed event of the effect
        /// </summary>
        /// <returns></returns>
        private async Task CleanSceneAnalysisEffectAsync()
        {
            // Disable detection and unsubscribe from the events
            _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = false;
            _sceneAnalysisEffect.SceneAnalyzed -= SceneAnalysisEffect_SceneAnalyzed;

            // Remove the effect from the pipeline (see ClearEffectsAsync method to remove all effects from a given stream)
            await _mediaCapture.RemoveEffectAsync(_sceneAnalysisEffect);

            Debug.WriteLine("SA effect removed from pipeline");

            // Clear the member variable that held the effect instance
            _sceneAnalysisEffect = null;
        }

        /// <summary>
        /// Creates an instance of the AdvancedPhotoCapture and registers for its events
        /// </summary>
        /// <returns></returns>
        private async Task EnableAdvancedCaptureAsync()
        {
            // No work to be done if there already is an AdvancedCapture instance
            if (_advancedCapture != null) return;

            // Configure one of the modes in the control
            CycleAdvancedCaptureMode();

            // Prepare for an Advanced Capture
            _advancedCapture = await _mediaCapture.PrepareAdvancedPhotoCaptureAsync(ImageEncodingProperties.CreateJpeg());

            Debug.WriteLine("Enabled Advanced Capture");

            // Register for events published by the AdvancedCapture
            _advancedCapture.AllPhotosCaptured += AdvancedCapture_AllPhotosCaptured;
            _advancedCapture.OptionalReferencePhotoCaptured += AdvancedCapture_OptionalReferencePhotoCaptured;
        }

        /// <summary>
        /// Configures the AdvancedPhotoControl to the next supported mode
        /// </summary>
        /// <remarks>
        /// Note that this method can be safely called regardless of whether the AdvancedPhotoCapture is in the "prepared
        /// state" or not. Internal changes to the mode will be applied  when calling Prepare, or when calling Capture if
        /// the mode has been changed since the call to Prepare. This allows for fast changing of desired capture modes,
        /// that can more quickly adapt to rapidly changing shooting conditions.
        /// </remarks>
        private void CycleAdvancedCaptureMode()
        {
            // Calculate the index for the next supported mode
            _advancedCaptureMode = (_advancedCaptureMode + 1) % _mediaCapture.VideoDeviceController.AdvancedPhotoControl.SupportedModes.Count;

            // Configure the settings object to the mode at the calculated index
            var settings = new AdvancedPhotoCaptureSettings
            {
                Mode = _mediaCapture.VideoDeviceController.AdvancedPhotoControl.SupportedModes[_advancedCaptureMode]
            };

            // Configure the mode on the control
            _mediaCapture.VideoDeviceController.AdvancedPhotoControl.Configure(settings);

            // Update the button text to reflect the current mode
            ModeTextBlock.Text = _mediaCapture.VideoDeviceController.AdvancedPhotoControl.Mode.ToString();
        }

        /// <summary>
        /// Cleans up the instance of the AdvancedCapture
        /// </summary>
        /// <returns></returns>
        private async Task DisableAdvancedCaptureAsync()
        {
            // No work to be done if there is no AdvancedCapture
            if (_advancedCapture == null) return;

            await _advancedCapture.FinishAsync();
            _advancedCapture = null;

            // Reset the Advanced Capture Mode index
            _advancedCaptureMode = -1;

            Debug.WriteLine("Disabled Advanced Capture");
        }

        /// <summary>
        /// Takes an Advanced Capture photo to a StorageFile and adds rotation metadata to it
        /// </summary>
        /// <returns></returns>
        private async Task TakeAdvancedCapturePhotoAsync()
        {
            // Lock the UI as a simple way to prevent reentrancy
            PhotoButton.IsEnabled = false;
            CycleModeButton.IsEnabled = false;

            try
            {
                Debug.WriteLine("Taking Advanced Capture photo...");
                // Read the current orientation of the camera and the capture time
                var photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation());
                var fileName = String.Format("AdvancedCapturePhoto_{0}.jpg", DateTime.Now.ToString("HHmmss"));

                // Create a context object, to identify the capture later on
                var context = new AdvancedCaptureContext {CaptureFileName = fileName, CaptureOrientation = photoOrientation};

                // Start capture, and pass the context object to get it back in the OptionalReferencePhotoCaptured event
                var capture = await _advancedCapture.CaptureAsync(context);

                using (var frame = capture.Frame)
                {
                    var file = await _captureFolder.CreateFileAsync(fileName, CreationCollisionOption.GenerateUniqueName);
                    Debug.WriteLine("Advanced Capture photo taken! Saving to " + file.Path);

                    await ReencodeAndSavePhotoAsync(frame, file, photoOrientation);
                }
            }
            catch (Exception ex)
            {
                // File I/O errors are reported as exceptions
                Debug.WriteLine("Exception when taking an Advanced Capture photo: " + ex.ToString());
            }
            finally
            {
                // Re-enable the buttons to reflect the current state of the app
                UpdateUi();
            }
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
                if (_isPreviewing)
                {
                    // The call to stop the preview is included here for completeness, but can be
                    // safely removed if a call to MediaCapture.Dispose() is being made later,
                    // as the preview will be automatically stopped at that point
                    await StopPreviewAsync();
                }

                if (_advancedCapture != null)
                {
                    await DisableAdvancedCaptureAsync();
                }

                if (_sceneAnalysisEffect != null)
                {
                    await CleanSceneAnalysisEffectAsync();
                }

                _isInitialized = false;
            }

            if (_mediaCapture != null)
            {
                _mediaCapture.Failed -= MediaCapture_Failed;
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }
        }

        #endregion MediaCapture methods


        #region Helper methods

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
        /// This method will enable/disable the buttons depending on the current state of the app
        /// </summary>
        private void UpdateUi()
        {
            // The buttons should only be enabled if the preview started sucessfully
            PhotoButton.IsEnabled = _isPreviewing;
            CycleModeButton.IsEnabled = _isPreviewing;
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
                UpdateControlOrientation();
            }

            // Have the Advanced Capture button call the method that cycles through the modes
            CycleModeButton.Click += (sender, args) => CycleAdvancedCaptureMode();

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
        private void UpdateControlOrientation()
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
            CycleModeButton.RenderTransform = transform;

            // Adjust which way the progress bar fills so it's always from the user's left->right or bottom->top
            HdrImpactBar.FlowDirection = (angle == 180 || angle == 270) ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
        }

        #endregion Rotation helpers
    }
}