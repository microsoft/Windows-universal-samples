#include "pch.h"
#include "ServiceViewModel.h"
#include "MainPage.xaml.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::System::Threading;
using namespace Windows::UI::Xaml::Data;

ServiceViewModel::ServiceViewModel()
{
    // Set default rate
    samplePeriod = 100;

    // Initialize UI state
    startButtonEnabled = true;
    stopButtonEnabled = false;
    sliderEnabled = true;
    sliderValue = samplePeriod;
    actualRpcCallbackRate = "0";
    expectedRpcCallbackRate = "0";

    meteringOn = false;

    // Get the dispatcher for notifying property change
    dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;

    sampleArray = ref new Platform::Array<__int64>(1000);
}

//
// if errCode represents an error, then display a message and return true.
//
inline bool ServiceViewModel::NotifyIfAnyError(__int64 errCode)
{
    if (errCode != 0)
    {
        meteringOn = false;
        NotifyStatusMessage("Error occured while communicating with RPC server: " + errCode.ToString(), NotifyType::ErrorMessage);

        // Reset button state
        StartButtonEnabled = true;
        StopButtonEnabled = false;
        return true;
    }
    return false;
}

// Bindable properties
#pragma region BindableProperties

String^ ServiceViewModel::ExpectedRpcCallbackRate::get() { return expectedRpcCallbackRate; }
void ServiceViewModel::ExpectedRpcCallbackRate::set(String^ value) 
{
    if (expectedRpcCallbackRate != value) 
    {
        expectedRpcCallbackRate = value;
        NotifyPropertyChanged("ExpectedRpcCallbackRate");
    }
}

String^ ServiceViewModel::ActualRpcCallbackRate::get() { return actualRpcCallbackRate; }
void ServiceViewModel::ActualRpcCallbackRate::set(String^ value) 
{
    if (actualRpcCallbackRate != value) 
    {
        actualRpcCallbackRate = value;
        NotifyPropertyChanged("ActualRpcCallbackRate");
    }
}

String^ ServiceViewModel::SampleMessage::get() { return sampleMessage; }
void ServiceViewModel::SampleMessage::set(String^ value) 
{
    if (sampleMessage != value) 
    {
        sampleMessage = value;
        NotifyPropertyChanged("SampleMessage");
    }
}

bool ServiceViewModel::StartButtonEnabled::get() { return startButtonEnabled; }
void ServiceViewModel::StartButtonEnabled::set(bool value) 
{
    if (startButtonEnabled != value) 
    {
        startButtonEnabled = value;
        NotifyPropertyChanged("StartButtonEnabled");
    }
}

bool ServiceViewModel::StopButtonEnabled::get() { return stopButtonEnabled; }
void ServiceViewModel::StopButtonEnabled::set(bool value) 
{
    if (stopButtonEnabled != value) 
    {
        stopButtonEnabled = value;
        NotifyPropertyChanged("StopButtonEnabled");
    }
}

bool ServiceViewModel::SliderEnabled::get() { return sliderEnabled; }
void ServiceViewModel::SliderEnabled::set(bool value) 
{
    if (sliderEnabled != value) 
    {
        sliderEnabled = value;
        NotifyPropertyChanged("SliderEnabled");
    }
}

double ServiceViewModel::SliderValue::get() { return sliderValue; }
void ServiceViewModel::SliderValue::set(double value) 
{
    if (sliderValue != value) 
    {
        sliderValue = value;
        NotifyPropertyChanged("SliderValue");
    }
}
#pragma endregion BindableProperties

void SDKTemplate::ServiceViewModel::NotifyPropertyChanged(Platform::String^ prop)
{
    if (dispatcher != nullptr)
    {
        dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([this, prop]()
        {
            PropertyChangedEventArgs^ args =
                ref new PropertyChangedEventArgs(prop);
            PropertyChanged(this, args);
        }));
    }
    // else log error
}

