using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Import;
using Windows.Storage;
using Windows.UI.Xaml.Media.Imaging;

namespace ImportSample
{
    /// <summary>
    /// Wrapper for the thumbnail and the ImportableItem
    /// </summary>
    class ImportableItemWrapper : INotifyPropertyChanged
    {
        private BitmapImage thumbnail;
        public BitmapImage Thumbnail
        {
            get
            {
                return thumbnail;
            }
            private set
            {
                if (thumbnail != value)
                {
                    thumbnail = value;
                    NotifyProperty("Thumbnail");
                }
            }
        }

        public PhotoImportItem ImportableItem
        {
            get;
            private set;
        }

        public ImportableItemWrapper(PhotoImportItem importableItem)
        {
            ImportableItem = importableItem;
            LoadThumbnail();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyProperty(string propertyName)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public async void LoadThumbnail()
        {
           var thumbnailResult = await ImportableItem.Thumbnail.OpenReadAsync();
                
           // if there is contention accessing the device the thumbnail will be null. 
           if (thumbnailResult != null)
           {
                var thumb = new BitmapImage();
                thumb.SetSource(thumbnailResult);
                this.Thumbnail = thumb;
           }           
        }
    }
}
