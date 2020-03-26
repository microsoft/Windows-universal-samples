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
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel.Calls;
using Windows.UI;
using Windows.UI.Popups;

namespace PhoneCall.Helpers
{
    public sealed class CallingInfo
    {
        public delegate void CallingInfoDelegate();
        public event CallingInfoDelegate CellInfoUpdateCompleted;
        public event CallingInfoDelegate ActivePhoneCallStateChanged;

        private int noOfLines;
        private Dictionary<Guid,PhoneLine> allPhoneLines;
        private PhoneLine currentPhoneLine;
        private int currentSIMSlotIndex;
        private string currentSIMState;
        private string currentNetworkState;
        //For multi line devices, we allow uses to name their lines. This holds that information
        private string currentDisplayName;      
        private string currentOperatorName;
        //For multi line devices, each line has a specific theme color. This holds that information
        private Color currentDisplayColor;
        private string currentVoicemailNumber;
        private int currentVoicemailCount;
        private bool doesPhoneCallExist;

        private static readonly Lazy<CallingInfo> callingInfoInstance = 
            new Lazy<CallingInfo>(() => new CallingInfo());

        /// <summary>
        /// Gets the calling information singleton.
        /// </summary>
        public static CallingInfo CallingInfoInstance
        {
            get
            {
                return callingInfoInstance.Value;
            }
        }

        /// <summary>
        /// A singleton that querys, holds, and updates calling information like phone lines and their information.
        /// </summary>
        private CallingInfo()
        {
            InitializeCallingInfoAsync();
        }

        #region Public functions

        /// <summary>
        /// Used to update the current phone line when the user changes this from the line picker.
        /// </summary>
        /// <param name="lineGuid">The unique ID of the new line. Used to retrieve the phone line</param>
        public void ChangePhoneLine(Guid lineGuid)
        {
            PhoneLine newLine = AllPhoneLines[lineGuid];
            if (newLine != null)
            {
                currentPhoneLine = newLine;
                updateCellularInformation();
            }
        }

        /// <summary>
        /// Dials a phone number with the display name on the current phone line.
        /// </summary>
        /// <param name="PhoneNumber">The phone number to dial</param>
        /// <param name="DisplayName">The display name to show in the UI</param>
        public async void DialOnCurrentLineAsync(string PhoneNumber, string DisplayName)
        {
            if ((currentPhoneLine != null) && (PhoneNumber.Trim().Length > 0))
            {
                currentPhoneLine.Dial(PhoneNumber, DisplayName);
            }
            else
            {
                var dialog = new MessageDialog("No line found to place the call");
                await dialog.ShowAsync();
            }
        }
        #endregion

        #region Private functions    

        /// <summary>
        /// Sets up an event listner for phone call state.
        /// Gets all cellular phone line objects.
        /// Calculates the number of lines on the device.
        /// Gets the default phone line object.
        /// Updates the cellular information properties based on the default phone line.
        /// </summary>
        private async void InitializeCallingInfoAsync()
        {
            this.MonitorCallState();

            //Get all phone lines (To detect dual SIM devices)
            Task<Dictionary<Guid,PhoneLine>> getPhoneLinesTask = GetPhoneLinesAsync();
            allPhoneLines = await getPhoneLinesTask;

            //Get number of lines
            noOfLines = allPhoneLines.Count;

            //Get Default Phone Line
            Task<PhoneLine> getDefaultLineTask = GetDefaultPhoneLineAsync();
            currentPhoneLine = await getDefaultLineTask;

            //Update cellular information based on default line
            updateCellularInformation();
        }

        /// <summary>
        /// Registers for and monitors the call state change event
        /// Fires a delegate event to let others registered know to update their UI
        /// </summary>
        private void MonitorCallState()
        {
            PhoneCallManager.CallStateChanged += (o, args) =>
            {
                doesPhoneCallExist = PhoneCallManager.IsCallActive || PhoneCallManager.IsCallIncoming;
                if (ActivePhoneCallStateChanged != null)
                {
                    ActivePhoneCallStateChanged();
                }
            };
        }

