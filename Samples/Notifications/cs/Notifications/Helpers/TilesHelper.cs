using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;

namespace Notifications.Helpers
{
    public class TilesHelper
    {
        public static async Task<SecondaryTile> PinNewSecondaryTile()
        {
            SecondaryTile tile = GenerateSecondaryTile("Secondary tile");

            await tile.RequestCreateAsync();

            return tile;
        }

        public static SecondaryTile GenerateSecondaryTile(string tileId, string displayName)
        {
            SecondaryTile tile = new SecondaryTile(tileId, displayName, "args", new Uri("ms-appx:///Assets/Logo.png"), TileSize.Default);
            tile.VisualElements.Square71x71Logo = new Uri("ms-appx:///Assets/Small.png");
            tile.VisualElements.Wide310x150Logo = new Uri("ms-appx:///Assets/WideLogo.png");
            tile.VisualElements.Square310x310Logo = new Uri("ms-appx:///Assets/LargeLogo.png");
            tile.VisualElements.Square44x44Logo = new Uri("ms-appx:///Assets/SmallLogo.png"); // Branding logo

            return tile;
        }

        public static SecondaryTile GenerateSecondaryTile(string displayName)
        {
            return GenerateSecondaryTile(DateTime.Now.Ticks.ToString(), displayName);
        }

        public static async Task<SecondaryTile> FindExisting(string tileId)
        {
            return (await SecondaryTile.FindAllAsync()).FirstOrDefault(i => i.TileId.Equals(tileId));
        }

        public static async Task<SecondaryTile> PinNewSecondaryTile(string displayName, string xml)
        {
            SecondaryTile tile = GenerateSecondaryTile(displayName);
            await tile.RequestCreateAsync();

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);

            TileUpdateManager.CreateTileUpdaterForSecondaryTile(tile.TileId).Update(new TileNotification(doc));

            return tile;
        }

        public static async Task UpdateTiles(string xml)
        {
            XmlDocument doc;

            try
            {
                doc = new XmlDocument();
                doc.LoadXml(xml);
            }

            catch (Exception ex)
            {
                await new MessageDialog(ex.ToString(), "ERROR: Invalid XML").ShowAsync();
                return;
            }

            await UpdateTiles(doc);
        }

        public static async Task UpdateTiles(XmlDocument doc)
        {
            try
            {
                TileUpdateManager.CreateTileUpdaterForApplication().Update(new TileNotification(doc));
                
                await UpdateTile("Small", doc);
                await UpdateTile("Medium", doc);
                await UpdateTile("Wide", doc);
                await UpdateTile("Large", doc);
            }

            catch (Exception ex)
            {
                await new MessageDialog(ex.ToString(), "ERROR: Failed updating tiles").ShowAsync();
            }
        }

        public static async Task UpdateTile(string tileId, XmlDocument doc)
        {
            if (!SecondaryTile.Exists(tileId))
            {
                SecondaryTile tile = GenerateSecondaryTile(tileId, tileId);
                tile.VisualElements.ShowNameOnSquare310x310Logo = true;
                await tile.RequestCreateAsync();
            }
                //await GenerateSecondaryTile(tileId, tileId).RequestCreateAsync();

            TileUpdateManager.CreateTileUpdaterForSecondaryTile(tileId).Update(new TileNotification(doc));
        }
    }
}
