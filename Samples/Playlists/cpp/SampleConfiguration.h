// Copyright (c) Microsoft. All rights reserved.

#pragma once 
#include "pch.h"

namespace SDKTemplate
{
    value struct Scenario;

    partial ref class MainPage
    {
    internal:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return "Playlists";
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        static property Platform::Array<Platform::String^>^ audioExtensions
        {
            Platform::Array<Platform::String^>^ get()
            {
                return audioExtensionsInner;
            }
        }
        static property Platform::Array<Platform::String^>^ playlistExtensions
        {
            Platform::Array<Platform::String^>^ get()
            {
                return playlistExtensionsInner;
            }
        }
        static Windows::Storage::Pickers::FileOpenPicker^ CreateFilePicker(Platform::Array<Platform::String^>^ extensions);
        concurrency::task<Windows::Media::Playlists::Playlist^> PickPlaylistAsync();
        concurrency::task<bool> TrySavePlaylistAsync(Windows::Media::Playlists::Playlist^ playlist);

    private:
        static Platform::Array<Scenario>^ scenariosInner;
        static Platform::Array<Platform::String^>^ audioExtensionsInner;
        static Platform::Array<Platform::String^>^ playlistExtensionsInner;
    };

    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };
}
