//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.System.Threading;

[StructLayout(LayoutKind.Explicit, Size = 8)]
struct LARGE_INTEGER
{
    [FieldOffset(0)] public Int64 QuadPart;
    [FieldOffset(0)] public Int32 LowPart;
    [FieldOffset(4)] public Int32 HighPart;
}

namespace SDKTemplate
{
    [Windows.UI.Xaml.Data.Bindable]
    public class ServiceViewModel : INotifyPropertyChanged
    {
        // RpcClient API's implemented in C++ dll
        [DllImport("RpcClient.dll")]
        static extern long RpcClientInitialize(out IntPtr rpcClient);

        [DllImport("RpcClient.dll")]
        static extern long RpcClientClose(IntPtr rpcClient);

        [DllImport("RpcClient.dll")]
        static extern long SetSampleRate(IntPtr rpcClient, int rate);

        [DllImport("RpcClient.dll")]
        static extern long StartMeteringAndWaitForStop(IntPtr RpcClientHandle, long samplePeriod);

        [DllImport("RpcClient.dll")]
        static extern long GetMeteringData(IntPtr RpcClientHandle, ref long MeteringData);

        [DllImport("RpcClient.dll")]
        static extern long GetCallbackCount(IntPtr RpcClientHandle, ref long MeteringData);

        [DllImport("RpcClient.dll")]
        static extern long StopMeteringData(IntPtr RpcClientHandle);

        [DllImport("api-ms-win-core-sysinfo-l1-2-1.dll")]
        static extern void GetSystemTimeAsFileTime(ref FILETIME lpSystemTimeAsFileTime);

        [DllImport("api-ms-win-core-libraryloader-l2-1-0.dll", CharSet = CharSet.Unicode)]
        static extern IntPtr LoadPackagedLibrary(string filename, uint reserved);

        public static ServiceViewModel Current { get; private set; }

        static ServiceViewModel()
        {
            Current = new ServiceViewModel();

            // Register for app suspend/resume handlers
            App.Current.Suspending += ServiceViewModel.Current.SuspendMeteringService;
            App.Current.Resuming += ServiceViewModel.Current.ResumeMeteringService;
        }

        private async void ResumeMeteringService(object sender, object e)
        {
            if (meteringOnWhileSuspension) {
                await StartMetering(samplePeriod);
                meteringOnWhileSuspension = false;
            }
        }

        private async void SuspendMeteringService(object sender, SuspendingEventArgs e)
        {
            var deferral = e.SuspendingOperation.GetDeferral();
            if (meteringOn) {
                await StopMetering();
                meteringOnWhileSuspension = true;
            }
            deferral.Complete();
        }

        Lazy<bool> isSupported = new Lazy<bool>(() => LoadPackagedLibrary("RpcClient.dll", 0) != IntPtr.Zero);
        volatile bool meteringOn;
        volatile bool meteringOnWhileSuspension;
        Windows.UI.Core.CoreDispatcher dispatcher;
        int samplePeriod;
        LARGE_INTEGER lastUpdateTime;
        long rpcDataCountOld = 0;
        uint sampleRefreshCount = 0;
        IntPtr rpcClient;
        bool stopMeteringRequested;
        long[] sampleArray;

        // Variables backing UI bindings
        string sampleMessage;
        string expectedRpcCallbackRate;
        string actualRpcCallbackRate;
        bool startButtonEnabled;
        bool stopButtonEnabled;
        bool sliderEnabled;
        double sliderValue;

