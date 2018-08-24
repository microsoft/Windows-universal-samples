#pragma once

#include "RpcClientApi.h"
#include <memory>

namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class ServiceViewModel sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        void StartMetering(int sampleRate);
        void SetSamplePeriod(int rate);
        void StopMetering();
        static property ServiceViewModel^ Current {ServiceViewModel^ get(); };
        property Platform::String^ ExpectedRpcCallbackRate 
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }
        property Platform::String^ ActualRpcCallbackRate 
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }
        property Platform::String^ SampleMessage 
        {
            Platform::String^ get();
            void set(Platform::String^ value);
        }
        property bool StartButtonEnabled 
        {
            bool get();
            void set(bool value);
        }
        property bool StopButtonEnabled 
        {
            bool get();
            void set(bool value);
        }
        property bool SliderEnabled 
        {
            bool get();
            void set(bool value);
        }
        property double SliderValue 
        {
            double get();
            void set(double value);
        }
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    private:
        ServiceViewModel();
        static ServiceViewModel^ _Current;
        volatile bool meteringOn;
        bool meteringOnWhileSuspension;
        Windows::UI::Core::ICoreDispatcher^ dispatcher;
        int samplePeriod;
        ULARGE_INTEGER lastUpdateTime;
        __int64 rpcDataCountOld = 0;
        unsigned int sampleRefreshCount = 0;
        RPC_CLIENT_HANDLE rpcclient;
        bool stopMeteringRequested;
        bool NotifyIfAnyError(__int64 errCode);

        // Variables backing UI bindings
        Platform::String^ sampleMessage;
        Platform::String^ expectedRpcCallbackRate;
        Platform::String^ actualRpcCallbackRate;
        bool startButtonEnabled;
        bool stopButtonEnabled;
        bool sliderEnabled;
        double sliderValue;

        void NotifyPropertyChanged(Platform::String^ prop);
        void ServiceViewModel::SuspendMeteringService(Object^, Windows::ApplicationModel::SuspendingEventArgs^);
        void ServiceViewModel::ResumeMeteringService(Object^, Object^);

        Platform::Array<__int64>^ sampleArray;
    };
}
