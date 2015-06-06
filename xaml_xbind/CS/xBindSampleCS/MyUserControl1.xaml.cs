using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace xBindSampleCS
{
    public sealed partial class MyUserControl1 : UserControl, INotifyPropertyChanged
    {
        public MyUserControl1()
        {
            NonDPProperty = "Not Set";
            this.InitializeComponent();
        }

        private string _nonDPProperty;
        public string NonDPProperty {
            get { return _nonDPProperty; }
            set
            {
                if (value != _nonDPProperty)
                {
                    _nonDPProperty = value;
                    if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs("NonDPProperty"));
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