        internal string ExpectedRpcCallbackRate
        {
            get { return expectedRpcCallbackRate; }
            set
            {
                expectedRpcCallbackRate = value;
                NotifyPropertyChangedAsync(nameof(ExpectedRpcCallbackRate));
            }
        }
        internal string ActualRpcCallbackRate
        {
            get { return actualRpcCallbackRate; }
            set
            {
                actualRpcCallbackRate = value;
                NotifyPropertyChangedAsync(nameof(ActualRpcCallbackRate));
            }
        }
        internal string SampleMessage
        {
            get { return sampleMessage; }
            set
            {
                sampleMessage = value;
                NotifyPropertyChangedAsync(nameof(SampleMessage));
            }
        }
        internal bool StartButtonEnabled
        {
            get { return startButtonEnabled; }
            set {
                startButtonEnabled = value;
                NotifyPropertyChangedAsync("StartButtonEnabled");
            }
        }
        internal bool StopButtonEnabled
        {
            get { return stopButtonEnabled; }
            set
            {
                stopButtonEnabled = value;
                NotifyPropertyChangedAsync(nameof(StopButtonEnabled));
            }
        }
        internal bool SliderEnabled
        {
            get { return sliderEnabled; }
            set
            {
                sliderEnabled = value;
                NotifyPropertyChangedAsync(nameof(SliderEnabled));
            }
        }
        internal double SliderValue
        {
            get { return sliderValue; }
            set
            {
                sliderValue = value;
                NotifyPropertyChangedAsync(nameof(SliderValue));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private ServiceViewModel()
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
            meteringOnWhileSuspension = false;

            // Get the dispatcher for notifying property change
            dispatcher = Windows.UI.Core.CoreWindow.GetForCurrentThread().Dispatcher;

            sampleArray = new long[1000];
        }

        //
        // Initializes rpcclient and metering
        //
        internal async Task StartMetering(int sampleRate)
        {
            await Task.Run(() =>
            {
                rpcClient = IntPtr.Zero;

                SampleMessage = "";
                StartButtonEnabled = false;
                StopButtonEnabled = true;
                stopMeteringRequested = false;

                if (!isSupported.Value)
                {
                    NotifyStatusMessage("NT service support has not been compiled for this architecture.", NotifyType.ErrorMessage);
                    return;
                }

                if (NotifyIfAnyError(RpcClientInitialize(out rpcClient))) return;

                meteringOn = true;
                sampleRefreshCount = 0;
                sampleArray[0] = 0;

                // Set the sample rate on server
                samplePeriod = sampleRate;
                long retCode = SetSampleRate(rpcClient, sampleRate);

                FILETIME initialSystemTime = new FILETIME();
                GetSystemTimeAsFileTime(ref initialSystemTime);
                lastUpdateTime = new LARGE_INTEGER();
                lastUpdateTime.LowPart = initialSystemTime.dwLowDateTime;
                lastUpdateTime.HighPart = initialSystemTime.dwHighDateTime;

                if (!NotifyIfAnyError(retCode))
                {
                    // Set up worker for UI update.
                    TimeSpan span = new TimeSpan(100 * 10000); // 100ms refresh rate

                    TimerElapsedHandler timerHandler = (ThreadPoolTimer) =>
                    {
                        long meteringData = 0;
                        GetMeteringData(rpcClient, ref meteringData);

                        // If there is no new data, return
                        if (sampleArray[0] == meteringData)
                        {
                            return;
                        }

                        // Arithmetic subtraction of time
                        // https://msdn.microsoft.com/en-us/library/ms724950%28VS.85%29.aspx
                        FILETIME currentSystemTime = new FILETIME();
                        LARGE_INTEGER currentTimeUi = new LARGE_INTEGER();
                        GetSystemTimeAsFileTime(ref currentSystemTime);
                        currentTimeUi.LowPart = currentSystemTime.dwLowDateTime;
                        currentTimeUi.HighPart = currentSystemTime.dwHighDateTime;

                        // Calculate number of samples received since last callback
                        long now = 0, diff = 0;
                        GetCallbackCount(rpcClient, ref now);

                        diff = now - rpcDataCountOld;
                        rpcDataCountOld = now;

                        // Calculate incoming sample rate
                        double rate = (double)diff * 1E7 / (currentTimeUi.QuadPart - lastUpdateTime.QuadPart);
                        ExpectedRpcCallbackRate = (1E3 / samplePeriod).ToString() + " /sec";
                        lastUpdateTime = currentTimeUi;

                        ActualRpcCallbackRate = rate.ToString() + " /sec";

                        // Construct the sample data string
                        string sampleMessage = meteringData + "\n";
                        uint length = (uint)sampleArray.Length;
                        if (sampleRefreshCount < length)
                        {
                            ++sampleRefreshCount;
                            length = sampleRefreshCount;
                        }

                        // Insert the new data point at the top of the array.
                        // Old data falls off the end of the array.
                        for (int i = 1; i < length; ++i)
                        {
                            sampleArray[i] = sampleArray[i - 1];
                            sampleMessage += sampleArray[i] + "\n";
                        }
                        sampleArray[0] = meteringData;

                        SampleMessage = sampleMessage;
                    };

                    ThreadPoolTimer periodicTimer = ThreadPoolTimer.CreatePeriodicTimer(timerHandler, span);
                    NotifyStatusMessage("Metering start command sent successfully", NotifyType.StatusMessage);
                    retCode = StartMeteringAndWaitForStop(rpcClient, sampleRate);
                    meteringOn = false;
                    periodicTimer.Cancel();
                    if (!NotifyIfAnyError(retCode) && !stopMeteringRequested)
                    {
                        NotifyStatusMessage("Rpc server connection closed without stop metering being requested",
                                            NotifyType.ErrorMessage);
                        StartButtonEnabled = true;
                        StopButtonEnabled = false;
                    }
                    RpcClientClose(rpcClient);
                }
            });
        }

        void NotifyStatusMessage(string message, NotifyType messageType)
        {
            MainPage.Current.NotifyUser(message, messageType);
        }

        //
        // if errCode represents an error, then display a message and return true.
        //
        public bool NotifyIfAnyError(long errCode)
        {
            if (errCode != 0)
            {
                meteringOn = false;
                NotifyStatusMessage("Error occured while communicating with RPC server: " + errCode.ToString(), NotifyType.ErrorMessage);

                // Reset button state
                StartButtonEnabled = true;
                StopButtonEnabled = false;
                return true;
            }
            return false;
        }

        internal async Task StopMetering()
        {
            await Task.Run(() =>
            {
                StopButtonEnabled = false;
                SliderEnabled = false;
                stopMeteringRequested = true;

                if (isSupported.Value)
                {
                    long retCode = StopMeteringData(rpcClient);
                    if (!NotifyIfAnyError(retCode))
                    {
                        NotifyStatusMessage("Metering stop command sent successfully",
                                            NotifyType.StatusMessage);
                    }
                }

                SliderEnabled = true;
                StartButtonEnabled = true;
            });
        }

        internal void SetSamplePeriod(int value)
        {
            samplePeriod = value;
            if (meteringOn)
            {
                long retCode = SetSampleRate(rpcClient, samplePeriod);
                if (!NotifyIfAnyError(retCode))
                {
                    NotifyStatusMessage(
                        "Sample period set to: " + samplePeriod,
                        NotifyType.StatusMessage);
                }
            }
        }

        private async void NotifyPropertyChangedAsync(string prop)
        {
            if (dispatcher != null)
            {
                await dispatcher.RunAsync(
                    Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () => PropertyChanged(this, new PropertyChangedEventArgs(prop)));
            }
        }
    }
}
