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

using SDKTemplate;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Media;
using Windows.Media.Audio;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.Render;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AudioCreation
{
    /// <summary>
    /// This scenario shows using AudioGraph for audio capture from a microphone with low latency
    /// to a memory buffer, and looping playback of the captured audio buffer.
    /// </summary>
    public sealed partial class Scenario7_RecordPlayback : Page
    {
        MainPage rootPage;
        AudioGraph graph;
        AudioDeviceOutputNode deviceOutputNode;
        AudioDeviceInputNode deviceInputNode;
        AudioFrameOutputNode frameOutputNode;
        AudioFrameInputNode frameInputNode;

        // These variables are synchronization points; they experience the following
        // state transitions:
        // requestStartRecording: false -> true (UI thread, user clicks Start Recording)
        // requestStartRecording: true -> false (audio thread, when it registers request)
        // isRecording: false -> true (audio thread, when it registers request)
        // isRecording: true -> false (UI thread, user clicks Stop Recording)
        // isRecording: true -> false (audio thread, buffer reaches maximum capacity)
        //
        // And similarly for requestStartPlaying and isPlaying.
        //
        volatile bool requestStartRecording;
        volatile bool requestStartPlaying;
        volatile bool isRecording;
        volatile bool isPlaying;
        volatile int audioFrameCount;

        DateTime recordingStartTime;
        int lastStatusMsec;

        uint maxCapacityEncountered;
        uint lastCapacityEncountered;

        // Current record/play index into the buffer.
        uint currentIndex;
        // Maximum index recorded in the buffer.
        uint maxIndex;

        // 120 seconds of 48Khz stereo float audio samples (43.9MB byte array)
        byte[] byteBuffer = new byte[120 * 48000 * 2 * 4];
        DeviceInformationCollection outputDevices;

        public Scenario7_RecordPlayback()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await PopulateDeviceList();

            
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (graph != null)
            {
                graph.Stop();
                graph.Dispose();
            }
        }

        private void RecordButton_Click(object sender, RoutedEventArgs e)
        {
            ToggleRecord();
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            TogglePlay();
        }

        private async void CreateGraphButton_Click(object sender, RoutedEventArgs e)
        {
            await CreateAudioGraph();
        }

        private void ToggleRecord()
        {
            if (!isRecording)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!isPlaying);

                // This volatile write will be polled by the audio thread and will cause recording to start there.
                requestStartRecording = true;
                frameOutputNode.Start();

                recordButton.Content = "Stop";
                playButton.IsEnabled = false;
            }
            else
            {
                Debug.Assert(isRecording);
                Debug.Assert(!isPlaying);

                // This volatile write will cause recording to stop on the audio thread.
                isRecording = false;
                frameOutputNode.Stop();

                recordButton.Content = "Record";
                TimeSpan span = DateTime.Now - recordingStartTime;
                int msec = span.Seconds * 1000 + span.Milliseconds;
                rootPage.NotifyUser($"Recorded OK! {maxIndex} bytes, {maxIndex>>3} samples, {audioFrameCount} frames, {msec} msec, last cap {lastCapacityEncountered}, max cap {maxCapacityEncountered}", NotifyType.StatusMessage);
                createGraphButton.IsEnabled = false;
                playButton.IsEnabled = true;
                audioPipe.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
            }
        }

        private void TogglePlay()
        {
            if (!isPlaying)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!isPlaying);

                // This volatile write will cause playing to start once the audio thread polls it.
                requestStartPlaying = true;
                frameInputNode.Start();

                playButton.Content = "Stop";
                recordButton.IsEnabled = false;
            }
            else if (playButton.Content.Equals("Stop"))
            {
                Debug.Assert(!isRecording);
                Debug.Assert(isPlaying);

                // This volatile write will stop playing on the audio thread immediately.
                isPlaying = false;
                frameInputNode.Stop();
                frameInputNode.DiscardQueuedFrames();

                playButton.Content = "Play";
                rootPage.NotifyUser("Playing from memory buffer completed successfully!", NotifyType.StatusMessage);
                recordButton.IsEnabled = true;
            }
        }

        private unsafe void Graph_QuantumStarted(AudioGraph sender, object args)
        {
            if (requestStartRecording)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!requestStartPlaying);
                Debug.Assert(!isPlaying);

                isRecording = true;
                requestStartRecording = false;
                maxIndex = 0;
                currentIndex = 0;
                audioFrameCount = 0;
                recordingStartTime = DateTime.Now;
                lastCapacityEncountered = maxCapacityEncountered = 0;
            }

            if (isRecording)
            {
                Debug.Assert(!isPlaying);
                Debug.Assert(!requestStartPlaying);
                Debug.Assert(!requestStartRecording);

                HandleIncomingAudio();

                /*
                int currentStatusMsec = DateTime.Now.Millisecond;
                // update status every tenth of a second
                if (lastStatusMsec % 100 > currentStatusMsec % 100)
                {
                    CoreApplication.MainView.CoreWindow.Dispatcher.TryRunAsync(
                        Windows.UI.Core.CoreDispatcherPriority.Normal,
                        UpdateStatusWhileRecording);
                }
                lastStatusMsec = currentStatusMsec;
                */
            }
        }

        /// <summary>
        /// Handle a frame of incoming audio from the input device.
        /// </summary>
        private unsafe void HandleIncomingAudio()
        {
            AudioFrame frame = frameOutputNode.GetFrame();

            using (AudioBuffer buffer = frame.LockBuffer(AudioBufferAccessMode.Read))
            using (IMemoryBufferReference reference = buffer.CreateReference())
            {
                byte* dataInBytes;
                uint capacityInBytes;

                // Get the buffer from the AudioFrame
                ((IMemoryBufferByteAccess)reference).GetBuffer(out dataInBytes, out capacityInBytes);

                if (capacityInBytes == 0)
                {
                    // we don't count zero-byte frames... and why do they ever happen???
                    return;
                }

                // Must be multiple of 8 (2 channels, 4 bytes/float)
                Debug.Assert((capacityInBytes & 0x7) == 0);

                lastCapacityEncountered = capacityInBytes;
                maxCapacityEncountered = Math.Max(maxCapacityEncountered, capacityInBytes);

                uint bufferStart = 0;
                if (maxIndex == 0)
                {
                    // if maxCapacityEncountered is greater than the audio graph buffer size, 
                    // then the audio graph decided to give us a big backload of buffer content
                    // as its first callback.  Not sure why it does this, but we don't want it,
                    // so take only the tail of the buffer.
                    if (maxCapacityEncountered > (graph.LatencyInSamples << 3))
                    {
                        bufferStart = capacityInBytes - (uint)(graph.LatencyInSamples << 3);
                        capacityInBytes = (uint)(graph.LatencyInSamples << 3);
                    }
                }

                // if we fill up, just spin forever.  don't try to exit recording from audio thread,
                // we don't bother with two-way signaling.
                if (maxIndex + capacityInBytes > byteBuffer.Length)
                {
                    Debug.Assert(byteBuffer.Length >= maxIndex);
                    capacityInBytes = (uint)byteBuffer.Length - maxIndex;
                }

                fixed (byte* buf = byteBuffer)
                {
                    for (int i = 0; i < capacityInBytes; i++)
                    {
                        byte b = dataInBytes[i + bufferStart];
                        buf[i + maxIndex] = b;
                    }
                }
                maxIndex += capacityInBytes;
            }

            audioFrameCount++;
        }

        private void UpdateStatusWhileRecording()
        {
            uint max = maxIndex >> 3;
            uint curr = currentIndex >> 3;
            int frameCount = audioFrameCount;
            // may have stopped recording while this async notification was in flight
            if (isRecording)
            {
                rootPage.NotifyUser($"recording: max samples {max}, frame count {frameCount}, last cap {lastCapacityEncountered}, max cap {maxCapacityEncountered}", NotifyType.StatusMessage);
            }
        }

        private void FrameInputNode_QuantumStarted(AudioFrameInputNode sender, FrameInputNodeQuantumStartedEventArgs args)
        {
            uint requiredSamples = (uint)args.RequiredSamples;

            if (requestStartPlaying)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!requestStartRecording);
                Debug.Assert(!isPlaying);

                isPlaying = true;
                requestStartPlaying = false;
                currentIndex = 0;
            }

            if (isPlaying)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!requestStartPlaying);
                Debug.Assert(!requestStartRecording);

                HandleOutgoingAudio(requiredSamples);

                /*
                int currentStatusMsec = DateTime.Now.Millisecond;
                // update status every tenth of a second
                if (lastStatusMsec % 100 > currentStatusMsec % 100)
                {
                    CoreApplication.MainView.CoreWindow.Dispatcher.TryRunAsync(
                        Windows.UI.Core.CoreDispatcherPriority.Normal,
                        UpdateStatusWhilePlaying);
                }
                lastStatusMsec = currentStatusMsec;
                */
            }
        }

        // We keep a 1MB AudioFrame and reuse it.
        static AudioFrame s_audioFrame = new Windows.Media.AudioFrame(1024 * 1024);
        private int s_zeroByteOutgoingFrameCount;

        /// <summary>
        /// Handle a frame of outgoing audio to the output device.
        /// </summary>
        /// <remarks>
        /// This pushes a megabyte of audio each time, to greatly reduce the number of callbacks.
        /// 
        /// We don't need the requiredSamples argument but we don't 
        /// </remarks>
        private unsafe void HandleOutgoingAudio(uint requiredSamples)
        {
            if (requiredSamples == 0)
            {
                s_zeroByteOutgoingFrameCount++;
                return;
            }

            using (AudioBuffer buffer = s_audioFrame.LockBuffer(AudioBufferAccessMode.Write))
            using (IMemoryBufferReference reference = buffer.CreateReference())
            {
                byte* dataInBytes;
                uint capacityInBytes;

                // Get the buffer from the AudioFrame
                ((IMemoryBufferByteAccess)reference).GetBuffer(out dataInBytes, out capacityInBytes);

                uint bytesRemaining = capacityInBytes;
                uint position = 0;

                while (bytesRemaining > 0)
                {
                    uint bytesInThisIteration = bytesRemaining;
                    if (currentIndex + bytesRemaining > maxIndex)
                    {
                        bytesInThisIteration = maxIndex - currentIndex;
                        if (bytesInThisIteration == 0)
                        {
                            // wraparound; next iteration will get more bytes
                            currentIndex = 0;
                            continue;
                        }
                    }

                    fixed (byte* buf = byteBuffer)
                    {
                        for (int i = 0; i < bytesInThisIteration; i++)
                        {
                            dataInBytes[position + i] = buf[i + currentIndex];
                        }
                    }
                    currentIndex += bytesInThisIteration;
                    position += bytesInThisIteration;

                    bytesRemaining -= bytesInThisIteration;
                }
            }

            frameInputNode.AddFrame(s_audioFrame);
        }

        private void UpdateStatusWhilePlaying()
        {
            uint max = maxIndex >> 3;
            uint curr = currentIndex >> 3;
            // may have stopped playing while this async notification was in flight
            if (isPlaying)
            {
                rootPage.NotifyUser($"playing: curr sample {curr}. max samples {max}", NotifyType.StatusMessage);
            }
        }

        private async Task PopulateDeviceList()
        {
            outputDevicesListBox.Items.Clear();
            outputDevices = await DeviceInformation.FindAllAsync(MediaDevice.GetAudioRenderSelector());
            outputDevicesListBox.Items.Add("-- Pick output device --");
            foreach (var device in outputDevices)
            {
                outputDevicesListBox.Items.Add(device.Name);
            }
        }

        private async Task CreateAudioGraph()
        {
            AudioGraphSettings settings = new AudioGraphSettings(AudioRenderCategory.Media);
            settings.QuantumSizeSelectionMode = QuantumSizeSelectionMode.LowestLatency;
            settings.PrimaryRenderDevice = outputDevices[outputDevicesListBox.SelectedIndex - 1];

            CreateAudioGraphResult result = await AudioGraph.CreateAsync(settings);

            if (result.Status != AudioGraphCreationStatus.Success)
            {
                // Cannot create graph
                rootPage.NotifyUser(String.Format("AudioGraph Creation Error because {0}", result.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            graph = result.Graph;
            rootPage.NotifyUser("Graph successfully created!", NotifyType.StatusMessage);

            // Create a device output node
            CreateAudioDeviceOutputNodeResult deviceOutputNodeResult = await graph.CreateDeviceOutputNodeAsync();
            if (deviceOutputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device output node
                rootPage.NotifyUser(String.Format("Audio Device Output unavailable because {0}", deviceOutputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                outputDeviceContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceOutputNode = deviceOutputNodeResult.DeviceOutputNode;
            rootPage.NotifyUser("Device Output connection successfully created", NotifyType.StatusMessage);
            outputDeviceContainer.Background = new SolidColorBrush(Colors.Green);

            // Create a device input node using the default audio input device
            CreateAudioDeviceInputNodeResult deviceInputNodeResult = await graph.CreateDeviceInputNodeAsync(MediaCategory.Other);

            if (deviceInputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device input node
                rootPage.NotifyUser(String.Format("Audio Device Input unavailable because {0}", deviceInputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                inputDeviceContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceInputNode = deviceInputNodeResult.DeviceInputNode;
            rootPage.NotifyUser("Device Input connection successfully created", NotifyType.StatusMessage);
            inputDeviceContainer.Background = new SolidColorBrush(Colors.Green);

            frameOutputNode = graph.CreateFrameOutputNode();
            deviceInputNode.AddOutgoingConnection(frameOutputNode);

            frameInputNode = graph.CreateFrameInputNode();
            frameInputNode.AddOutgoingConnection(deviceOutputNode);
            frameInputNode.QuantumStarted += FrameInputNode_QuantumStarted;
                                                        
            // Attach to QuantumStarted event in order to receive synchronous updates from audio graph (to capture incoming audio).
            graph.QuantumStarted += Graph_QuantumStarted;

            // Disable the graph button to prevent accidental click
            createGraphButton.IsEnabled = false;

            // Because we are using lowest latency setting, we need to handle device disconnection errors
            graph.UnrecoverableErrorOccurred += Graph_UnrecoverableErrorOccurred;

            // Start the graph and keep it running.
            graph.Start();

            // Enable recording.
            recordButton.IsEnabled = true;

            createGraphButton.Background = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));

        }

        private async void Graph_UnrecoverableErrorOccurred(AudioGraph sender, AudioGraphUnrecoverableErrorOccurredEventArgs args)
        {
            // Recreate the graph and all nodes when this happens
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
            {
                sender.Dispose();
                // Re-query for devices
                await PopulateDeviceList();
                // Reset UI
                recordButton.IsEnabled = true;
                recordButton.Content = "Record";
                playButton.IsEnabled = false;
                playButton.Content = "Play";
                isRecording = isPlaying = false;
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
            });
        }

        private void outputDevicesListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (outputDevicesListBox.SelectedIndex == 0)
            {
                createGraphButton.IsEnabled = false;
                outputDevice.Foreground = new SolidColorBrush(Color.FromArgb(255, 110, 110, 110));
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));

                // Destroy graph
                if (graph != null)
                {
                    graph.Dispose();
                    graph = null;
                }
            }
            else
            {
                createGraphButton.IsEnabled = true;
                outputDevice.Foreground = new SolidColorBrush(Colors.White);
            }
        }

        private void inputDevicesListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
