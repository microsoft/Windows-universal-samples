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
using System.Windows.Input;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    class DialerViewModel : ViewModelBase
    {
        private DialerPhoneNumber dialerPhoneNumber;
        private RelayCommand dialPadCommand;
        private RelayCommand dialPadHoldingCommand;
        private RelayCommand dialDialerNumberHeapCommand;


        /// <summary>
        /// A view model class for the dialer panel/module.
        /// </summary>
        public DialerViewModel()
        {
            dialerPhoneNumber = new DialerPhoneNumber();
        }

        /// <summary>
        /// Relay command action to append the primary newly dialed digit to the string 
        /// of number to dial
        /// </summary>
        /// <param name="CommandParam">Primary newly dialed digit string</param>
        private void DialPadInvoked(object CommandParam)
        {
            dialerPhoneNumber.Digit = CommandParam as string;
        }

        /// <summary>
        /// Relay command action to append the secdondary newly dialed digit to the string 
        /// of number to dial.
        /// For "1" this just dials voicemail. 
        /// </summary>
        /// <param name="CommandParam">Secdondary newly dialed digit string</param>
        private async void DialPadHoldingInvokedAsync(object CommandParam)
        {
            string OnHoldingDigit = CommandParam as string;

            if ((OnHoldingDigit == "1") && (dialerPhoneNumber.NumberToDial.Length == 1) && (CallingInfo.CallingInfoInstance.CurrentPhoneLine != null))
            {
                dialerPhoneNumber.ClearDialerNumberHeap();
                await CallingInfo.CallingInfoInstance.CurrentPhoneLine.Voicemail.DialVoicemailAsync();
            }
            else
            {
                dialerPhoneNumber.ReplaceOnHoldingDigit(OnHoldingDigit);
            }
        }

        /// <summary>
        /// Relay command action to place a call to the numbers dialed.
        /// </summary>
        private void DialDialerNumberHeap()
        {
            if(dialerPhoneNumber.NumberToDial.Length > 0)
            {
                string phoneNumber = this.dialerPhoneNumber.NumberToDial;
                this.dialerPhoneNumber.ClearDialerNumberHeap();
                CallingInfo.CallingInfoInstance.DialOnCurrentLineAsync(phoneNumber, phoneNumber);
            }
        }

        /// <summary>
        /// Relay command to execute the dial invoked action from the dialer view.
        /// </summary>
        public ICommand ProcessDialPad
        {
            get
            {
                if (dialPadCommand == null)
                {
                    dialPadCommand = new RelayCommand(
                        this.DialPadInvoked);
                }
                return dialPadCommand;
            }
        }

        /// <summary>
        /// Relay command to execute the dial pad holding action from the dialer view.
        /// </summary>
        public ICommand ProcessDialPadHolding
        {
            get
            {
                if (dialPadHoldingCommand == null)
                {
                    dialPadHoldingCommand = new RelayCommand(
                        this.DialPadHoldingInvokedAsync);
                }
                return dialPadHoldingCommand;
            }
        }

        /// <summary>
        /// Relay command to execute the DialDialerNumberHeap action from the dialer view.
        /// </summary>
        public ICommand ProcessDialDialerNumberHeap
        {
            get
            {
                if (dialDialerNumberHeapCommand == null)
                {
                    dialDialerNumberHeapCommand = new RelayCommand(p =>
                        this.DialDialerNumberHeap());
                }
                return dialDialerNumberHeapCommand;
            }
        }

        /// <summary>
        /// Relay command to execute the backspace action from the dialer view.
        /// </summary>
        public ICommand ProcessBackspace
        {
            get
            {
                return dialerPhoneNumber.BackSpaceCommand;
            }
        }

        /// <summary>
        /// Relay command to execute the backspace holding (clear dialer number heap) action from the dialer view.
        /// </summary>
        public ICommand ProcessBackSpaceHolding
        {
            get
            {
                return dialerPhoneNumber.BackSpaceHoldingCommand;
            }
        }

        #region Getters
        /// <summary>
        /// Get the phone number to dial object.
        /// </summary>
        public DialerPhoneNumber DialerPhoneNumber
        {
            get
            {
                return dialerPhoneNumber;
            }
        }
        #endregion
    }
}