//
// Initializes rpcclient and metering
//
void ServiceViewModel::StartMetering(int sampleRate)
{
    create_task([this, sampleRate] 
    {
        SampleMessage = "";
        StartButtonEnabled = false;
        StopButtonEnabled = true;
        stopMeteringRequested = false;

        rpcclient = std::make_unique<RpcClient>();
        if (NotifyIfAnyError(rpcclient->Initialize())) return;

        meteringOn = true;
        sampleRefreshCount = 0;
        sampleArray[0] = 0;

        // Set the sample rate on server
        this->samplePeriod = sampleRate;
        __int64 retCode = rpcclient->SetSampleRate(sampleRate);

        FILETIME initialSystemTime;
        GetSystemTimeAsFileTime(&initialSystemTime);
        lastUpdateTime.LowPart = initialSystemTime.dwLowDateTime;
        lastUpdateTime.HighPart = initialSystemTime.dwHighDateTime;

        if (!NotifyIfAnyError(retCode))
        {
            // Set up worker for UI update.
            Windows::Foundation::TimeSpan span{ 100 * 10000 }; // 100ms refresh rate

            auto timerHandler = ref new TimerElapsedHandler([this](ThreadPoolTimer^)
            {
                __int64 meteringData = rpcclient->MeteringData;

                // If there is no new data, return
                if (sampleArray[0] == meteringData)
                {
                    return;
                }

                // Arithmetic subtraction of time
                // https://msdn.microsoft.com/en-us/library/ms724950%28VS.85%29.aspx?f=255&MSPPError=-2147217396
                FILETIME currentSystemTime;
                ULARGE_INTEGER currentTimeUi;
                GetSystemTimeAsFileTime(&currentSystemTime);
                currentTimeUi.LowPart = currentSystemTime.dwLowDateTime;
                currentTimeUi.HighPart = currentSystemTime.dwHighDateTime;

                // Calculate number of samples received since last callback
                __int64 now = rpcclient->CallbackCount;
                __int64 diff = now - rpcDataCountOld;
                rpcDataCountOld = now;

                // Calculate incoming sample rate
                double rate = (double)diff*1E7 / (currentTimeUi.QuadPart - lastUpdateTime.QuadPart);
                ExpectedRpcCallbackRate = (1E3 / this->samplePeriod).ToString() + " /sec";
                lastUpdateTime = currentTimeUi;

                ActualRpcCallbackRate = rate.ToString() + " /sec";

                // Construct the sample data string
                String^ sampleMessage = meteringData + "\n";
                unsigned int length = sampleArray->Length;
                if (sampleRefreshCount < length)
                {
                    ++sampleRefreshCount;
                    length = sampleRefreshCount;
                }

                // Insert the new data point at the top of the array.
                // Old data falls off the end of the array.
                for (unsigned int i = 1; i < length; ++i)
                {
                    sampleArray[i] = sampleArray[i - 1];
                    sampleMessage += sampleArray[i] + "\n";
                }
                sampleArray[0] = meteringData;

                SampleMessage = sampleMessage;
            });

            ThreadPoolTimer^ periodicTimer = ThreadPoolTimer::CreatePeriodicTimer(timerHandler, span);
            NotifyStatusMessage("Metering start command sent successfully", NotifyType::StatusMessage);
            retCode = this->rpcclient->StartMeteringAndWaitForStop(sampleRate);
            meteringOn = false;
            periodicTimer->Cancel();
            if (!NotifyIfAnyError(retCode) && !stopMeteringRequested)
            {
                NotifyStatusMessage("Rpc server connection closed without stop metering being requested",
                                    NotifyType::ErrorMessage);
                StartButtonEnabled = true;
                StopButtonEnabled = false;
            }
        }
    });
}

//
// Stop metering
//
void ServiceViewModel::StopMetering()
{
    create_task([this] 
    {
        StopButtonEnabled = false;
        SliderEnabled = false;
        stopMeteringRequested = true;

        __int64 retCode = this->rpcclient->StopMetering();
        if (!NotifyIfAnyError(retCode))
        {
            NotifyStatusMessage("Metering stop command sent successfully",
                                NotifyType::StatusMessage);
        }

        SliderEnabled = true;
        StartButtonEnabled = true;
    });
}

//
// Set sample period
//
void ServiceViewModel::SetSamplePeriod(int samplePeriod)
{
    create_task([this, samplePeriod] 
    {
        this->samplePeriod = samplePeriod;
        if (meteringOn)
        {
            _int64 retCode = rpcclient->SetSampleRate(samplePeriod);
            if (!NotifyIfAnyError(retCode))
            {
                NotifyStatusMessage(
                    "Sample period set to: " + samplePeriod,
                    NotifyType::StatusMessage);
            }
        }
    });
}