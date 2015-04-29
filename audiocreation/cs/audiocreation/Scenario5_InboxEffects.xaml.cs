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
using System.Threading.Tasks;
using Windows.Media.Audio;
using Windows.Media.Render;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AudioCreation
{
    /// <summary>
    /// This scenario is to demonstrate the use of Built-In Effects.
    /// </summary>
    public sealed partial class Scenario5_InboxEffects : Page
    {
        private MainPage rootPage;
        private AudioGraph graph;
        private AudioFileInputNode fileInputNode;
        private AudioDeviceOutputNode deviceOutputNode;
        private EchoEffectDefinition echoEffectDefinition;
        private ReverbEffectDefinition reverbEffectDefinition;
        private EqualizerEffectDefinition eqEffectDefinition;
        private LimiterEffectDefinition limiterEffectDefinition;

        public Scenario5_InboxEffects()
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

        private async void File_Click(object sender, RoutedEventArgs e)
        {
            // If another file is already loaded into the FileInput node
            if (fileInputNode != null)
            {
                // Release the file and dispose the contents of the node
                fileInputNode.Dispose();
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
            StorageFile file = await filePicker.PickSingleFileAsync();

            // File can be null if cancel is hit in the file picker
            if (file == null)
            {
                return;
            }

            CreateAudioFileInputNodeResult fileInputResult = await graph.CreateFileInputNodeAsync(file);
            if (fileInputResult.Status != AudioFileNodeCreationStatus.Success)
            {
                // Error reading the input file
                rootPage.NotifyUser(String.Format("Can't read input file because {0}", fileInputResult.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            // File loaded successfully. Enable these buttons in the UI
            graphButton.IsEnabled           = true;
            echoEffectToggle.IsEnabled      = true;
            reverbEffectToggle.IsEnabled    = true;
            limiterEffectToggle.IsEnabled   = true;
            eqToggle.IsEnabled              = true;

            fileInputNode = fileInputResult.FileInputNode;
            fileInputNode.AddOutgoingConnection(deviceOutputNode);

            rootPage.NotifyUser("Successfully loaded input file", NotifyType.StatusMessage);
            fileButton.Background = new SolidColorBrush(Colors.Green);

            // Create the four inbox effects
            CreateEchoEffect();
            CreateReverbEffect();
            CreateLimiterEffect();
            CreateEqEffect();
        }

        private void Graph_Click(object sender, RoutedEventArgs e)
        {
            TogglePlay();
        }

        private void TogglePlay()
        {
            if (graphButton.Content.Equals("Start Graph"))
            {
                graph.Start();
                graphButton.Content = "Stop Graph";
                audioPipe.Fill = new SolidColorBrush(Colors.Blue);
            }
            else
            {
                graph.Stop();
                graphButton.Content = "Start Graph";
                audioPipe.Fill = new SolidColorBrush(Color.FromArgb(255, 49, 49, 49));
            }
        }

        private void EchoEffectToggle_Toggled(object sender, RoutedEventArgs e)
        {
            // Enable/Disable the effect in the graph
            // Also enable/disable the associated UI for effect parameters
            if (echoEffectToggle.IsOn)
            {
                echoSlider.IsEnabled = true;
                echoLabel.Foreground = new SolidColorBrush(Colors.White);
                fileInputNode.EnableEffectsByDefinition(echoEffectDefinition);
            }
            else
            {
                echoSlider.IsEnabled = false;
                echoLabel.Foreground = new SolidColorBrush(Color.FromArgb(255,74,74,74));
                fileInputNode.DisableEffectsByDefinition(echoEffectDefinition);
            }
        }

        private void EchoSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if(echoEffectDefinition != null)
            {
                int currentValue = (int)echoSlider.Value;
                echoLabel.Text = "Delay: " + currentValue.ToString() + "ms";
                echoEffectDefinition.Delay = currentValue;
            }
        }

        private void ReverbEffectToggle_Toggled(object sender, RoutedEventArgs e)
        {
            // Enable/Disable the effect in the graph
            // Also enable/disable the associated UI for effect parameters
            if (reverbEffectToggle.IsOn)
            {
                decaySlider.IsEnabled = true;
                decayLabel.Foreground = new SolidColorBrush(Colors.White);
                fileInputNode.EnableEffectsByDefinition(reverbEffectDefinition);
            }
            else
            {
                decaySlider.IsEnabled = false;
                decayLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                fileInputNode.DisableEffectsByDefinition(reverbEffectDefinition);
            }
        }

        private void DecaySlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if(reverbEffectDefinition != null)
            {
                double currentValue = decaySlider.Value;
                decayLabel.Text = "Decay: " + currentValue.ToString() + "  s";
                reverbEffectDefinition.DecayTime = currentValue;
            }
        }

        private void LimiterEffectToggle_Toggled(object sender, RoutedEventArgs e)
        {
            // Enable/Disable the effect in the graph
            // Also enable/disable the associated UI for effect parameters
            if (limiterEffectToggle.IsOn)
            {
                loudnessSlider.IsEnabled = true;
                loudnessLabel.Foreground = new SolidColorBrush(Colors.White);
                fileInputNode.EnableEffectsByDefinition(limiterEffectDefinition);
            }
            else
            {
                loudnessSlider.IsEnabled = false;
                loudnessLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                fileInputNode.DisableEffectsByDefinition(limiterEffectDefinition);
            }
        }

        private void LoudnessSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (limiterEffectDefinition!= null)
            {
                uint currentValue = (uint)loudnessSlider.Value;
                limiterEffectDefinition.Loudness = currentValue;
                loudnessLabel.Text = "Loudness: " + currentValue.ToString();
            }
        }

        private void EqToggle_Toggled(object sender, RoutedEventArgs e)
        {
            // Enable/Disable the effect in the graph
            // Also enable/disable the associated UI for effect parameters
            if (eqToggle.IsOn)
            {
                eq1Slider.IsEnabled = true;
                eq2Slider.IsEnabled = true;
                eq3Slider.IsEnabled = true;
                eq4Slider.IsEnabled = true;
                eq1SliderLabel.Foreground = new SolidColorBrush(Colors.White);
                eq2SliderLabel.Foreground = new SolidColorBrush(Colors.White);
                eq3SliderLabel.Foreground = new SolidColorBrush(Colors.White);
                eq4SliderLabel.Foreground = new SolidColorBrush(Colors.White);
                fileInputNode.EnableEffectsByDefinition(eqEffectDefinition);
            }
            else
            {
                eq1Slider.IsEnabled = false;
                eq2Slider.IsEnabled = false;
                eq3Slider.IsEnabled = false;
                eq4Slider.IsEnabled = false;
                eq1SliderLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                eq2SliderLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                eq3SliderLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                eq4SliderLabel.Foreground = new SolidColorBrush(Color.FromArgb(255, 74, 74, 74));
                fileInputNode.DisableEffectsByDefinition(eqEffectDefinition);
            }
        }

        // Mapping the 0-100 scale of the slider to a value between the min and max gain
        private double ConvertRange(double value)
        {
            // These are the same values as the ones in xapofx.h
            const double fxeq_min_gain = 0.126;
            const double fxeq_max_gain = 7.94;

            double scale = (fxeq_max_gain - fxeq_min_gain)/100;
            return (fxeq_min_gain + ((value) * scale));
        }

        // Change effect paramters to reflect UI control
        private void Eq1Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if(eqEffectDefinition != null)
            {
                double currentValue = ConvertRange(eq1Slider.Value);
                eqEffectDefinition.Bands[0].Gain = currentValue;
            }
        }

        private void Eq2Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (eqEffectDefinition != null)
            {
                double currentValue = ConvertRange(eq2Slider.Value);
                eqEffectDefinition.Bands[1].Gain = currentValue;
            }
        }

        private void Eq3Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (eqEffectDefinition != null)
            {
                double currentValue = ConvertRange(eq3Slider.Value);
                eqEffectDefinition.Bands[2].Gain = currentValue;
            }
        }

        private void Eq4Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (eqEffectDefinition != null)
            {
                double currentValue = ConvertRange(eq4Slider.Value);
                eqEffectDefinition.Bands[3].Gain = currentValue;
            }
        }

        private void CreateEchoEffect()
        {
            // create echo effect
            echoEffectDefinition = new EchoEffectDefinition(graph);

            // See the MSDN page for parameter explanations
            // http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.xapofx.fxecho_parameters(v=vs.85).aspx
            echoEffectDefinition.WetDryMix    = 0.7f;
            echoEffectDefinition.Feedback     = 0.5f;
            echoEffectDefinition.Delay        = 500.0f;

            fileInputNode.EffectDefinitions.Add(echoEffectDefinition);
            fileInputNode.DisableEffectsByDefinition(echoEffectDefinition);
        }

        private void CreateReverbEffect()
        {
            // Create reverb effect
            reverbEffectDefinition = new ReverbEffectDefinition(graph);

            // See the MSDN page for parameter explanations
            // https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.xaudio2.xaudio2fx_reverb_parameters(v=vs.85).aspx
            reverbEffectDefinition.WetDryMix          = 50;
            reverbEffectDefinition.ReflectionsDelay   = 120;
            reverbEffectDefinition.ReverbDelay        = 30;
            reverbEffectDefinition.RearDelay          = 3;
            reverbEffectDefinition.DecayTime          = 2;

            fileInputNode.EffectDefinitions.Add(reverbEffectDefinition);
            fileInputNode.DisableEffectsByDefinition(reverbEffectDefinition);
        }

        private void CreateLimiterEffect()
        {
            // Create limiter effect
            limiterEffectDefinition = new LimiterEffectDefinition(graph);

            limiterEffectDefinition.Loudness = 1000;
            limiterEffectDefinition.Release = 10;

            fileInputNode.EffectDefinitions.Add(limiterEffectDefinition);
            fileInputNode.DisableEffectsByDefinition(limiterEffectDefinition);
        }

        private void CreateEqEffect()
        {
            // See the MSDN page for parameter explanations
            // https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.xapofx.fxeq_parameters(v=vs.85).aspx
            eqEffectDefinition = new EqualizerEffectDefinition(graph);
            eqEffectDefinition.Bands[0].FrequencyCenter   = 100.0f;
            eqEffectDefinition.Bands[0].Gain              = 4.033f;
            eqEffectDefinition.Bands[0].Bandwidth         = 1.5f;

            eqEffectDefinition.Bands[1].FrequencyCenter   = 900.0f;
            eqEffectDefinition.Bands[1].Gain              = 1.6888f;
            eqEffectDefinition.Bands[1].Bandwidth         = 1.5f;

            eqEffectDefinition.Bands[2].FrequencyCenter   = 5000.0f;
            eqEffectDefinition.Bands[2].Gain              = 2.4702f;
            eqEffectDefinition.Bands[2].Bandwidth         = 1.5f;

            eqEffectDefinition.Bands[3].FrequencyCenter   = 12000.0f;
            eqEffectDefinition.Bands[3].Gain              = 5.5958f;
            eqEffectDefinition.Bands[3].Bandwidth         = 2.0f;

            fileInputNode.EffectDefinitions.Add(eqEffectDefinition);
            fileInputNode.DisableEffectsByDefinition(eqEffectDefinition);
        }

        private async Task CreateAudioGraph()
        {
            // Create an AudioGraph with default settings
            AudioGraphSettings settings = new AudioGraphSettings(AudioRenderCategory.Media);
            CreateAudioGraphResult result = await AudioGraph.CreateAsync(settings);

            if (result.Status != AudioGraphCreationStatus.Success)
            {
                // Cannot create graph
                rootPage.NotifyUser(String.Format("AudioGraph Creation Error because {0}", result.Status.ToString()), NotifyType.ErrorMessage);
                return;
            }

            graph = result.Graph;

            // Create a device output node
            CreateAudioDeviceOutputNodeResult outputDeviceNodeResult = await graph.CreateDeviceOutputNodeAsync();

            if (outputDeviceNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                // Cannot create device output node
                rootPage.NotifyUser(String.Format("Audio Device Output unavailable because {0}", outputDeviceNodeResult.Status.ToString()), NotifyType.ErrorMessage);
                speakerContainer.Background = new SolidColorBrush(Colors.Red);
                return;
            }

            deviceOutputNode = outputDeviceNodeResult.DeviceOutputNode;
            rootPage.NotifyUser("Device Output Node successfully created", NotifyType.StatusMessage);
            speakerContainer.Background = new SolidColorBrush(Colors.Green);
        }
    }
}