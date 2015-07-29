using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PivotCS
{
    public class SampleDataModel
    {
        public string Title { get; private set; }
        public string ImagePath { get; private set; }
        public bool IsNew { get; private set; }
        public bool IsFlagged { get; private set; }

        public SampleDataModel(string title, string imagePath, bool isNew = false, bool isFlagged = false)
        {
            this.Title = title;
            this.ImagePath = imagePath;
            this.IsNew = isNew;
            this.IsFlagged = isFlagged;
        }

        public override string ToString()
        {
            return this.Title;
        }

        static public ObservableCollection<SampleDataModel> GetSampleData()
        {
            var MyCollection = new ObservableCollection<SampleDataModel>();
            MyCollection.Add(new SampleDataModel("Cliff", "ms-appx:///Assets/cliff.jpg"));
            MyCollection.Add(new SampleDataModel("Grapes", "ms-appx:///Assets/grapes.jpg"));
            MyCollection.Add(new SampleDataModel("Rainier", "ms-appx:///Assets/Rainier.jpg", true));
            MyCollection.Add(new SampleDataModel("Sunset", "ms-appx:///Assets/Sunset.jpg", true, true));
            MyCollection.Add(new SampleDataModel("Treetops", "ms-appx:///Assets/Treetops.jpg", true));
            MyCollection.Add(new SampleDataModel("Valley", "ms-appx:///Assets/Valley.jpg", false, true));
            return MyCollection;
        }
    }
}
