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
using System.Threading.Tasks;
using VoipBackEnd;
using VoipTasks.BackgroundOperations;
using Windows.ApplicationModel.AppService;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Calls;
using Windows.Foundation.Collections;

namespace VoipTasks.Helpers
{
    static class Current
    {
        public static String RtcCallTaskName
        {
            get { return _rtcCallTaskName; }
        }

        public static AppServiceRequest AppRequest
        {
            set
            {
                lock (_lock)
                {
                    _appRequest = value;
                }
            }
            get
            {
                lock (_lock)
                {
                    return _appRequest;
                }
            }
        }

        public static AppServiceDeferral AppRequestDeferal
        {
            set
            {
                lock (_lock)
                {
                    _appDeferral = value;
                }
            }
            get
            {
                lock (_lock)
                {
                    return _appDeferral;
                }
            }
        }

        public static BackgroundTaskDeferral RTCTaskDeferral
        {
            set
            {
                lock (_lock)
                {
                    _rtcTaskDeferral = value;
                }
            }
            get
            {
                lock (_lock)
                {
                    return _rtcTaskDeferral;
                }
            }
        }

        public static VoipPhoneCall VoipCall
        {
            set
            {
                lock (_lock)
                {
                    _voipCall = value;
                }
            }
            get
            {
                lock (_lock)
                {
                    return _voipCall;
                }
            }
        }

        public static BackgroundRequest Request
        {
            set
            {
                lock (_lock)
                {
                    _request = value;
                }
            }
            get
            {
                lock (_lock)
                {
                    return _request;
                }
            }
        }

        public static async void SendResponse(ValueSet response)
        {
            AppServiceRequest request = AppRequest;
            AppServiceDeferral deferal = AppRequestDeferal;

            try
            {
                if (request != null)
                {
                    await request.SendResponseAsync(response);
                }
            }
            finally
            {
                if (deferal != null)
                {
                    deferal.Complete();
                    deferal = null;
                }

                AppRequest = null;
            }
        }

        public static async Task<bool> GetCallDurationAsync()
        {
            VoipPhoneCall call = VoipCall;
            AppServiceRequest request = AppRequest;
            AppServiceDeferral deferal = AppRequestDeferal;
            TimeSpan timeSpan = new TimeSpan();

            if (call != null)
            {
                call.NotifyCallActive();
                timeSpan = DateTimeOffset.Now.Subtract(call.StartTime);
            }

            if (request != null)
            {
                ValueSet response = new ValueSet();
                response[BackgroundOperation.Result] = timeSpan.ToString();
                await request.SendResponseAsync(response);
            }

            if (deferal != null)
            {
                deferal.Complete();
            }

            return true;
        }

        public static async void EndCallAsync()
        {
            AppServiceRequest request = AppRequest;
            AppServiceDeferral deferal = AppRequestDeferal;

            try
            {
                StopAudio();
                EndCall();
                
                if (request != null)
                {
                    ValueSet response = new ValueSet();
                    response[BackgroundOperation.Result] = (int)OperationResult.Succeeded;
                    await request.SendResponseAsync(response);
                }

                if (deferal != null)
                {
                    deferal.Complete();
                }
            }
            finally
            {
                AppRequest = null;
                AppRequestDeferal = null;
            }
        }


        public static void EndCall()
        {
            VoipPhoneCall call = VoipCall;
            BackgroundTaskDeferral deferral = RTCTaskDeferral;

            try
            {
                StopAudio();

                if (call != null)
                {
                    call.NotifyCallEnded();
                }
            }
            catch
            {

            }

            try
            {
                if (deferral != null)
                {
                    deferral.Complete();
                }

            }
            catch
            {

            }
            finally
            {
                VoipCall = null;
                RTCTaskDeferral = null;
            }
        }

        public static async Task<VoipPhoneCallResourceReservationStatus> RequestNewCallAsync(string context, string contactName, string serviceName)
        {
            VccCallHelper vccCallHelper;
            VoipPhoneCallResourceReservationStatus status = VoipPhoneCallResourceReservationStatus.ResourcesNotAvailable;

            lock (_lock)
            {
                vccCallHelper = _vccCallHelper;
            }

            if (vccCallHelper != null)
            {
                status = await vccCallHelper.RequestNewCallAsync( context, contactName, serviceName);
            }

            return status;
        }

        public static void StartAudio()
        {
            lock (_lock)
            {
                // Initialize transport
                transportController = new BackEndTransport(); // local

                // Initialize audio controller
                audioController = new BackEndAudio();

                // Set the transport for audio
                audioController.SetTransport(transportController);

                audioController.Start();
            }
        }

        public static void StopAudio()
        {
            lock (_lock)
            {
                if (audioController != null)
                {
                    audioController.Stop();
                }

                audioController = null;
                transportController = null;
            }
        }

        private const String _rtcCallTaskName = "VoipTasks.CallRtcTask";
        private static BackgroundRequest _request = BackgroundRequest.InValid;
        private static Object _lock = new Object();
        private static AppServiceRequest _appRequest = null;
        private static AppServiceDeferral _appDeferral = null;
        private static VoipPhoneCall _voipCall = null;
        private static VccCallHelper _vccCallHelper = new VccCallHelper();
        private static BackgroundTaskDeferral _rtcTaskDeferral = null;
        private static BackEndTransport transportController;
        private static BackEndAudio audioController;
    }
}
