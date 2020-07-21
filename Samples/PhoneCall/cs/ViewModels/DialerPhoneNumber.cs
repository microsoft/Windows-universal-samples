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

using System.Windows.Input;
using PhoneCall.Helpers;

namespace PhoneCall.ViewModels
{
    /// <summary>
    /// A simple class that holds and modifies the phone number to dial
    /// in the dial pad module.
    /// </summary>
    public class DialerPhoneNumber : ViewModelBase
    {
        private string numberToDial = "";
        private bool dialPadEnabled = false;
        private RelayCommand backSpaceCommand;
        private RelayCommand backSpaceHoldingCommand;

        /// <summary>
        /// Takes an input, ensures its a dialable chanracter.
        /// Then appends this input to the number field
        /// </summary>
        private void cleanInput(string content)
        {
            foreach (char c in content)
            {
                if (",;+#*0123456789".Contains(c.ToString()))
                {
                    this.NumberToDial += c;                
                    EvalDialerState();
                }
            }
        }

        /// <summary>
        /// Evaluates the dialpad buttons with secondary characters.
        /// These characters are replaced with the primary on press and hold
        /// </summary>
        public void ReplaceOnHoldingDigit(string newDigit)
        {
            if (numberToDial.Length > 0)
            {
                if ((newDigit == "+") && (numberToDial.Length > 1))
                {
                    return;
                }
                this.NumberToDial = this.NumberToDial.Remove(numberToDial.Length - 1);
                this.NumberToDial += newDigit;
            }
        }

        /// <summary>
        /// Removes the last character in the number field.
        /// </summary>
        private void BackSpaceInvoked()
        {
            if (numberToDial.Length > 0)
            {
                this.NumberToDial = this.NumberToDial.Remove(numberToDial.Length - 1);
                EvalDialerState();
            }
        }

        /// <summary>
        /// Clears the entire number field.
        /// </summary>
        public void ClearDialerNumberHeap()
        {
            if (numberToDial.Length > 0)
            {
                this.NumberToDial = "";
                EvalDialerState();
            }
        }

        /// <summary>
        /// Evaluate the number field.
        /// if its empty, it returns false, which disables the call and backspace button in the dialer
        /// if its not empty, it returns true, which enables the call and backspace button in the dialer
        /// </summary>
        private void EvalDialerState()
        {
            if (numberToDial.Length > 0)
            {
                DialPadEnabled = true;
            }
            else
            {
                DialPadEnabled = false;
            }
        }

        /// <summary>
        /// Relay command to execute the backspace action from the dialer view.
        /// </summary>
        public ICommand BackSpaceCommand
        {
            get
            {
                if (backSpaceCommand == null)
                {
                    backSpaceCommand = new RelayCommand(p => 
                        this.BackSpaceInvoked());
                }
                return backSpaceCommand;
            }
        }

        /// <summary>
        /// Relay command to execute the clear phone number action from the dialer view.
        /// </summary>
        public ICommand BackSpaceHoldingCommand
        {
            get
            {
                if (backSpaceHoldingCommand == null)
                {
                    backSpaceHoldingCommand = new RelayCommand(p => 
                        this.ClearDialerNumberHeap());
                }
                return backSpaceHoldingCommand;
            }
        }

        #region Setters and Getters
        /// <summary>
        /// Appends the latest dialed character to the number field.
        /// </summary>
        public string Digit
        {
            set
            {
                cleanInput(value);
            }
        }

        /// <summary>
        /// Gets and sets the phone number to be dialed.
        /// </summary>
        public string NumberToDial
        {
            get
            {
                return numberToDial;
            }
            private set
            {
                if (numberToDial != value)
                {
                    numberToDial = value;
                    RaisePropertyChanged();
                }
            }
        }

        /// <summary>
        /// Gets and sets the state of the dialer (enable/disable the call and backspace button).
        /// </summary>
        public bool DialPadEnabled
        {
            get
            {
                return dialPadEnabled;
            }
            private set
            {
                if (dialPadEnabled != value)
                {
                    dialPadEnabled = value;
                    RaisePropertyChanged();
                }
            }
        }
        #endregion

    }
}
