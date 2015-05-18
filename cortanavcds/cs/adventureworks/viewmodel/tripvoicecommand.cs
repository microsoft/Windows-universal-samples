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
using System.Text;
using System.Threading.Tasks;

namespace AdventureWorks.ViewModel
{
    /// <summary>
    /// Simple struct to pass voice-command arguments to the appropriate view, in order to execute the 
    /// action requested by the user. Reference OnActivated() in App.xaml.cs, and AdventureWorksCommands.xml
    /// for context on what should go in here.
    /// </summary>
    public struct TripVoiceCommand
    {
        public string voiceCommand;
        public string commandMode;
        public string textSpoken;
        public string destination;

        /// <summary>
        /// Set up the voice command struct with the provided details about the voice command.
        /// Oriented around the "showTripToDestination" VCD command (See AdventureWorksCommands.xml)
        /// </summary>
        /// <param name="voiceCommand">The voice command (the Command element in the VCD xml) </param>
        /// <param name="commandMode">The command mode (whether it was voice or text activation)</param>
        /// <param name="textSpoken">The raw voice command text.</param>
        /// <param name="destination">The destination parameter.</param>
        public TripVoiceCommand(string voiceCommand, string commandMode, string textSpoken, string destination)
        {
            this.voiceCommand = voiceCommand;
            this.commandMode = commandMode;
            this.textSpoken = textSpoken;
            this.destination = destination;
        }
    }
}
