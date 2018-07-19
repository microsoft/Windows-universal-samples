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
using System.Runtime.InteropServices;
using System.Text;
using Windows.UI.Xaml;


namespace SDKTemplate
{
    public sealed partial class FirmwareAccess
    {
        MainPage rootPage = MainPage.Current;

        public FirmwareAccess()
        {
            this.InitializeComponent();
        }

        [DllImport("Kernel32", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern int GetFirmwareEnvironmentVariable(string name, string guid,
        [Out, MarshalAs(UnmanagedType.LPArray)] char[] lpszBuffer, int size);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern uint GetLastError();

        [DllImport("rpcclient.dll", CharSet = CharSet.Unicode)]
        extern static uint GetManufacturerNameFromSmbios(StringBuilder ManufacturerName, uint Size);

        [DllImport("rpcclient.dll", CharSet = CharSet.Unicode)]
        extern static uint GetSecureBootEnabledFromUefi([MarshalAs(UnmanagedType.U1)] ref bool SecureBootEnabled);

        const uint ERROR_SUCCESS = 0;
        const uint ERROR_INVALID_FUNCTION = 1;

        void GetManufacturerName_Click(Object sender, RoutedEventArgs e)
        {
            const int maxNameLength = 255;
            StringBuilder name = new StringBuilder(maxNameLength);
 
            uint error = GetManufacturerNameFromSmbios(name, (uint)name.Capacity);
            if (error == ERROR_SUCCESS)
            {
                ManufacturerName.Text = name.ToString();
            }
            else
            {
                rootPage.NotifyUser($"Failed to retrieve manufacturer name from SMBIOS, error {error}", NotifyType.ErrorMessage);
            }
        }

        void SecureBootState_Click(Object sender, RoutedEventArgs e)
        {
            bool secureBootEnabled = false;

            uint error = GetSecureBootEnabledFromUefi(ref secureBootEnabled);
            switch (error)
            {
                case ERROR_SUCCESS:
                SecureBootState.Text = secureBootEnabled.ToString();
                    break;

                case ERROR_INVALID_FUNCTION:
                    rootPage.NotifyUser("Windows was installed using legacy BIOS", NotifyType.ErrorMessage);
                    break;

                default:
                    rootPage.NotifyUser($"Failed to retrieve secure boot state from UEFI, error {error}", NotifyType.ErrorMessage);
                    break;
            }
        }
    }
}