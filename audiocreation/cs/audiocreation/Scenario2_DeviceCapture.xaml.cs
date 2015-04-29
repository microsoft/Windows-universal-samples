//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Media.Audio;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.Media.Render;
using Windows.Media.Transcoding;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AudioCreation
{
    /// <summary>
    /// This scenario shows using AudioGraph for audio capture from a microphone with low latency.
    /// </summary>
    public sealed partial class Scenario2_DeviceCapture : Page
    {
        private MainPage rootPage;
        private AudioGraph graph;
        private AudioFileOutputNode fileOutputNode;
        private AudioDeviceOutputNode deviceOutputNode;
        private AudioDeviceInputNode deviceInputNode;
        private DeviceInformationCollection outputDevices;

        public Scenario2_DeviceCapture()
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
                graph.Dispose();
            }
        }

        private async void FileButton_Click(object sender, RoutedEventArgs e)
        {
            await SelectOutputFile();
        }

        private async void RecordStopButton_Click(object sender, RoutedEventArgs e)
        {
            await ToggleRecordStop();
        }

        private async void CreateGraphButton_Click(object sender, RoutedEventArgs e)
        {
            await CreateAudioGraph();
        }

        private async Task SelectOutputFile()
        {
            FileSavePicker saveFilePicker = new FileSavePicker();
            saveFilePicker.FileTypeChoices.Add("Pulse Code Modulation", new List<string>() { ".wav" });
            saveFilePicker.FileTypeChoices.Add("Windows Media Audio", new List<string>() { ".wma" });
            saveFilePicker.FileTypeChoices.Add("MPEG Audio Layer-3", new List<string>() { ".mp3" });
            saveFilePicker.SuggestedFileName = "New Audio Track";
            StorageFile file = await saveFilePicker.PickSaveFileAsync();

            // File can be null if cancel is hit in the file picker
            if (file == null)
            {
                return;
            }

            rootPage.NotifyUser(String.Format("Recording to {0}", file.Name.ToString()), NotifyType.StatusMessage);
            MediaEncodingProfile fileProfile = CreateMediaEncodingProfile(file);

            // Operate node at the graph format, but save file at the specified format
            CreateAudioFileOutputNodeResult fileOutputNodeResult = await graph.CreateFileOutputNodeAsync(file, fileProfile);

            if (fileOutputNodeResult.Status != AudioFileNodeCreationStatus.Success)
            {
                // FileOutputNode creation failed
                rootPage.NotifyUser(String.Format("Cannot create output file because {0}", fileOutputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                fileButton.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            fileOutputNode = fileOutputNodeResult.FileOutputNode;
            fileButton.Background = new SolidColorBrush(Colors.YellowGreen);

            // Connect the input node to both output nodes
            deviceInputNode.AddOutgoingConnection(fileOutputNode);
            deviceInputNode.AddOutgoingConnection(deviceOutputNode);
            recordStopButton.IsEnabled = true;
        }

        private MediaEncodingProfile CreateMediaEncodingProfile(StorageFile file)
        {
            switch(file.FileType.ToString().ToLowerInvariant())
            {
                case ".wma":
                    return MediaEncodingProfile.CreateWma(AudioEncodingQuality.High);
                case ".mp3":
                    return MediaEncodingProfile.CreateMp3(AudioEncodingQuality.High);
                case ".wav":
                    return MediaEncodingProfile.CreateWav(AudioEncodingQuality.High);
                default:
                    throw new ArgumentException();
            }
        }

        private async Task ToggleRecordStop()
        {
            if (recordStopButton.Content.Equals("Record"))
            {
                graph.Start();
                recordStopButton.Content = "Stop";
                audioPipe1.Fill = new SolidColorBrush(Colors.Blue);
                audioPipe2.Fill = new SolidColorBrush(Colors.Blue);
            }
            else if (recordStopButton.Content.Equals("Stop"))
            {
                // Good idea to stop the graph to avoid data loss
                graph.Stop();
                audioPipe1.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
                audioPipe2.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));

                TranscodeFailureReason finalizeResult = await fileOutputNode.FinalizeAsync();
                if (finalizeResult != TranscodeFailureReason.None)
                {
                    // Finalization of file failed. Check result code to see why
                    rootPage.NotifyUser(String.Format("Finalization of file failed because {0}", finalizeResult.ToString()), NotifyType.ErrorMessage);
                    fileButton.Background = new SolidColorBrush(Colors.Red);
                    return;
                }

                recordStopButton.Content = "Record";
                rootPage.NotifyUser("Recording to file completed successfully!", NotifyType.StatusMessage);
                fileButton.Background = new SolidColorBrush(Colors.Green);
                recordStopButton.IsEnabled = false;
                createGraphButton.IsEnabled = false;
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

            // Since graph is successfully created, enable the button to select a file output
            fileButton.IsEnabled = true;

            // Disable the graph button to prevent accidental click
            createGraphButton.IsEnabled = false;

            // Because we are using lowest latency setting, we need to handle device disconnection errors
            graph.UnrecoverableErrorOccurred += Graph_UnrecoverableErrorOccurred;
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
                fileButton.IsEnabled = false;
                recordStopButton.IsEnabled = false;
                recordStopButton.Content = "Record";
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                audioPipe1.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
                audioPipe2.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
            });
        }

        private void outputDevicesListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (outputDevicesListBox.SelectedIndex == 0)
            {
                createGraphButton.IsEnabled = false;
                outputDevice.Foreground = new SolidColorBrush(Color.FromArgb(255, 110, 110, 110));
                outputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                fileButton.IsEnabled = false;
                fileButton.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                inputDeviceContainer.Background = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));

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
