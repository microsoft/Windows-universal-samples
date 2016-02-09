using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media.Imaging;
using System.Diagnostics;
using Windows.Storage.Search;
using Windows.Storage.FileProperties;
using Windows.UI.Core;
using System.Threading;

namespace DataVirtualizationSample
{
    /// <summary>
    /// Represents a file and its properties
    /// </summary>
    class FileItem
    {
        public string Filename { get; set; }
        public int Size { get; set; }
        public BitmapImage ImageData { get; set; }

        public string Key { get; private set; }

        // Needed to ensure only one request is in progress at once
        private static SemaphoreSlim gettingFileProperties = new SemaphoreSlim(1);

        // Fetches all the data for the specified file
        public async static Task<FileItem> fromStorageFile(StorageFile f, CancellationToken ct)
        {
            FileItem item = new FileItem();
            item.Filename = f.DisplayName;
            
            // Block to make sure we only have one request outstanding
            await gettingFileProperties.WaitAsync();

            BasicProperties bp = null;
            try
            {
                bp = await f.GetBasicPropertiesAsync().AsTask(ct);
            }
            catch (Exception) { }
            finally
            {
                gettingFileProperties.Release();
            }

            ct.ThrowIfCancellationRequested();

            item.Size = (int)bp.Size;
            item.Key = f.FolderRelativeId;

            StorageItemThumbnail thumb = await f.GetThumbnailAsync(ThumbnailMode.SingleItem).AsTask(ct);
            ct.ThrowIfCancellationRequested();
            BitmapImage img = new BitmapImage();
            await img.SetSourceAsync(thumb).AsTask(ct);

            item.ImageData = img;
            return item;
        }
    }
}
