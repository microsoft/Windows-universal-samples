using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OneHandedUse_TailoredMultipleViews.ViewModel
{
    public class MainViewModel
    {
        private ObservableCollection<Model> _albumItems, _collectionItems;
        public ObservableCollection<Model> AlbumItems
        {
            get
            {
                return _albumItems;
            }

            set
            {
                _albumItems = value;
            }
        }
        public ObservableCollection<Model> CollectionItems
        {
            get
            {
                return _collectionItems;
            }

            set
            {
                _collectionItems = value;
            }
        }
        public ObservableCollection<Model> GetAlbums()
        {
            AlbumItems = new ObservableCollection<Model>();
            AlbumItems.Clear();
            for (int i = 1; i < 13; i++)
            {
                AlbumItems.Add(new Model { ImageUri = new Uri("ms-appx:///Images/LandscapeImage" + i.ToString() + ".jpg", UriKind.Absolute) });
            }
            return AlbumItems;
        }

        public ObservableCollection<Model> GetCollections()
        {
            CollectionItems = new ObservableCollection<Model>();
            CollectionItems.Clear();
            for (int i = 13; i <= 25; i++)
            {
                CollectionItems.Add(new Model { ImageUri = new Uri("ms-appx:///Images/LandscapeImage" + i.ToString() + ".jpg", UriKind.Absolute) });
            }
            return CollectionItems;
        }
    }
}
