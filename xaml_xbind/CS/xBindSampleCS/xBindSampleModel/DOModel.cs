using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;

namespace xBindSampleModel
{
    public sealed class DOModel : DependencyObject
    {
        public DOModel()
        {
            YangObj = new YangContainer(this);
        }

        public void UpdateValues()
        {
            this.IntPropertyDP++;
            YinPropertyDP = 65;
        }

        public YangContainer YangObj
        {
            get;
            set;
        }

        public int IntPropertyDP
        {
            get { return (int)GetValue(IntPropertyDPProperty); }
            set { SetValue(IntPropertyDPProperty, value); }
        }
        #region IntPropertyDP DP
        private const string IntPropertyDPName = "IntPropertyDP";
        private static readonly DependencyProperty _IntPropertyDPProperty =
            DependencyProperty.Register(IntPropertyDPName, typeof(int), typeof(DataModel), new PropertyMetadata(0));
        public static DependencyProperty IntPropertyDPProperty {  get { return _IntPropertyDPProperty; } }
        #endregion

        public double YinPropertyDP
        {
            get { return (double)GetValue(YinPropertyDPProperty); }
            set { SetValue(YinPropertyDPProperty, value); }
        }
        #region YinPropertyDP DP
        private const string YinPropertyDPName = "YinPropertyDP";
        private static readonly DependencyProperty _YinPropertyDPProperty =
            DependencyProperty.Register(YinPropertyDPName, typeof(double), typeof(DataModel), new PropertyMetadata(0, YinCallback));
        public static DependencyProperty YinPropertyDPProperty { get { return _YinPropertyDPProperty; } }
        #endregion


        public static void YinCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            DOModel inst = d as DOModel;
            double value = (double)e.NewValue;
            if (inst.YangObj.Value != value)
            {
                inst.YangObj.Value = value;
            }
        }
    }

    public sealed class YangContainer : INotifyPropertyChanged
    {
        private DOModel _parent;

        public YangContainer(DOModel parent)
        {
            _parent = parent;
        }

        private double _value;
        public double Value
        {
            get { return _value; }
            set
            {
                if (value != _value)
                {
                    _value = value;
                    NotifyPropertyChanged();
                    _parent.YinPropertyDP = value;
                }
            }
        }


        #region INPC

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        #endregion
    }
}
