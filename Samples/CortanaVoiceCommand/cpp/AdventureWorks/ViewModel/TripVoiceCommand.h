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
#pragma once

namespace AdventureWorks
{
    ref class TripVoiceCommand sealed
    {
    public:
        TripVoiceCommand(
            Platform::String^ voiceCommandName,
            Platform::String^ commandMode,
            Platform::String^ textSpoken,
            Platform::String^ destination);

        property Platform::String^ VoiceCommandName
        {
            Platform::String^ get();
        }

        property Platform::String^ CommandMode
        {
            Platform::String^ get();
        }

        property Platform::String^ TextSpoken
        {
            Platform::String^ get();
        }

        property Platform::String^ Destination
        {
            Platform::String^ get();
        }

    private:
        Platform::String^ voiceCommandName;
        Platform::String^ commandMode;
        Platform::String^ textSpoken;
        Platform::String^ destination;
    };
}

