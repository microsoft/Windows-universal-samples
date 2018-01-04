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
using System.Numerics;
using System.Threading.Tasks;
using Windows.ApplicationModel.Preview.Holographic;
using Windows.Perception.Spatial;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CreateTile : Page
    {
        MainPage rootPage = MainPage.Current;
        string glbName = string.Empty;
        Uri glbUri;

        string lastFileName = string.Empty;
        Uri lastFileUri;

        public Scenario1_CreateTile()
        {
            this.InitializeComponent();
            GetFromPackage();
        }

        void UpdateSourceRun()
        {
            if (GlbSourceRun != null)
            {
                GlbSourceRun.Text = glbUri == null ? String.Empty : glbUri.OriginalString;
            }
        }

        void GetFromPackage()
        {
            glbUri = new Uri("ms-appx:///glb/microsoft-logo.glb");
            glbName = "Microsoft logo";
            UpdateSourceRun();
        }

        void GetFromFile()
        {
            glbUri = lastFileUri;
            glbName = lastFileName;
            UpdateSourceRun();
        }

        async void PickFile()
        {
            var picker = new FileOpenPicker();
            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.SuggestedStartLocation = PickerLocationId.Objects3D;
            picker.FileTypeFilter.Add(".glb");

            var file = await picker.PickSingleFileAsync();
            if (file == null)
            {
                return;
            }

            // ms-appdata content must be of the form ms-appdata:///local/...
            // Copy the picked file into our local folder.

            const string SpecialFolder = "tilecontent";
            try
            {
                StorageFolder folder = await ApplicationData.Current.LocalFolder.CreateFolderAsync(SpecialFolder,
                    CreationCollisionOption.OpenIfExists);
                file = await file.CopyAsync(folder, file.Name, NameCollisionOption.ReplaceExisting);
            }
            catch
            {
                // File I/O errors are reported as exceptions.
                rootPage.NotifyUser("Could not copy file to local storage.", NotifyType.ErrorMessage);
                return;
            }

            // Escape the file name in case it contains special characters.
            lastFileUri = new Uri($"ms-appdata:///local/{SpecialFolder}/{Uri.EscapeDataString(file.Name)}");
            lastFileName = file.DisplayName;
            glbUri = lastFileUri;
            glbName = lastFileName;
            UpdateSourceRun();
        }

        string GenerateTileId()
        {
            // Tile IDs consist of up to 64 alphanumerics, underscore, and period.
            // Create a guid to use as a unique tile id. Use "N" format so it consists
            // solely of hex digits with no extra punctuation.
            Guid guid = Guid.NewGuid();
            return guid.ToString("N");
        }

        float ParseFloatOrDefault(string input)
        {
            float parsed = 0.0f;
            float.TryParse(input, out parsed);
            return parsed;
        }

        async void CreateSecondaryTile()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            if (glbUri == null)
            {
                rootPage.NotifyUser("Please pick content to be shown.", NotifyType.ErrorMessage);
                return;
            }

            // Create a guid to use as a unique tile id
            string tileId = GenerateTileId();

            var logoUri = new Uri("ms-appx:///Assets/squareTile-sdk.png");
            var tile = new SecondaryTile(
                    tileId, // TileId
                    glbName, // Display Name
                    tileId, // Arguments
                    logoUri, // Square150x150Logo
                    TileSize.Square150x150); // DesiredSize

            // Set the mixed reality model properties.
            TileMixedRealityModel model = tile.VisualElements.MixedRealityModel;

            model.Uri = glbUri;

            if (BoundingBoxCheckBox.IsChecked.Value)
            {
                var boundingBox = new SpatialBoundingBox();
                boundingBox.Center = new Vector3(
                    ParseFloatOrDefault(BoundingBoxCenterXTextBox.Text),
                    ParseFloatOrDefault(BoundingBoxCenterYTextBox.Text),
                    ParseFloatOrDefault(BoundingBoxCenterZTextBox.Text));
                boundingBox.Extents = new Vector3(
                    ParseFloatOrDefault(BoundingBoxExtentsXTextBox.Text),
                    ParseFloatOrDefault(BoundingBoxExtentsYTextBox.Text),
                    ParseFloatOrDefault(BoundingBoxExtentsZTextBox.Text));
                model.BoundingBox = boundingBox;
            }

            // Create the tile
            bool created = await tile.RequestCreateAsync();
            if (created)
            {
                // Warn the user that they need to be in the headset for anything interesting to happen.
                if (!HolographicApplicationPreview.IsCurrentViewPresentedOnHolographicDisplay())
                {
                    rootPage.NotifyUser("3D content can be created only while the view is being displayed in a Mixed Reality headset.", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Could not create secondary tile.", NotifyType.ErrorMessage);
            }
        }
    }
}
