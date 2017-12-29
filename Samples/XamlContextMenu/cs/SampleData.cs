using System;
using System.Collections.Generic;

namespace SDKTemplate
{
    public class SampleDataModel
    {
        public string Title { get; private set; }
        public string ImagePath { get; private set; }

        public SampleDataModel(string title, string imagePath)
        {
            this.Title = title;
            this.ImagePath = imagePath;
        }

        // Not used in sample but makes debugging easier.
        public override string ToString() => this.Title;

        static public List<SampleDataModel> GetSampleData()
        {
            return new List<SampleDataModel>() {
                new SampleDataModel("Cliff", "Assets/cliff.jpg"),
                new SampleDataModel("Grapes", "Assets/grapes.jpg"),
                new SampleDataModel("Rainier", "Assets/Rainier.jpg"),
                new SampleDataModel("Sunset", "Assets/Sunset.jpg"),
                new SampleDataModel("Treetops", "Assets/Treetops.jpg"),
                new SampleDataModel("Valley", "Assets/Valley.jpg"),
            };
        }
    }
}
