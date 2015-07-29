using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StateTriggersSample.ViewModel
{
    public class MainViewModel
    {
        private ObservableCollection<Model> _items;
        public ObservableCollection<Model> Items
        {
            get
            {
                return _items;
            }

            set
            {
                _items = value;
            }
        }
        public MainViewModel()
        {
            Items = new ObservableCollection<Model>();
            Items.Clear();
            Items.Add(new Model { Name = "Item1", Description = "First item that is plain and simple", IsColorEnabled = false });
            Items.Add(new Model { Name = "Item2", Description = "Second item uses a ViewModel property in a custom trigger for color variation", IsColorEnabled = true });
            Items.Add(new Model { Name = "Item3", Description = "Third item without color in ViewModel property, but has a lot of text that increases its size. This larger size driven by the content drives changes to the colors in XAML using a custom ControlSizeTrigger", IsColorEnabled = false });
        }
    }
}