        /// <summary>
        /// Enumerate through all phone lines and returns a list of all phone lines
        /// </summary>
        /// <returns>A dictionary of cellular phone lines and their guids.</returns>    
        private async Task<Dictionary<Guid,PhoneLine>> GetPhoneLinesAsync()
        {
            PhoneCallStore store = await PhoneCallManager.RequestStoreAsync();

            // Start the PhoneLineWatcher
            var watcher = store.RequestLineWatcher();
            var phoneLines = new List<PhoneLine>();
            var lineEnumerationCompletion = new TaskCompletionSource<bool>();
            watcher.LineAdded += async (o, args) => { var line = await PhoneLine.FromIdAsync(args.LineId); phoneLines.Add(line); };
            watcher.Stopped += (o, args) => lineEnumerationCompletion.TrySetResult(false);
            watcher.EnumerationCompleted += (o, args) => lineEnumerationCompletion.TrySetResult(true);
            watcher.Start();

            // Wait for enumeration completion
            if (!await lineEnumerationCompletion.Task)
            {
                throw new Exception("Phone Line Enumeration failed");
            }

            watcher.Stop();

            Dictionary<Guid,PhoneLine> returnedLines = new Dictionary<Guid,PhoneLine>();

            foreach (PhoneLine phoneLine in phoneLines)
            {
                if (phoneLine != null && phoneLine.Transport == PhoneLineTransport.Cellular)
                {
                    returnedLines.Add(phoneLine.Id,phoneLine);
                }
            }

            return returnedLines;
        }

        /// <summary>
        /// Gets the default phone line
        /// </summary>
        /// <returns>A one phone line. The default one on the device.</returns> 
        private async Task<PhoneLine> GetDefaultPhoneLineAsync()
        {
            PhoneCallStore phoneCallStore = await PhoneCallManager.RequestStoreAsync();
            Guid lineId = await phoneCallStore.GetDefaultLineAsync();
            return await PhoneLine.FromIdAsync(lineId);
        }

        /// <summary>
        /// Updates the cellular information fields based on the current phone line.
        /// </summary>
        private void updateCellularInformation()
        {
            PhoneLine line = currentPhoneLine;
            PhoneLineCellularDetails cellularDetails = line.CellularDetails;

            //Update SIM slot index
            currentSIMSlotIndex = cellularDetails.SimSlotIndex;

            //Update display name
            currentDisplayName = line.DisplayName;

            //Update display Color
            currentDisplayColor = line.DisplayColor;

            //Update voicemail number
            currentVoicemailNumber = line.Voicemail.Number;

            //Update voicemail count
            currentVoicemailCount = line.Voicemail.MessageCount;

            //Set default operator name
            currentOperatorName = "N/A";

            //Update sim state
            PhoneSimState simState = cellularDetails.SimState;
            switch (simState)
            {
                case PhoneSimState.Unknown:
                    currentSIMState = "Unknown";
                    break;
                case PhoneSimState.PinNotRequired:
                    currentSIMState = "Pin Not Required";
                    break;
                case PhoneSimState.PinUnlocked:
                    currentSIMState = "Pin Unlocked";
                    break;
                case PhoneSimState.PinLocked:
                    currentSIMState = "Pin Locked";
                    break;
                case PhoneSimState.PukLocked:
                    currentSIMState = "Puk Locked";
                    break;
                case PhoneSimState.NotInserted:
                    currentSIMState = "No SIM";
                    break;
                case PhoneSimState.Invalid:
                    currentSIMState = "Invalid";
                    break;
                case PhoneSimState.Disabled:
                    currentSIMState = "Disabled";
                    break;
                default:
                    currentSIMState = "Unknown";
                    break;
            }

            //Update network state
            PhoneNetworkState networkState = line.NetworkState;
            switch (line.NetworkState)
            {
                case PhoneNetworkState.NoSignal:
                    if ((bool)line.LineConfiguration.ExtendedProperties["ShouldDisplayEmergencyCallState"])
                    {
                        currentNetworkState = "Emergency calls only";
                        break;
                    }
                    else
                    {
                        currentNetworkState = "No Service";
                        break;
                    }

                case PhoneNetworkState.Deregistered:
                    currentNetworkState = "Deregistered";
                    break;
                case PhoneNetworkState.Denied:
                    currentNetworkState = "Denied";
                    break;
                case PhoneNetworkState.Searching:
                    currentNetworkState = "Searching";
                    break;
                case PhoneNetworkState.Home:
                    currentNetworkState = "Connected";
                    currentOperatorName = line.NetworkName;
                    break;
                case PhoneNetworkState.RoamingInternational:
                    currentNetworkState = "Roaming International";
                    currentOperatorName = line.NetworkName;
                    break;
                case PhoneNetworkState.RoamingDomestic:
                    currentNetworkState = "Roaming Domestic";
                    currentOperatorName = line.NetworkName;
                    break;
                default:
                    currentNetworkState = "Unknown";
                    break;
            }

            //Cell info update complete. Fire event
            if (CellInfoUpdateCompleted != null)
            {
                CellInfoUpdateCompleted();
            }
        }
        #endregion

