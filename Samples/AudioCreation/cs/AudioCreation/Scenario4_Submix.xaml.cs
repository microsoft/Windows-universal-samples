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
using System.Threading.Tasks;
using Windows.Media.Audio;
using Windows.Media.Render;
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
    /// Shows use of a SubMix node with an effect applied to it.
    /// </summary>
    public sealed partial class Scenario4_Submix : Page
    {
        private MainPage rootPage;
        private AudioGraph graph;
        private AudioFileInputNode fileInputNode1, fileInputNode2;
        private AudioSubmixNode submixNode;
        private AudioDeviceOutputNode deviceOutputNode;
        private EchoEffectDefinition echoEffect;

        public Scenario4_Submix()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await CreateAudioGraph();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (graph != null)
            {
                graph.Dispose();
            }
        }

        private async void File1_Click(object sender, RoutedEventArgs e)
        {
            // If another file is already loaded into the FileInput node
            if (fileInputNode1 != null)
            {
                // Release the file and dispose the contents of the node
                fileInputNode1.Dispose();
                // Stop playback since a new file is being loaded. Also reset the button UI
                if (graphButton.Content.Equals("Stop Graph"))
                {
                    TogglePlay();
                }
            }

            FileOpenPicker filePicker = new FileOpenPicker();
            filePicker.SuggestedStartLocation = PickerLocationId.MusicLibrary;
            filePicker.FileTypeFilter.Add(".mp3");
            filePicker.FileTypeFilter.Add(".wma");
            filePicker.FileTypeFilter.Add(".wav");
            filePicker.ViewMode = PickerViewMode.Thumbnail;
            StorageFile file1 = await filePicker.PickSingleFileAsync();

            // File can be null if cancel is hit in the file picker
            if (file1 == null)
            {
                return;
            }

            CreateAudioFileInputNodeResult fileInputNodeResult = await graph.CreateFileInputNodeAsync(file1);
            if (fileInputNodeResult.Status != AudioFileNodeCreationStatus.Success)
            {
                // Cannot read input file
                rootPage.NotifyUser(String.Format("Can't read input file1 because {0}", fileInputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            fileInputNode1 = fileInputNodeResult.FileInputNode;
            // Since we are going to play two files simultaneously, set outgoing gain to 0.5 to prevent clipping
            fileInputNode1.AddOutgoingConnection(submixNode, 0.5);

            // The graph might already be playing, so set up UI accordingly
            if (graphButton.Content.Equals("Stop Graph"))
            {
                audioPipe1.Fill = new SolidColorBrush(Colors.Blue);
            }

            // UI tasks: enable buttons, show status message and change color of the input node box
            graphButton.IsEnabled = true;
            echoEffectToggle.IsEnabled = true;
            rootPage.NotifyUser("Loaded File 1", NotifyType.StatusMessage);
            fileButton1.Background = new SolidColorBrush(Colors.Green);
        }

        private async void File2_Click(object sender, RoutedEventArgs e)
        {
            // If another file is already loaded into the FileInput node
            if (fileInputNode2 != null)
            {
                // Release the file and dispose the contents of the node
                fileInputNode2.Dispose();
                // Stop playback since a new file is being loaded. Also reset the button UI
                if (graphButton.Content.Equals("Stop Graph"))
                {
                    TogglePlay();
                }
            }

            FileOpenPicker filePicker = new FileOpenPicker();
            filePicker.SuggestedStartLocation = PickerLocationId.MusicLibrary;
            filePicker.FileTypeFilter.Add(".mp3");
            filePicker.ViewMode = PickerViewMode.Thumbnail;
            StorageFile file2 = await filePicker.PickSingleFileAsync();

            // File can be null if cancel is hit in the file picker
            if (file2 == null)
            {
                return;
            }

            CreateAudioFileInputNodeResult fileInputNodeResult = await graph.CreateFileInputNodeAsync(file2);
            if (fileInputNodeResult.Status != AudioFileNodeCreationStatus.Success)
            {
                // Cannot read input file
                rootPage.NotifyUser(String.Format("Can't read input file2 because {0}", fileInputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            fileInputNode2 = fileInputNodeResult.FileInputNode;
            // Since we are going to play two files simultaneously, set outgoing gain to 0.5 to prevent clipping
            fileInputNode2.AddOutgoingConnection(submixNode, 0.5);

            // The graph might already be playing, so set up UI accordingly
            if (graphButton.Content.Equals("Stop Graph"))
            {
                audioPipe2.Fill = new SolidColorBrush(Colors.Blue);
            }

            // UI tasks: enable buttons, show status message and change color of the input node box
            graphButton.IsEnabled = true;
            echoEffectToggle.IsEnabled = true;
            rootPage.NotifyUser("Loaded File 2", NotifyType.StatusMessage);
            fileButton2.Background = new SolidColorBrush(Colors.Green);
        }

        private void GraphButton_Click(object sender, RoutedEventArgs e)
        {
            TogglePlay();
        }

        private void TogglePlay()
        {
            if (graphButton.Content.Equals("Start Graph"))
            {
                graph.Start();
                graphButton.Content = "Stop Graph";
                audioPipe3.Fill = new SolidColorBrush(Colors.Blue);
                if (fileInputNode1 != null)
                {
                    audioPipe1.Fill = new SolidColorBrush(Colors.Blue);
                }
                if (fileInputNode2 != null)
                {
                    audioPipe2.Fill = new SolidColorBrush(Colors.Blue);
                }
            }
            else if (graphButton.Content.Equals("Stop Graph"))
            {
                graph.Stop();
                graphButton.Content = "Start Graph";
                audioPipe1.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
                audioPipe2.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
                audioPipe3.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
            }
        }

        private void EchoEffectToggle_Toggled(object sender, RoutedEventArgs e)
        {
            // Enable/Disable the echo effect
            // Also set the label for the UI
            if (echoEffectToggle.IsOn)
            {
                submixNode.EnableEffectsByDefinition(echoEffect);
            }
            else
            {
                submixNode.DisableEffectsByDefinition(echoEffect);
            }
        }

        private async Task CreateAudioGraph()
        {
            // Create an AudioGraph with default setting
            AudioGraphSettings settings = new AudioGraphSettings(AudioRenderCategory.Media);
            CreateAudioGraphResult result = await AudioGraph.CreateAsync(settings);

            if (result.Status != AudioGraphCreationStatus.Success)
            {
                // Can't create the graph
                rootPage.NotifyUser(String.Format("AudioGraph Creation Error because {0}", result.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            graph = result.Graph;

            // Create a device output node
            CreateAudioDeviceOutputNodeResult deviceOutputNodeResult = await graph.CreateDeviceOutputNodeAsync();

            if (deviceOutputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device output node
                rootPage.NotifyUser(String.Format("Audio Device Output unavailable because {0}", deviceOutputNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                speakerContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceOutputNode = deviceOutputNodeResult.DeviceOutputNode;
            rootPage.NotifyUser("Device Output Node successfully created", NotifyType.StatusMessage);
            speakerContainer.Background = new SolidColorBrush(Colors.Green);

            submixNode = graph.CreateSubmixNode();
            submixNodeContainer.Background = new SolidColorBrush(Colors.Green);
            submixNode.AddOutgoingConnection(deviceOutputNode);

            echoEffect = new EchoEffectDefinition(graph);
            echoEffect.WetDryMix = 0.7f;
            echoEffect.Feedback = 0.5f;
            echoEffect.Delay = 500.0f;
            submixNode.EffectDefinitions.Add(echoEffect);

            // Disable the effect in the beginning. Enable in response to user action (UI toggle switch)
            submixNode.DisableEffectsByDefinition(echoEffect);

            // All nodes can have an OutgoingGain property
            // Setting the gain on the Submix node attenuates the output of the node
            submixNode.OutgoingGain = 0.5;

            // Graph successfully created. Enable buttons to load files
            fileButton1.IsEnabled = true;
            fileButton2.IsEnabled = true;
        }
    }
}
