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
using System.Diagnostics;
using System.Threading.Tasks;
using VoipTasks.BackgroundOperations;
using Windows.ApplicationModel.Calls;
using Windows.Foundation.Collections;
using Windows.Foundation.Metadata;

namespace VoipTasks.Helpers
{
    class VccCallHelper
    {
        private int RTcTaskAlreadyRuningErrorCode = -2147024713;
        public async Task<VoipPhoneCallResourceReservationStatus> RequestNewCallAsync(string context, string contactName, string serviceName)
        {
            VoipPhoneCallResourceReservationStatus status = VoipPhoneCallResourceReservationStatus.ResourcesNotAvailable;
            
            switch (Current.Request)
            {
                case BackgroundRequest.NewIncomingCall:
                    status = await LaunchRTCTaskAsync();
                    if (status == VoipPhoneCallResourceReservationStatus.Success)
                    {
                        NewIncomingCall(context, contactName, serviceName);
                    }
                    break;

                case BackgroundRequest.NewOutgoingCall:
                    status = await LaunchRTCTaskAsync();
                    if (status == VoipPhoneCallResourceReservationStatus.Success)
                    {
                        NewOutgoingCall(context, contactName, serviceName);
                    }
                    break;
            }

            Current.Request = BackgroundRequest.InValid;

            return status;
        }

        private async Task<VoipPhoneCallResourceReservationStatus> LaunchRTCTaskAsync()
        {
            // End current call before starting another call, there should be only one RTC task active at a time.
            // Duplicate calls to launch RTC task will result in HR ERROR_ALREADY_EXSIST
            // <TODO> For multiple calls against single rtc task add logic to verify that the rtc is not completed, 
            // and then Skip launching new rtc task
            Current.EndCall();

            VoipCallCoordinator vCC = VoipCallCoordinator.GetDefault();
            VoipPhoneCallResourceReservationStatus status = VoipPhoneCallResourceReservationStatus.ResourcesNotAvailable;

            try
            {
                status = await vCC.ReserveCallResourcesAsync(Current.RtcCallTaskName);
            }
            catch (Exception ex)
            {
                if (ex.HResult == RTcTaskAlreadyRuningErrorCode )
                {
                    Debug.WriteLine("RTC Task Already running");
                }
            }

            return status;
        }

        private void NewOutgoingCall(string context, string contactName, string serviceName)
        {
            bool status = false;
            try
            {
                VoipCallCoordinator vCC = VoipCallCoordinator.GetDefault();
                VoipPhoneCall call = vCC.RequestNewOutgoingCall( context, contactName, serviceName, VoipPhoneCallMedia.Audio);
                if (call != null)
                {
                    call.EndRequested += Call_EndRequested;
                    call.HoldRequested += Call_HoldRequested;
                    call.RejectRequested += Call_RejectRequested;
                    call.ResumeRequested += Call_ResumeRequested;

                    call.NotifyCallActive();

                    Current.VoipCall = call;

                    Current.StartAudio();

                    status = true;
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            ValueSet response = new ValueSet();
            response[BackgroundOperation.Result] = status ? (int)OperationResult.Succeeded : (int)OperationResult.Failed;

            Current.SendResponse(response);
        }

        private void NewIncomingCall(string context, string contactName, string serviceName)
        {
            bool status = false;
            try
            {

                VoipCallCoordinator vCC = VoipCallCoordinator.GetDefault();
                VoipPhoneCall call = vCC.RequestNewIncomingCall(
                                                                "Hello",
                                                                contactName,
                                                                context,
                                                                new Uri("file://c:/data/test/bin/FakeVoipAppLight.png"),
                                                                serviceName,
                                                                new Uri("file://c:/data/test/bin/FakeVoipAppLight.png"),
                                                                "",
                                                               new Uri("file://c:/data/test/bin/FakeVoipAppRingtone.wma"),
                                                                VoipPhoneCallMedia.Audio,
                                                                new TimeSpan(0, 1, 20));
                if (call != null)
                {
                    call.AnswerRequested += Call_AnswerRequested;
                    call.EndRequested += Call_EndRequested;
                    call.HoldRequested += Call_HoldRequested;
                    call.RejectRequested += Call_RejectRequested;
                    call.ResumeRequested += Call_ResumeRequested;

                    Current.VoipCall = call;

                    status = true;
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            ValueSet response = new ValueSet();
            response[BackgroundOperation.Result] = status ? (int)OperationResult.Succeeded : (int)OperationResult.Failed;

            Current.SendResponse(response);
        }

        private void Call_ResumeRequested(VoipPhoneCall sender, CallStateChangeEventArgs args)
        {
            sender.NotifyCallActive();
            Current.VoipCall = sender;
            Current.StartAudio();
        }

        private void Call_RejectRequested(VoipPhoneCall sender, CallRejectEventArgs args)
        {
            Current.StopAudio();
            sender.NotifyCallEnded();
            Current.VoipCall = null;
        }

        private void Call_HoldRequested(VoipPhoneCall sender, CallStateChangeEventArgs args)
        {
            Current.StopAudio();
            sender.NotifyCallHeld();
            Current.VoipCall = sender;
        }

        private void Call_EndRequested(VoipPhoneCall sender, CallStateChangeEventArgs args)
        {
            Current.StopAudio();
            sender.NotifyCallEnded();
            Current.VoipCall = null;
        }

        private void Call_AnswerRequested(VoipPhoneCall sender, CallAnswerEventArgs args)
        {
            sender.NotifyCallActive();
            Current.VoipCall = sender;
            Current.StartAudio();
        }
    }
}
