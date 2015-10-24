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
using System.Threading.Tasks;
using Windows.Media.Playlists;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Playlists";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Create a playlist", ClassType = typeof(Scenario1_Create) }, 
            new Scenario() { Title = "Display a playlist", ClassType = typeof(Scenario2_Display) }, 
            new Scenario() { Title = "Add items to a playlist", ClassType = typeof(Scenario3_Add) }, 
            new Scenario() { Title = "Remove an item from a playlist", ClassType = typeof(Scenario4_Remove) }, 
            new Scenario() { Title = "Clear a playlist", ClassType = typeof(Scenario5_Clear) } 
        };

        /// <summary> 
        /// Audio file extensions. 
        /// </summary> 
        public static readonly string[] audioExtensions = new string[] { ".wma", ".mp3", ".mp2", ".aac", ".adt", ".adts", ".m4a" }; 
 
        /// <summary> 
        /// Playlist file extensions. 
        /// </summary> 
        public static readonly string[] playlistExtensions = new string[] { ".m3u", ".wpl", ".zpl" }; 
 
        /// <summary> 
        /// Creates a FileOpenPicker for the specified extensions.  
        /// </summary> 
        /// <param name="extensions">File extensions to pick.</param> 
        /// <returns>FileOpenPicker instance.</returns> 
        public static FileOpenPicker CreateFilePicker(string[] extensions) 
        { 
            FileOpenPicker picker = new FileOpenPicker(); 
            picker.SuggestedStartLocation = PickerLocationId.MusicLibrary; 
 
            foreach (string extension in extensions) 
            { 
                picker.FileTypeFilter.Add(extension); 
            } 
 
            return picker; 
        } 
 
        /// <summary> 
        /// Picks and loads a playlist. 
        /// </summary> 
        async public Task<Playlist> PickPlaylistAsync() 
        { 
            FileOpenPicker picker = CreateFilePicker(MainPage.playlistExtensions); 
            StorageFile file = await picker.PickSingleFileAsync(); 
 
            if (file == null)
            {
                NotifyUser("No playlist picked.", NotifyType.ErrorMessage);
                return null;
            }

            try
            {
                return await Playlist.LoadAsync(file);
            }
            catch (Exception ex)
            {
                NotifyUser(ex.Message, NotifyType.ErrorMessage);
                return null;
            }
        }

        async public Task<bool> TrySavePlaylistAsync(Playlist playlist)
        {
            try
            {
                await playlist.SaveAsync();
                return true;
            }
            catch (Exception ex)
            {
                NotifyUser(ex.Message, NotifyType.ErrorMessage);
                return false;
            }
        }
    } 

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
