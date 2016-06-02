using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Storage;

namespace BackgroundMediaPlayback.Services
{
    class SettingsService
    {
        static SettingsService instance;

        public static SettingsService Instance
        {
            get
            {
                if (instance == null)
                    instance = new SettingsService();

                return instance;
            }
        }

        const string ToastOnAppEventsKey = "toast-on-app-events";
        const string UseCustomControlsKey = "use-custom-controls";
        IPropertySet settings = ApplicationData.Current.RoamingSettings.Values;

        public event EventHandler UseCustomControlsChanged;

        public bool ToastOnAppEvents
        {
            get
            {
                object setting;
                if (settings.TryGetValue(ToastOnAppEventsKey, out setting))
                    return (bool)setting;

                return true;
            }
            set
            {
                settings[ToastOnAppEventsKey] = value;
            }
        }

        public bool UseCustomControls
        {
            get
            {
                object setting;
                if (settings.TryGetValue(UseCustomControlsKey, out setting))
                    return (bool)setting;

                return false;
            }
            set
            {
                if (UseCustomControls != value) {
                    settings[UseCustomControlsKey] = value;
                    UseCustomControlsChanged?.Invoke(this, EventArgs.Empty);
                }
            }
        }
    }
}
