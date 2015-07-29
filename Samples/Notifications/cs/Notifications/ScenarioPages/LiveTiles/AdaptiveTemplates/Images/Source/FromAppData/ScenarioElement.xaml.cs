using Notifications.Helpers;
using System;
using Windows.Data.Xml.Dom;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.LiveTiles.AdaptiveTemplates.Images.Source.FromAppData
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();
        }

        private Uri _imagePath;

        private async void ButtonPickImage_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker picker = new FileOpenPicker();
            picker.ViewMode = PickerViewMode.Thumbnail;
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.CommitButtonText = "Open Image";
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".jpeg");
            picker.FileTypeFilter.Add(".png");

            StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                StorageFolder logosFolder = await ApplicationData.Current.LocalFolder.CreateFolderAsync("TileNotifications", CreationCollisionOption.OpenIfExists);

                // Copy the file to local app data
                StorageFile localFile = await file.CopyAsync(logosFolder, file.Name, NameCollisionOption.GenerateUniqueName);

                _imagePath = new Uri("ms-appdata:///Local/TileNotifications/" + localFile.Name);

                ButtonPinTile.IsEnabled = true;
                ImageLogoPreview.Source = new BitmapImage(_imagePath);
            }
        }

        private async void ButtonPinTile_Click(object sender, RoutedEventArgs e)
        {
            base.IsEnabled = false;

            string ImageUrl = _imagePath.ToString();

            SecondaryTile tile = TilesHelper.GenerateSecondaryTile("App Package Image");
            await tile.RequestCreateAsync();

            string tileXmlString =
                "<tile>"
                + "<visual>"
                + "<binding template='TileSmall'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileMedium'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileWide'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "<binding template='TileLarge'>"
                + "<image src='" + ImageUrl + "' alt='image'/>"
                + "</binding>"
                + "</visual>"
                + "</tile>";

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(tileXmlString);
            TileNotification notifyTile = new TileNotification(xmlDoc);
            TileUpdateManager.CreateTileUpdaterForSecondaryTile(tile.TileId).Update(notifyTile);

            base.IsEnabled = true;
        }
    }
}
