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
#include "pch.h"
#include "TripVoiceCommand.h"

using namespace AdventureWorks;
using namespace Platform;

/// <summary> Utility class for passing voice command information around internally </summary>
TripVoiceCommand::TripVoiceCommand(
    String^ voiceCommandName,
    String^ commandMode,
    String^ textSpoken,
    String^ destination)
{
    this->voiceCommandName = voiceCommandName;
    this->commandMode = commandMode;
    this->textSpoken = textSpoken;
    this->destination = destination;
}

Platform::String^ TripVoiceCommand::VoiceCommandName::get()
{
    return this->voiceCommandName;
}

Platform::String^ TripVoiceCommand::CommandMode::get()
{
    return this->commandMode;
}

Platform::String^ TripVoiceCommand::TextSpoken::get()
{
    return this->textSpoken;
}

Platform::String^ TripVoiceCommand::Destination::get()
{
    return this->destination;
}