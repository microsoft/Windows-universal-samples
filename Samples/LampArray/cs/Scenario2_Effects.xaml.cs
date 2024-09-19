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
using Microsoft.Graphics.Canvas;
using Windows.Devices.Enumeration;
using Windows.Devices.Lights;
using Windows.Devices.Lights.Effects;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    // State for the custom wave effect.
    internal class CustomEffectState
    {
        public LampArray lampArray;
        public int frameNumber = 0;
        public float[] lampNormalizedX;
        public Color[] lampColors;

        public Color color = Colors.HotPink;
        public bool isColorOn = true;
    }

    // State for the generated bitmap effect.
    internal class GeneratedBitmapState
    {
        // Helper variables for the generated bitmap effect.
        public int bitmapWidth;
        public int bitmapHeight;
        public float squareLength;
        public float squareY;
        public int frameNumber = 0;

        public CanvasDevice canvasDevice = null;
        public CanvasRenderTarget offscreenCanvas = null;
        public CanvasDrawingSession offscreenDrawingSession = null;

        public SoftwareBitmap squareBitmap = null;
        public Windows.Storage.Streams.Buffer squareBuffer = null;
    }

    public sealed partial class Scenario2_Effects : Page
    {
        private static readonly TimeSpan EffectUpdateInterval = TimeSpan.FromMilliseconds(33);
        private static readonly TimeSpan ColorRampDuration = TimeSpan.FromMilliseconds(500);
        private static readonly int CustomEffectFrameCount = 30;
        private static readonly int GeneratedBitmapEffectFrameCount = 60;

        private DeviceWatcher m_deviceWatcher;

        // Currently attached LampArrays
        private readonly List<LampArrayInfo> m_attachedLampArrays = new List<LampArrayInfo>();

        private readonly IReadOnlyList<LampArrayEffectPlaylist> m_emptyList = new List<LampArrayEffectPlaylist>();
        // Playlists that can be paused, stopped, and started.
        private IReadOnlyList<LampArrayEffectPlaylist> m_readyPlaylists;

        private Random m_random = new Random();

        private MainPage rootPage = MainPage.Current;

        public Scenario2_Effects()
        {
            InitializeComponent();
            m_readyPlaylists = m_emptyList;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateLampArrayList();

            // Start watching for newly attached LampArrays.
            m_deviceWatcher = DeviceInformation.CreateWatcher(LampArray.GetDeviceSelector());

            m_deviceWatcher.Added += Watcher_Added;
            m_deviceWatcher.Removed += Watcher_Removed;

            m_deviceWatcher.Start();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            m_deviceWatcher.Stop();

            // When changing scenarios, stop our effects to prevent them from interfering with the new scenario.
            CleanupPreviousEffect();
        }

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            var info = new LampArrayInfo(args.Id, args.Name, await LampArray.FromIdAsync(args.Id));

            // DeviceWatcher events are invoked on a background thread.
            // We need to switch to the foreground thread to update our app UI
            // and access member variables without race conditions.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (info.lampArray == null)
                {
                    // A LampArray was found, but Windows couldn't initialize it.
                    // This suggests a device error.
                    rootPage.NotifyUser($"Problem with LampArray {info.displayName}.", NotifyType.ErrorMessage);
                    return;
                }

                // Initial condition for the new LampArray is all lights off.
                info.lampArray.SetColor(Colors.Black);

                // Set up the AvailabilityChanged event callback for being notified whether this process can control
                // RGB lighting for the newly attached LampArray.
                info.lampArray.AvailabilityChanged += LampArray_AvailabilityChanged;

                m_attachedLampArrays.Add(info);

                UpdateLampArrayList();
            });
        }

        private async void Watcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            // DeviceWatcher events are invoked on a background thread.
            // We need to switch to the foreground thread to update our app UI
            // and access member variables without race conditions.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                m_attachedLampArrays.RemoveAll(info => info.id == args.Id);
                UpdateLampArrayList();
            });
        }

        // The AvailabilityChanged event will fire when this calling process gains or loses control of RGB lighting
        // for the specified LampArray.
        private async void LampArray_AvailabilityChanged(LampArray sender, object args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                UpdateLampArrayList();
            });
        }

        private void UpdateLampArrayList()
        {
            string message = $"Attached LampArrays: {m_attachedLampArrays.Count}\n";
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                message += $"\t{info.displayName} ({info.lampArray.LampArrayKind.ToString()}, {info.lampArray.LampCount} lamps, " +
                           $"{(info.lampArray.IsAvailable ? "Available" : "Unavailable")})\n";
            }
            LampArraysSummary.Text = message;
        }

        // This method will be called when a new effect is selected. It will stop any running
        // LampArrayEffectPlaylists in progress and reset LampArray state in preparation for the new effect.
        private void CleanupPreviousEffect()
        {
            // Clear the bitmap image in the app UI, as it is no longer current.
            ImageBitmap.Source = null;

            // Passing all the playlists to a single call to StopAll will stop them
            // simultaneously across all attached LampArrays.
            LampArrayEffectPlaylist.StopAll(m_readyPlaylists);
            m_readyPlaylists = m_emptyList;
        }

        #region Static bitmap effect
        private async void StaticBitmapButton_Click(object sender, RoutedEventArgs e)
        {
            CleanupPreviousEffect();

            // This effect will display a specified bitmap image on all connected LampArrays.
            // The image will be stretched and scaled to fit the Lamp dimensions of each LampArray.

            // First, open the desired bitmap file and convert it to a SoftwareBitmap
            Uri bitmapUri = new Uri("ms-appx:///Assets/grapes.jpg");
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(bitmapUri);
            SoftwareBitmap bitmap;
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                bitmap = await decoder.GetSoftwareBitmapAsync();
            }

            // Show our bitmap image in the app UI
            ImageBitmap.Source = new BitmapImage(bitmapUri);

            // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
            var playlists = new List<LampArrayEffectPlaylist>();

            // Create the effect playlist for each LampArray and add it to the collection.
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                var bitmapEffect = new LampArrayBitmapEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    // We want to run this effect until is stopped. For the purposes of this sample,
                    // set the effect Duration to run as long as possible.
                    Duration = TimeSpan.MaxValue,

                    // The UpdateInterval specifies how frequently we want to receive our callback function.
                    // Since this effect will display a static image, we only need to update once per effect iteration.
                    // Set this interval to be the same as the effect duration.
                    UpdateInterval = TimeSpan.MaxValue
                };

                // Set the event handler for our effect. Use a helper function to
                // to specify a bitmap to display on the device.
                bitmapEffect.BitmapRequested += (_, args) => StaticBitmapEffect_BitmapRequested(bitmap, args);

                // Create a playlist consisting of our static bitmap effect.
                var playlist = new LampArrayEffectPlaylist();
                playlist.Append(bitmapEffect);

                // Add it to the list of playlists to start simultaneously.
                playlists.Add(playlist);
            }

            // Start the newly created effect playlists simultaneously.
            LampArrayEffectPlaylist.StartAll(playlists);

            // Remember the playlists so we can stop them later.
            m_readyPlaylists = playlists;
        }

        // Helper function for BitmapRequested event handler that
        // specifies the bitmap to display on the device.
        // The frequency of the event is customizable.
        // As configured above when setting up the static bitmap effect, this event will
        // be raised only once per effect iteration, since we set the update interval
        // equal to the effect duration.
        private static void StaticBitmapEffect_BitmapRequested(SoftwareBitmap bitmap, LampArrayBitmapRequestedEventArgs args)
        {
            // In a BitmapRequested handler, we can change the bitmap here to produce animations.
            // For the static bitmap effect, show the same image each time.
            args.UpdateBitmap(bitmap);
        }
        #endregion

        #region Fade effect
        private void FadeButton_Click(object sender, RoutedEventArgs e)
        {
            CleanupPreviousEffect();

            // This effect will set each Lamp to a random color and blink all Lamps simultaneously.
            // LampArrayBlinkEffect only supports a single color at a time. To create the desired effect with
            // different colors for each Lamp, we will create a separate LampArrayBlinkEffect for each Lamp,
            // specifying that we only want the effect to apply to a single Lamp index.
            // We will then add each Lamp effect to the playlist for its corresponding LampArray and configure the
            // playlist to run its effects simultaneously instead of sequentially.

            // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
            var playlists = new List<LampArrayEffectPlaylist>();

            // Create the effect playlist for each LampArray and add it to the collection.
            foreach (LampArrayInfo context in m_attachedLampArrays)
            {
                var playlist = new LampArrayEffectPlaylist();

                // Configure the playlist to run the effects simultaneously.
                // This will result in all Lamps for this LampArray blinking in unison,
                // even though their colors will be different.
                playlist.EffectStartMode = LampArrayEffectStartMode.Simultaneous;

                for (int i = 0; i < context.lampArray.LampCount; i++)
                {
                    // Generate a color for each Lamp by choosing a random Red/Green/Blue combination.
                    var color = new Color
                    {
                        A = byte.MaxValue, // Maximum alpha makes it opaque
                        R = (byte)(m_random.Next(byte.MaxValue + 1)),
                        G = (byte)(m_random.Next(byte.MaxValue + 1)),
                        B = (byte)(m_random.Next(byte.MaxValue + 1)),
                    };

                    // Create the effect and apply it to a single Lamp.
                    playlist.Append(new LampArrayBlinkEffect(context.lampArray, new[] { i })
                    {
                        Color = color,

                        // We can adjust the timing of the LampArrayBlinkEffect using these properties.
                        // Specify how long it should take for the full color to fade in from off/Black to peak intensity.
                        AttackDuration = TimeSpan.FromMilliseconds(300),

                        // Specify how long to display the color at peak intensity.
                        SustainDuration = TimeSpan.FromMilliseconds(500),

                        // Specify how long it should take for the full color to fade out from peak intensity to off/Black.
                        DecayDuration = TimeSpan.FromMilliseconds(800),

                        // Specify how long the Lamps should stay dark between blink repetitions.
                        RepetitionDelay = TimeSpan.FromMilliseconds(100),

                        // Repeat the blink sequence indefinitely until the effect playlist is stopped.
                        RepetitionMode = LampArrayRepetitionMode.Forever
                    });
                }

                // Add it to the list of playlists to start simultaneously.
                playlists.Add(playlist);
            }

            // Start the newly created effect playlists simultaneously.
            LampArrayEffectPlaylist.StartAll(playlists);

            // Remember the playlists so we can stop them later.
            m_readyPlaylists = playlists;
        }
        #endregion

        #region Custom effect
        // Windows.Devices.Lights.Effects contains several built-in effect types. The LampArrayCustomEffect is provided for
        // greater customization of lighting beyond these simple effects. It enables the SetColor controls of LampArray to
        // be integrated and scheduled with other effects and effect playlists.
        // This sample will demonstrate using LampArrayCustomEffect to display a simple lighting animation.
        private void CustomEffectButton_Click(object sender, RoutedEventArgs e)
        {
            CleanupPreviousEffect();

            // This effect will use LampArrayCustomEffect to create a wave-like animation.
            // Lamps will turn on from left to right across the LampArray, then turn off in a similar manner.

            // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
            var playlists = new List<LampArrayEffectPlaylist>();

            // Create the effect playlist for each LampArray and add it to the collection.
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                LampArray lampArray = info.lampArray;
                int[] indices = GetLampArrayIndices(lampArray);
                float boundingBoxWidth = lampArray.BoundingBox.X;
                // Avoid division by zero when calculating normalized positions.
                if (boundingBoxWidth == 0.0f)
                {
                    boundingBoxWidth = 1.0f;
                }
                CustomEffectState state = new CustomEffectState()
                {
                    lampArray = lampArray,
                    lampNormalizedX = new float[lampArray.LampCount],
                    lampColors = new Color[lampArray.LampCount]
                };

                // Calculate each lamp's normalized X position relative to the width of the bounding box.
                // (zero will be the leftmost edge of the bounding box, and 1 will be the rightmost edge).
                for (int i = 0; i < state.lampNormalizedX.Length; i++)
                {
                    LampInfo lampInfo = lampArray.GetLampInfo(i);
                    state.lampNormalizedX[i] = lampInfo.Position.X / boundingBoxWidth;
                }

                var customEffect = new LampArrayCustomEffect(lampArray, indices)
                {
                    // We want to run this effect until is stopped. For the purposes of this sample,
                    // set the effect Duration to run as long as possible.
                    Duration = TimeSpan.MaxValue,

                    // The UpdateInterval specifies how frequently we want to receive our callback function.
                    // For our animation, set this interval to 33 milliseconds, which equates to approximately 30 frames per second.
                    UpdateInterval = EffectUpdateInterval
                };

                // This list will track the colors to be applied to each Lamp in the effect callback function.
                // The list is ordered by Lamp index. The ordering does not reflect positions of any Lamp.
                state.lampColors = Enumerable.Repeat(Colors.Black, state.lampArray.LampCount).ToArray();

                // This event handler will be called on every frame to update the animation.
                customEffect.UpdateRequested += (_, args) => CustomEffect_UpdateRequested(state, args);

                // Create a playlist consisting of our wave effect.
                var playlist = new LampArrayEffectPlaylist();
                playlist.Append(customEffect);

                // Add it to the list of playlists to start simultaneously.
                playlists.Add(playlist);
            }

            // Start the newly created effect playlists.
            LampArrayEffectPlaylist.StartAll(playlists);

            // Remember the playlists so we can stop them later.
            m_readyPlaylists = playlists;
        }

        // Helper for the LampArrayCustomEffect.UpdateRequested event, called periodically at the requested UpdateInterval.
        // This enables more complex lighting patterns, such as animations. It also supports integration and scheduling with
        // other effects and effect playlists.
        // As configured above, this callback will be invoked every 33 milliseconds, or about 30 frames per second.
        private static void CustomEffect_UpdateRequested(CustomEffectState state, LampArrayUpdateRequestedEventArgs args)
        {
            // Update the Lamp colors for the current frame. Start by calculating how far into the current iteration we are.
            // We will use this to mark how far across the LampArray bounding box the effect has reached.
            double effectProgress = (state.frameNumber + 1) / (double)CustomEffectFrameCount;

            // If Lamps were being turned off in the previous iteration, turn them on now, and vice versa.
            Color color = state.isColorOn ? state.color : Colors.Black;

            for (int i = 0; i < state.lampNormalizedX.Length; i++)
            {
                // All Lamps that are to the left of our marker (normalized to the LampArray bounding box width)
                // should have their states updated.
                if (state.lampNormalizedX[i] <= effectProgress)
                {
                    state.lampColors[i] = color;
                }
            }

            // Apply the Lamp states to the LampArray.
            args.SetColorsForIndices(state.lampColors, GetLampArrayIndices(state.lampArray));

            // Increment our frame counter, with wraparound if we've reached the frame limit.
            // Also update whether the lamps should be turned on or off for the next iteration.
            if (++state.frameNumber == CustomEffectFrameCount)
            {
                state.frameNumber = 0;
                state.isColorOn = !state.isColorOn;
            }
        }
        #endregion

        #region Generated bitmap effect
        // This effect uses Win2D to create a simple animation. On each frame, we
        // generate a bitmap image and apply it to the entire LampArray.
        // The effect illustrates dynamically creating a bitmap image and using it in a lighting effect.
        // LampArrayBitmapEffect could be used for in-game lighting and/or integration with 2D graphics libraries.
        private void GeneratedBitmapButton_Click(object sender, RoutedEventArgs e)
        {
            CleanupPreviousEffect();

            // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
            var playlists = new List<LampArrayEffectPlaylist>();

            // Create the effect playlist for each LampArray and add it to the collection.
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                GeneratedBitmapState state = new GeneratedBitmapState();

                var generatedBitmapEffect = new LampArrayBitmapEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    // We want to run this effect until is stopped. For the purposes of this sample,
                    // set the effect Duration to run as long as possible.
                    Duration = TimeSpan.MaxValue,

                    // The UpdateInterval specifies how frequently we want to receive our callback function.
                    // For our animation, set this interval to 33 milliseconds, which equates to approximately 30 frames per second.
                    UpdateInterval = EffectUpdateInterval
                };

                state.bitmapWidth = (int)generatedBitmapEffect.SuggestedBitmapSize.Width;
                state.bitmapHeight = (int)generatedBitmapEffect.SuggestedBitmapSize.Height;

                // The size of the square is half the height or half the width, whichever is smaller.
                state.squareLength = Math.Min(state.bitmapHeight, state.bitmapWidth) / 2.0f;

                // Vertically center the square.
                state.squareY = (state.bitmapHeight - state.squareLength) / 2.0f;

                // Setup our Win2D helper objects.
                state.canvasDevice = CanvasDevice.GetSharedDevice();

                state.offscreenCanvas = new CanvasRenderTarget(state.canvasDevice, state.bitmapWidth, state.bitmapHeight, 96);

                state.offscreenDrawingSession = state.offscreenCanvas.CreateDrawingSession();

                // The generated bitmap encompasses the entire LampArray
                state.squareBitmap = new SoftwareBitmap(BitmapPixelFormat.Bgra8, state.bitmapWidth, state.bitmapHeight, BitmapAlphaMode.Premultiplied);

                state.squareBuffer = new Windows.Storage.Streams.Buffer((uint)(state.bitmapWidth * state.bitmapHeight * 4));

                // This event handler will be called on every frame to update the bitmap to display on the device.
                generatedBitmapEffect.BitmapRequested += (_, args) => GeneratedBitmapEffect_UpdateRequested(state, args);

                // Create a playlist consisting of our bitmap effect.
                var playlist = new LampArrayEffectPlaylist();
                playlist.Append(generatedBitmapEffect);

                // Add it to the list of playlists to start simultaneously.
                playlists.Add(playlist);
            }

            // Start the newly created effect playlists.
            LampArrayEffectPlaylist.StartAll(playlists);

            // Remember the playlists so we can stop them later.
            m_readyPlaylists = playlists;
        }

        // Callback function that allows us to specify a bitmap to display on the device.
        // The frequency of the effect callback is customizable.
        // As configured above when setting up the generated bitmap effect, this will be invoked
        // every 33 milliseconds, or approximately 30 frames per second.
        // To demonstrate using a LampArrayBitmapEffect to perform lighting animations,
        // this callback will create an animation of a red rectangle moving left to right
        // across a blue background.
        private static void GeneratedBitmapEffect_UpdateRequested(GeneratedBitmapState state, LampArrayBitmapRequestedEventArgs args)
        {
            // Draw a red rectangle on a blue background.
            state.offscreenDrawingSession.Clear(Colors.Blue);

            // The rectangle starts out just beyond the left edge of the keyboard,
            // then moves to the right until it just leaves the keyboard,
            // and then repeats.
            float squareX = (state.frameNumber * (state.bitmapWidth + state.squareLength) / GeneratedBitmapEffectFrameCount) - state.squareLength;
            state.offscreenDrawingSession.FillRectangle(squareX, state.squareY, state.squareLength, state.squareLength, Colors.Red);

            state.offscreenDrawingSession.Flush();

            // Apply the generated bitmap to the LampArray.
            state.offscreenCanvas.GetPixelBytes(state.squareBuffer);
            state.squareBitmap.CopyFromBuffer(state.squareBuffer);

            args.UpdateBitmap(state.squareBitmap);

            // Advance to the next frame for the next iteration.
            if (++state.frameNumber == GeneratedBitmapEffectFrameCount)
            {
                state.frameNumber = 0;
            }
        }
        #endregion

        #region Color cycle effect
        private void CycleButton_Click(object sender, RoutedEventArgs e)
        {
            CleanupPreviousEffect();

            // This effect uses LampArrayColorRampEffect to perform a rainbow cycle pattern on all Lamps.
            // We do this for each LampArray by creating a LampArrayEffectPlaylist containing four LampArrayColorRampEffects.
            // Each effect will blend from the previously set color to a new color over a 500ms interval.

            // Gather the new LampArrayEffectPlaylists in this collection so we can start them simultaneously.
            var playlists = new List<LampArrayEffectPlaylist>();

            // Create the effect playlist for each LampArray and add it to the collection.
            foreach (LampArrayInfo info in m_attachedLampArrays)
            {
                var playlist = new LampArrayEffectPlaylist()
                {
                    EffectStartMode = LampArrayEffectStartMode.Sequential,
                    RepetitionMode = LampArrayRepetitionMode.Forever,
                };

                playlist.Append(new LampArrayColorRampEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    Color = Colors.Red,
                    RampDuration = ColorRampDuration,
                    CompletionBehavior = LampArrayEffectCompletionBehavior.KeepState
                });

                playlist.Append(new LampArrayColorRampEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    Color = Colors.Yellow,
                    RampDuration = ColorRampDuration,
                    CompletionBehavior = LampArrayEffectCompletionBehavior.KeepState
                });

                playlist.Append(new LampArrayColorRampEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    Color = Colors.Green,
                    RampDuration = ColorRampDuration,
                    CompletionBehavior = LampArrayEffectCompletionBehavior.KeepState
                });

                playlist.Append(new LampArrayColorRampEffect(info.lampArray, GetLampArrayIndices(info.lampArray))
                {
                    Color = Colors.Blue,
                    RampDuration = ColorRampDuration,
                    CompletionBehavior = LampArrayEffectCompletionBehavior.KeepState
                });

                // Add it to the list of playlists to start simultaneously.
                playlists.Add(playlist);
            }

            // Start the newly created effect playlists.
            LampArrayEffectPlaylist.StartAll(playlists);

            // Remember the playlists so we can stop them later.
            m_readyPlaylists = playlists;
        }
        #endregion

        private void PauseButton_Click(object sender, RoutedEventArgs e)
        {
            LampArrayEffectPlaylist.PauseAll(m_readyPlaylists);
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            LampArrayEffectPlaylist.StartAll(m_readyPlaylists);
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
                LampArrayEffectPlaylist.StopAll(m_readyPlaylists);
        }

        // Helper function that returns all indices of a LampArray in order.
        // Used when we want to apply an effect to all Lamps on a LampArray.
        private static int[] GetLampArrayIndices(LampArray lampArray)
        {
            return Enumerable.Range(0, lampArray.LampCount).ToArray();
        }
    }
}
