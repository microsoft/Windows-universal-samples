namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class DeviceListEntry sealed
    {

        Windows::Devices::Enumeration::DeviceInformation^ m_Device;

    public:

        property Platform::String^ InstanceId 
        {
            Platform::String^ get(void) 
            {
                return safe_cast<Platform::String^>(m_Device->Properties->Lookup("System.Devices.DeviceInstanceId"));
            }
        }

        property Platform::String^ Id 
        {
            Platform::String^ get(void) 
            {
                return m_Device->Id;
            }
        }

        property bool Matched;

        DeviceListEntry(Windows::Devices::Enumeration::DeviceInformation^ DeviceInterface) 
        {
            m_Device = DeviceInterface;
            Matched = true;
        }
    };
        
    typedef Windows::Foundation::EventHandler<Windows::Devices::Custom::CustomDevice^> DeviceClosingHandler;

    ref class DeviceList
    {
    public:
        static property DeviceList^ Current {DeviceList^ get();};

        property Windows::Foundation::Collections::IObservableVector<DeviceListEntry^>^ Fx2Devices 
        {
            Windows::Foundation::Collections::IObservableVector<DeviceListEntry^>^ get(void)
            {
                return m_List;
            }
        };

        Windows::Devices::Custom::CustomDevice^ GetSelectedDevice()
        {
            return m_Fx2Device;
        }

        Platform::String^ GetSelectedDeviceId()
        {
            return m_Fx2DeviceId;
        }

        void SetSelectedDevice(Platform::String^ id, Windows::Devices::Custom::CustomDevice^ device)
        {
            if (m_Fx2Device != nullptr)
            {
                DeviceClosing(this, m_Fx2Device);
            }

            m_Fx2Device = device;
            m_Fx2DeviceId = id;
        }

        void ClearSelectedDevice()
        {
            SetSelectedDevice(nullptr, nullptr);
        }

        property bool WatcherStarted 
        {
            bool get(void) 
            {
                return m_WatcherStarted;
            }
            void set(bool value) 
            {
                m_WatcherStarted = value;
            }
        };

        void StartFx2Watcher();

        void StopFx2Watcher();

        static void CreateBooleanTable(
            Windows::UI::Xaml::Documents::InlineCollection^ Table,
            const Platform::Array<bool>^ NewValues,
            const Platform::Array<bool>^ OldValues,
            Platform::String^ IndexTitle,
            Platform::String^ ValueTitle,
            Platform::String^ TrueValue,
            Platform::String^ FalseValue
            );

        event DeviceClosingHandler^ DeviceClosing;

    private:
        static DeviceList^ _Current;
        Windows::Devices::Enumeration::DeviceWatcher^ m_Fx2Watcher;
            
        Platform::Collections::Vector<DeviceListEntry^>^ m_List;
            
        Windows::Devices::Custom::CustomDevice^ m_Fx2Device;
        Platform::String^ m_Fx2DeviceId;

        bool m_WatcherSuspended;
        bool m_WatcherStarted;

        DeviceList();

        DeviceListEntry^ FindDevice(Platform::String^ Id);

        void InitDeviceWatcher();

        void SuspendDeviceWatcher(Object^ Sender, Windows::ApplicationModel::SuspendingEventArgs^ Args);
        void ResumeDeviceWatcher(Object^ Sender, Object^ Args);

        void OnFx2Added(Windows::Devices::Enumeration::DeviceWatcher^ Sender, Windows::Devices::Enumeration::DeviceInformation^ DevInformation);
        void OnFx2Removed(Windows::Devices::Enumeration::DeviceWatcher^ Sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ DevInformation);
        void OnFx2EnumerationComplete(Windows::Devices::Enumeration::DeviceWatcher^ Sender, Object^ Args);
    };
}
