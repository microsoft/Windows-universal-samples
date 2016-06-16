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
using System.Collections.Concurrent;
using System.Threading.Tasks;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;

namespace SDKTemplate
{
    class IsApiPropertyPresentTrigger : StateTriggerBase
    {
        // IsPresent defaults to true. The others default to null.
        private string _typeName;
        private string _propertyName;
        private bool _isPresent = true;

        // Create a process-wide cache of property presence queries.
        private static ConcurrentDictionary<Tuple<string, string>, bool> propertyPresence =
            new ConcurrentDictionary<Tuple<string, string>, bool>();

        private bool _isPropertyPresent;

        public string TypeName
        {
            get { return _typeName; }
            set { _typeName = value; UpdatePresence(); }
        }

        public string PropertyName
        {
            get { return _propertyName; }
            set { _propertyName = value; UpdatePresence(); }
        }

        public bool IsPresent
        {
            get { return _isPresent; }
            set { _isPresent = value; UpdateState(); }
        }

        void UpdatePresence()
        {
            if (string.IsNullOrEmpty(_typeName) || string.IsNullOrEmpty(_propertyName))
            {
                // State trigger not yet fully initialized.
                return;
            }

            // If the answer is not already cached, then call into
            // ApiInformation method to determine whether property is present.
            _isPropertyPresent = propertyPresence.GetOrAdd(Tuple.Create(_typeName, _propertyName),
               (key) => ApiInformation.IsPropertyPresent(key.Item1, key.Item2));

            UpdateState();
        }

        void UpdateState()
        {
            // If the property presence matches _isPresent then the trigger will be activated.
            SetActive(_isPresent == _isPropertyPresent);
        }
    }
}
