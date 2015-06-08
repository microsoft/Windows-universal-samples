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

namespace AdventureWorks.Common
{
    /// <summary>
    /// Navigation service interface, allows a test implementation to be substituted in its place.
    /// </summary>
    public interface INavigationService
    {
        bool Navigate<T>(object parameter = null);

        bool Navigate(Type source, object parameter = null);

        void GoBack();
    }
}