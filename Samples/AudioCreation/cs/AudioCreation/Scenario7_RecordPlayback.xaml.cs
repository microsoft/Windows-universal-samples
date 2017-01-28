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
        private MainPage rootPage;
        private AudioGraph graph;
        private AudioDeviceOutputNode deviceOutputNode;
        private AudioDeviceInputNode deviceInputNode;
        private AudioFrameOutputNode frameOutputNode;
        private AudioFrameInputNode frameInputNode;

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
        private volatile bool requestStartRecording;
        private volatile bool requestStartPlaying;
        private volatile bool isRecording;
        private volatile bool isPlaying;

        // Current record/play index into the buffer.
        private uint currentIndex;
        // Maximum index recorded in the buffer.
        private uint maxIndex;

        // 120 seconds of 48Khz stereo float audio samples (43.9MB byte array)
        private byte[] byteBuffer = new byte[120 * 48000 * 2 * 4];
        private DeviceInformationCollection outputDevices;

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

                recordButton.Content = "Stop";
                playButton.IsEnabled = false;

                // This volatile write will be polled by the audio thread and will cause recording to start there.
                requestStartRecording = true;
            }
            else
            {
                Debug.Assert(isRecording);
                Debug.Assert(!isPlaying);

                recordButton.Content = "Record";
                rootPage.NotifyUser($"Recording to memory buffer completed successfully! {maxIndex} bytes recorded", NotifyType.StatusMessage);
                createGraphButton.IsEnabled = false;
                playButton.IsEnabled = true;

                // This volatile write will cause recording to stop on the audio thread.
                isRecording = false;
            }
        }

        private void TogglePlay()
        {
            if (!isPlaying)
            {
                Debug.Assert(!isRecording);
                Debug.Assert(!isPlaying);

                playButton.Content = "Stop";
                recordButton.IsEnabled = false;

                // This volatile write will cause playing to start once the audio thread polls it.
                requestStartPlaying = true;
            }
            else if (playButton.Content.Equals("Stop"))
            {
                Debug.Assert(!isRecording);
                Debug.Assert(isPlaying);

                playButton.Content = "Play";
                rootPage.NotifyUser("Playing from memory buffer completed successfully!", NotifyType.StatusMessage);
                recordButton.IsEnabled = true;

                // This volatile write will stop playing on the audio thread immediately.
                isPlaying = false;
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
            }

            if (isRecording)
            {
                HandleIncomingAudio();
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
                        byte b = dataInBytes[i];
                        buf[i + maxIndex] = b;
                    }
                }
                maxIndex += capacityInBytes;
            }
        }

        private void FrameInputNode_QuantumStarted(AudioFrameInputNode sender, FrameInputNodeQuantumStartedEventArgs args)
        {
            Debug.Assert(args.RequiredSamples >= 0);
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
                HandleOutgoingAudio(requiredSamples);
            }
        }

        // We keep the last AudioFrame in the common case that it's the same size as the next requested buffer.
        static AudioFrame s_lastAudioFrame = null;
        static uint s_lastAudioFrameSampleCount = 0;

        /// <summary>
        /// Handle a frame of outgoing audio to the output device.
        /// </summary>
        private unsafe void HandleOutgoingAudio(uint requiredSampleCount)
        {
            Debug.Assert(requiredSampleCount > 0);

            uint bufferSize = requiredSampleCount * sizeof(float) * 2;

            AudioFrame frame;
            if (s_lastAudioFrameSampleCount == requiredSampleCount)
            {
                frame = s_lastAudioFrame;
            }
            else
            {
                frame = new Windows.Media.AudioFrame(bufferSize);
                s_lastAudioFrame = frame;
                s_lastAudioFrameSampleCount = bufferSize;
            }

            using (AudioBuffer buffer = frame.LockBuffer(AudioBufferAccessMode.Write))
            using (IMemoryBufferReference reference = buffer.CreateReference())
            {
                byte* dataInBytes;
                uint capacityInBytes;

                // Get the buffer from the AudioFrame
                ((IMemoryBufferByteAccess)reference).GetBuffer(out dataInBytes, out capacityInBytes);

                Debug.Assert(requiredSampleCount == capacityInBytes / 2 / 4);

                uint tailCount = 0;

                if (capacityInBytes + currentIndex > maxIndex)
                {
                    Debug.Assert(maxIndex >= currentIndex);
                    uint newCapacityInBytes = Math.Max(0, maxIndex - currentIndex);
                    tailCount = capacityInBytes - newCapacityInBytes;
                    capacityInBytes = newCapacityInBytes;
                }

                fixed (byte* buf = byteBuffer)
                {
                    for (int i = 0; i < capacityInBytes; i++)
                    {
                        dataInBytes[i] = buf[i + currentIndex];
                    }
                    currentIndex += capacityInBytes;
                    if (currentIndex == maxIndex)
                    {
                        // loop time!
                        currentIndex = 0;
                    }
                    for (int i = 0; i < tailCount; i++)
                    {
                        dataInBytes[capacityInBytes + i] = buf[i];
                    }
                }
            }

            frameInputNode.AddFrame(frame);
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
            // We keep the output node and input node running all the time, even if they have nothing to do.
            // Optimizations might be possible here but this gets our inner loop running consistently.
            frameOutputNode.Start();

            frameInputNode = graph.CreateFrameInputNode();
            frameInputNode.AddOutgoingConnection(deviceOutputNode);
            frameInputNode.QuantumStarted += FrameInputNode_QuantumStarted;
            frameInputNode.Start();
                                                        
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
    }
}
