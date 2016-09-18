using BackgroundMediaPlayback.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Xaml;

namespace BackgroundMediaPlayback.ViewModels
{
    public class SettingsViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public bool ToastOnAppEvents
        {
            get
            {
                return SettingsService.Instance.ToastOnAppEvents;
            }
            set
            {
                if (SettingsService.Instance.ToastOnAppEvents != value)
                {
                    SettingsService.Instance.ToastOnAppEvents = value;
                    RaisePropertyChanged("ToastOnAppEvents");
                }
            }
        }

        public bool UseCustomControls
        {
            get
            {
                return SettingsService.Instance.UseCustomControls;
            }
            set
            {
                if (SettingsService.Instance.UseCustomControls != value)
                {
                    SettingsService.Instance.UseCustomControls = value;
                    RaisePropertyChanged("UseCustomControls");
                }
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
