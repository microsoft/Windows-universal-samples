using System;
using Windows.UI.Popups;
using Windows.UI.Xaml;

namespace xBindSampleModel
{
    internal class Employee : IEmployee
    {
        private string _FirstName;
        private string _LastName;
        private Manager _Manager;

        internal Employee()
        {
            Visibility = true;
        }

        public void Poke(object sender, RoutedEventArgs e)
        {
            MessageDialog dlg = new MessageDialog(Name + " has been poked.");
            var t = dlg.ShowAsync();
        }

        public bool Visibility { get; set; }

        public string NullStringProperty
        {
            get { return null; }
            set { }
        }
        public Windows.UI.Xaml.Media.ImageSource NullImageSource
        {
            get { return null; }
            set { }
        }

        public string FirstName
        {
            get { return _FirstName; }
            set
            {
                if (value != _FirstName)
                {
                    _FirstName = value;
                }
            }
        }

        public string LastName
        {
            get { return _LastName; }
            set
            {
                if (value != _LastName)
                {
                    _LastName = value;
                }
            }
        }

        public Manager DirectManager
        {
            get { return _Manager; }
            set
            {
                if (value != _Manager)
                {
                    _Manager = value;
                }
            }
        }

        public bool IsManager { get { return false; } }

        public String Title { get { return "Developer"; } }

        public string Name { get { return FirstName + " " + LastName; } }

        public string FullName() { return FirstName + " " + LastName; }
    }
}
