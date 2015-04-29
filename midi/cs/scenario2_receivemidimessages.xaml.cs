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
using System.Text;
using Windows.Devices.Enumeration;
using Windows.Devices.Midi;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace MIDI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_ReceiveMIDIMessages : Page
    {
        /// <summary>
        /// Main Page 
        /// </summary>
        private MainPage rootPage;

        /// <summary>
        /// Collection of active MidiInPorts
        /// </summary>
        private List<MidiInPort> midiInPorts;

        /// <summary>
        /// Device watcher for MIDI in ports
        /// </summary>
        MidiDeviceWatcher midiInDeviceWatcher;

        /// <summary>
        /// Set the rootpage context when entering the scenario
        /// </summary>
        /// <param name="e">Event arguments</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.rootPage = MainPage.Current;
        }

        /// <summary>
        /// Stop the device watcher when leaving the scenario
        /// </summary>
        /// <param name="e">Event arguments</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);

            // Stop the input device watcher
            this.midiInDeviceWatcher.Stop();

            // Close all MidiInPorts
            foreach (MidiInPort inPort in this.midiInPorts)
            {
                inPort.Dispose();
            }
            this.midiInPorts.Clear();
        }

        /// <summary>
        /// Constructor: Start the device watcher
        /// </summary>
        public Scenario2_ReceiveMIDIMessages()
        {
            this.InitializeComponent();

            // Initialize the list of active MIDI input devices
            this.midiInPorts = new List<MidiInPort>();

            // Set up the MIDI input device watcher
            this.midiInDeviceWatcher = new MidiDeviceWatcher(MidiInPort.GetDeviceSelector(), Dispatcher, this.inputDevices);

            // Start watching for devices
            this.midiInDeviceWatcher.Start();
        }

        /// <summary>
        /// Change the input MIDI device from which to receive messages
        /// </summary>
        /// <param name="sender">Element that fired the event</param>
        /// <param name="e">Event arguments</param>
        private async void inputDevices_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Get the selected input MIDI device
            int selectedInputDeviceIndex = (sender as ListBox).SelectedIndex;

            // Try to create a MidiInPort
            if (selectedInputDeviceIndex < 0)
            {
                // Clear input device messages
                this.inputDeviceMessages.Items.Clear();
                this.inputDeviceMessages.Items.Add("Select a MIDI input device to be able to see its messages");
                this.inputDeviceMessages.IsEnabled = false;
                this.rootPage.NotifyUser("Select a MIDI input device to be able to see its messages", NotifyType.StatusMessage);
                return;
            }

            DeviceInformationCollection devInfoCollection = this.midiInDeviceWatcher.GetDeviceInformationCollection();
            if (devInfoCollection == null)
            {
                this.inputDeviceMessages.Items.Clear();
                this.inputDeviceMessages.Items.Add("Device not found!");
                this.inputDeviceMessages.IsEnabled = false;
                this.rootPage.NotifyUser("Device not found!", NotifyType.ErrorMessage);
                return;
            }

            DeviceInformation devInfo = devInfoCollection[selectedInputDeviceIndex];
            if (devInfo == null)
            {
                this.inputDeviceMessages.Items.Clear();
                this.inputDeviceMessages.Items.Add("Device not found!");
                this.inputDeviceMessages.IsEnabled = false;
                this.rootPage.NotifyUser("Device not found!", NotifyType.ErrorMessage);
                return;
            }

            var currentMidiInputDevice = await MidiInPort.FromIdAsync(devInfo.Id);
            if (currentMidiInputDevice == null)
            {
                this.rootPage.NotifyUser("Unable to create MidiInPort from input device", NotifyType.ErrorMessage);
                return;
            }

            // We have successfully created a MidiInPort; add the device to the list of active devices, and set up message receiving
            if (!this.midiInPorts.Contains(currentMidiInputDevice))
            {
                this.midiInPorts.Add(currentMidiInputDevice);
                currentMidiInputDevice.MessageReceived += MidiInputDevice_MessageReceived;
            }

            // Clear any previous input messages
            this.inputDeviceMessages.Items.Clear();
            this.inputDeviceMessages.IsEnabled = true;

            this.rootPage.NotifyUser("Input Device selected successfully! Waiting for messages...", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Display the received MIDI message in a readable format
        /// </summary>
        /// <param name="sender">Element that fired the event</param>
        /// <param name="args">The received message</param>
        private async void MidiInputDevice_MessageReceived(MidiInPort sender, MidiMessageReceivedEventArgs args)
        {
            IMidiMessage receivedMidiMessage = args.Message;

            // Build the received MIDI message into a readable format
            StringBuilder outputMessage = new StringBuilder();
            outputMessage.Append(receivedMidiMessage.Timestamp.ToString()).Append(", Type: ").Append(receivedMidiMessage.Type);

            // Add MIDI message parameters to the output, depending on the type of message
            switch (receivedMidiMessage.Type)
            {
                case MidiMessageType.NoteOff:
                    var noteOffMessage = (MidiNoteOffMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(noteOffMessage.Channel).Append(", Note: ").Append(noteOffMessage.Note).Append(", Velocity: ").Append(noteOffMessage.Velocity);
                    break;
                case MidiMessageType.NoteOn:
                    var noteOnMessage = (MidiNoteOnMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(noteOnMessage.Channel).Append(", Note: ").Append(noteOnMessage.Note).Append(", Velocity: ").Append(noteOnMessage.Velocity);
                    break;
                case MidiMessageType.PolyphonicKeyPressure:
                    var polyphonicKeyPressureMessage = (MidiPolyphonicKeyPressureMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(polyphonicKeyPressureMessage.Channel).Append(", Note: ").Append(polyphonicKeyPressureMessage.Note).Append(", Pressure: ").Append(polyphonicKeyPressureMessage.Pressure);
                    break;
                case MidiMessageType.ControlChange:
                    var controlChangeMessage = (MidiControlChangeMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(controlChangeMessage.Channel).Append(", Controller: ").Append(controlChangeMessage.Controller).Append(", Value: ").Append(controlChangeMessage.ControlValue);
                    break;
                case MidiMessageType.ProgramChange:
                    var programChangeMessage = (MidiProgramChangeMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(programChangeMessage.Channel).Append(", Program: ").Append(programChangeMessage.Program);
                    break;
                case MidiMessageType.ChannelPressure:
                    var channelPressureMessage = (MidiChannelPressureMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(channelPressureMessage.Channel).Append(", Pressure: ").Append(channelPressureMessage.Pressure);
                    break;
                case MidiMessageType.PitchBendChange:
                    var pitchBendChangeMessage = (MidiPitchBendChangeMessage)receivedMidiMessage;
                    outputMessage.Append(", Channel: ").Append(pitchBendChangeMessage.Channel).Append(", Bend: ").Append(pitchBendChangeMessage.Bend);
                    break;
                case MidiMessageType.SystemExclusive:
                    var systemExclusiveMessage = (MidiSystemExclusiveMessage)receivedMidiMessage;
                    outputMessage.Append(", ");

                    // Read the SysEx bufffer
                    var sysExDataReader = DataReader.FromBuffer(systemExclusiveMessage.RawData);
                    while (sysExDataReader.UnconsumedBufferLength > 0)
                    {
                        byte byteRead = sysExDataReader.ReadByte();
                        // Pad with leading zero if necessary
                        outputMessage.Append(byteRead.ToString("X2")).Append(" ");
                    }
                    break;
                case MidiMessageType.MidiTimeCode:
                    var timeCodeMessage = (MidiTimeCodeMessage)receivedMidiMessage;
                    outputMessage.Append(", FrameType: ").Append(timeCodeMessage.FrameType).Append(", Values: ").Append(timeCodeMessage.Values);
                    break;
                case MidiMessageType.SongPositionPointer:
                    var songPositionPointerMessage = (MidiSongPositionPointerMessage)receivedMidiMessage;
                    outputMessage.Append(", Beats: ").Append(songPositionPointerMessage.Beats);
                    break;
                case MidiMessageType.SongSelect:
                    var songSelectMessage = (MidiSongSelectMessage)receivedMidiMessage;
                    outputMessage.Append(", Song: ").Append(songSelectMessage.Song);
                    break;
                case MidiMessageType.TuneRequest:
                    var tuneRequestMessage = (MidiTuneRequestMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.TimingClock:
                    var timingClockMessage = (MidiTimingClockMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.Start:
                    var startMessage = (MidiStartMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.Continue:
                    var continueMessage = (MidiContinueMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.Stop:
                    var stopMessage = (MidiStopMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.ActiveSensing:
                    var activeSensingMessage = (MidiActiveSensingMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.SystemReset:
                    var systemResetMessage = (MidiSystemResetMessage)receivedMidiMessage;
                    break;
                case MidiMessageType.None:
                    throw new InvalidOperationException();
                default:
                    break;
            }

            // Use the Dispatcher to update the messages on the UI thread
            await Dispatcher.RunAsync(CoreDispatcherPriority.High, () =>
            {
                // Skip TimingClock and ActiveSensing messages to avoid overcrowding the list. Commment this check out to see all messages
                if ((receivedMidiMessage.Type != MidiMessageType.TimingClock) && (receivedMidiMessage.Type != MidiMessageType.ActiveSensing))
                {
                    this.inputDeviceMessages.Items.Add(outputMessage + "\n");
                    this.inputDeviceMessages.ScrollIntoView(this.inputDeviceMessages.Items[this.inputDeviceMessages.Items.Count - 1]);
                    this.rootPage.NotifyUser("Message received successfully!", NotifyType.StatusMessage);
                }
            });
        }
    }
}