        #region Getters
        /// <summary>
        /// Gets the number of lines the device has.
        /// </summary>
        public int NoOfLines
        {
            get
            {
                return noOfLines;
            }
        }

        /// <summary>
        /// Gets a dictionary of line ids to phone lines.
        /// </summary>
        public Dictionary<Guid,PhoneLine> AllPhoneLines
        {
            get
            {
                if (allPhoneLines == null)
                {
                    allPhoneLines = new Dictionary<Guid, PhoneLine>();
                }
                return allPhoneLines;
            }
        }

        /// <summary>
        /// Gets the current phone line.
        /// </summary>
        public PhoneLine CurrentPhoneLine
        {
            get
            {
                return currentPhoneLine;
            }
        }

        /// <summary>
        /// Gets the current phone line's SIM slot index.
        /// </summary>
        public int CurrentSIMSlotIndex
        {
            get
            {
                return currentSIMSlotIndex;
            }
        }

        /// <summary>
        /// Gets the current phone line's SIM state.
        /// </summary>
        public string CurrentSIMState
        {
            get
            {
                if (currentSIMState == null)
                {
                    currentSIMState = "Loading...";
                }
                return currentSIMState;
            }
        }

        /// <summary>
        /// Gets the current phone line's network state.
        /// </summary>
        public string CurrentNetworkState
        {
            get
            {
                if (currentNetworkState == null)
                {
                    currentNetworkState = "Loading...";
                }
                return currentNetworkState;
            }
        }

        /// <summary>
        /// Gets the current phone line's display name.
        /// </summary>
        public string CurrentDisplayName
        {
            get
            {
                if (currentDisplayName == null)
                {
                    currentDisplayName = "Loading...";
                }
                return currentDisplayName;
            }
        }

        /// <summary>
        /// Gets the current phone line's operator nbame.
        /// </summary>
        public string CurrentOperatorName
        {
            get
            {
                if (currentOperatorName == null)
                {
                    currentOperatorName = "Loading...";
                }
                return currentOperatorName;
            }
        }

        /// <summary>
        /// Gets the current phone line's display color.
        /// </summary>
        public Color CurrentDisplayColor
        {
            get
            {
                if (currentDisplayColor == null)
                {
                    currentDisplayColor = Color.FromArgb(0, 0, 0, 0);
                }
                return currentDisplayColor;
            }
        }

        /// <summary>
        /// Gets the current phone line's voicemail number.
        /// </summary>
        public string CurrentVoicemailNumber
        {
            get
            {
                if (currentVoicemailNumber == null)
                {
                    currentVoicemailNumber = "Loading...";
                }
                return currentVoicemailNumber;
            }
        }

        /// <summary>
        /// Gets the current phone line's voicemail count.
        /// </summary>
        public int CurrentVoicemailCount
        {
            get
            {
                return currentVoicemailCount;
            }
        }

        /// <summary>
        /// Gets the call active state.
        /// </summary>
        public bool DoesPhoneCallExist
        {
            get
            {
                return doesPhoneCallExist;
            }
        }
        #endregion
    }
}
