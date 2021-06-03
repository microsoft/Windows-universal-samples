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

#pragma once

#include "UserViewModel.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct UserViewModel : UserViewModelT<UserViewModel>
    {
        UserViewModel(hstring userId, hstring displayName) :
            m_userId(std::move(userId)),
            m_displayName(std::move(displayName))
        {
        }

        hstring UserId()
        {
            return m_userId;
        }

        void UserId(hstring userId)
        {
            m_userId = std::move(userId);
            RaisePropertyChanged(L"UserId");
        }

        hstring DisplayName()
        {
            return m_displayName;
        }

        void DisplayName(hstring displayName)
        {
            m_displayName = std::move(displayName);
            RaisePropertyChanged(L"DisplayName");
        }

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }

        void PropertyChanged(event_token token)
        {
            m_propertyChanged.remove(token);
        }

    private:
        void RaisePropertyChanged(hstring propertyName)
        {
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
        }

    private:
        hstring m_userId;
        hstring m_displayName;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct UserViewModel : UserViewModelT<UserViewModel, implementation::UserViewModel>
    {
    };
}