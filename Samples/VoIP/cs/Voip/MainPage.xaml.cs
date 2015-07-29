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
using SDKTemplate.Helpers;
using VoipTasks.BackgroundOperations;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.Diagnostics;

namespace SDKTemplate
{
    public sealed partial class MainPage : Page
    {
        //App developer should include the logic to prevent the user from being
        //able to call the Start Call async method multiple times. This is out
        //of the scope of this sample.
        private int MethodCallUnexpectedTime = -2147483634;

        public static string callerName, callerNumber;
        public static MainPage Current;
        public MainPage()
        {
            this.InitializeComponent();
            Current = this;
        }

        private void btn_NewOutgoingCall_Click(object sender, RoutedEventArgs e)
        {
            NewOutgoingCallAsync();
        }

        private async void NewOutgoingCallAsync()
        {
            txtBx_ActionLog.Text += "\n" + "Making New Outgoing Call Request";
            CallRequested.IsChecked = true;
            try {
                OperationResult result = await VoipCallHelper.NewOutgoingCallAsync(txt_CallerName.Text, txt_CallerNumber.Text);
                if (result == OperationResult.Succeeded)
                {
                    txtBx_ActionLog.Text += "\n" + "Request Succeeded.";
                    RequestSuccess.IsChecked = true;
                    InProgress.IsChecked = true;
                }
                else
                {
                    txtBx_ActionLog.Text += "\n" + "Request Failed.";
                    CallRequested.IsChecked = false;
                    RequestSuccess.IsChecked = false;
                    InProgress.IsChecked = false;
                }
            }
            catch(Exception ex)
            {
                if(ex.HResult == MethodCallUnexpectedTime)
                {
                    Debug.WriteLine("Double tapped Async Call");
                }
            }
        }

        private void btn_NewIncomingCall_Click(object sender, RoutedEventArgs e)
        {
            NewIncomingCallAsync();
        }

        private async void NewIncomingCallAsync()
        {
            txtBx_ActionLog.Text += "\n" + "Making New Incoming Call Request.";
            CallRequested.IsChecked = true;
            try
            {
                OperationResult result = await VoipCallHelper.NewIncomingCallAsync(txt_CallerName.Text, txt_CallerNumber.Text);

                if (result == OperationResult.Succeeded)
                {
                    txtBx_ActionLog.Text += "\n" + "Request Succeeded.";
                    RequestSuccess.IsChecked = true;
                }
                else
                {
                    txtBx_ActionLog.Text += "\n" + "Request Failed.";
                    CallRequested.IsChecked = false;
                    RequestSuccess.IsChecked = false;
                    InProgress.IsChecked = false;
                }
            }
            catch (Exception ex)
            {
                if (ex.HResult == MethodCallUnexpectedTime)
                {
                    Debug.WriteLine("Double tapped Async Call");
                }
            }
        }

        private void btn_EndCall_Click(object sender, RoutedEventArgs e)
        {
            EndCall();
            CallRequested.IsChecked = false;
            RequestSuccess.IsChecked = false;
            InProgress.IsChecked = false;
        }

        private void EndCall()
        {
            txtBx_ActionLog.Text += "\n" + "Making New End Call Request.";
            CallRequested.IsChecked = true;
            OperationResult result = VoipCallHelper.EndCallAsync();

            if (result == OperationResult.Succeeded)
            {
                txtBx_ActionLog.Text += "\n" + "Request Succeeded.";
                RequestSuccess.IsChecked = true;
            }
            else
            {
                txtBx_ActionLog.Text += "\n" + "Request Failed.";
                CallRequested.IsChecked = false;
            }
        }

        private void btn_GetCallDuration_Click(object sender, RoutedEventArgs e)
        {
            ShowCallDuration();
        }

        private async void ShowCallDuration()
        {
            txtBx_ActionLog.Text += "\n" + "Showing Call Duration.";
            String result = await VoipCallHelper.GetCallDurationAsync();

            if (result != null)
            {
                MessageDialog dialog = new MessageDialog(result);
                await dialog.ShowAsync();
            }
            else
            {
                txtBx_ActionLog.Text += "\n" + "Request Failed.";
            }
        }
    }
}