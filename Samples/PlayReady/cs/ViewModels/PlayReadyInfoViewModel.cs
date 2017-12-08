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
using Windows.ApplicationModel.Core;
using Windows.Media.Protection.PlayReady;
using Windows.UI.Core;

namespace SDKTemplate.ViewModels
{
    public class PlayReadyInfoViewModel : ViewModelBase
    {
        
        public async void RefreshStatics()
        {
            var dispatcher = CoreApplication.MainView.CoreWindow.Dispatcher;
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                try
                {
                    SecurityLevel = PlayReadyStatics.PlayReadyCertificateSecurityLevel;
                    PlayReadySecurityVersion = PlayReadyStatics.PlayReadySecurityVersion;
                    HasHardwareDRM = PlayReadyStatics.CheckSupportedHardware(PlayReadyHardwareDRMFeatures.HardwareDRM);
                    HasHEVCSupport = PlayReadyStatics.CheckSupportedHardware(PlayReadyHardwareDRMFeatures.HEVC);
                }
                catch
                {
                    ViewModelBase.Log("PlayReadyStatics not yet available");
                }
            });
            

        }

        private uint playReadySecurityVersion;

        public uint PlayReadySecurityVersion
        {
            get { return playReadySecurityVersion; }
            private set
            {
                if (playReadySecurityVersion != value)
                {
                    playReadySecurityVersion = value;
                    RaisePropertyChanged();
                }
            }
        }


        private uint securityLevel;
        public uint SecurityLevel
        {
            get
            {
                return securityLevel;
            }
            private set
            {
                if (securityLevel != value)
                {
                    securityLevel = value;
                    RaisePropertyChanged();
                }
            }
        }

        private bool hasHEVCSupport;
        public bool HasHEVCSupport
        {
            get
            {
                return hasHEVCSupport;
            }
            private set
            {
                if (hasHEVCSupport != value)
                {
                    hasHEVCSupport = value;
                    RaisePropertyChanged();
                }
            }
        }

        private bool hasHardwareDRM;
        public bool HasHardwareDRM
        {
            get
            {
                return hasHardwareDRM;
            }
            private set
            {
                if (hasHardwareDRM != value)
                {
                    hasHardwareDRM = value;
                    RaisePropertyChanged();
                }
            }
        }

    }
}
