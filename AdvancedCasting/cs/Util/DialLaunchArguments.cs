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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScreenCasting.Util
{
    class DialLaunchArguments
    {
        public string VideoId { get; private set; }
        public string PairingCode { get; private set; }
        public TimeSpan Position { get; private set; }

        private DialLaunchArguments() { }

        public static DialLaunchArguments Parse(string arguments)
        {
            try
            {
                DialLaunchArguments dialLaunchArgs = new DialLaunchArguments();

                string[] argswithkeys = arguments.Split('&');

                foreach (string currentArgWithKey in argswithkeys)
                {
                    string key = currentArgWithKey.Split('=')[0];
                    string val = currentArgWithKey.Split('=')[1];
                    switch (key)
                    {
                        case "v": { dialLaunchArgs.VideoId = val; break; }
                        case "t": { dialLaunchArgs.Position = TimeSpan.FromTicks(long.Parse(val)); break; }
                        case "pairingCode": { dialLaunchArgs.PairingCode = val; break; }
                    }
                }
                return dialLaunchArgs;
            }
            catch (Exception ex){
                throw new ArgumentOutOfRangeException(string.Format("Failed to parse DIAL launch arguments: '{0}'", arguments), ex);
                }
        }


    }


}
