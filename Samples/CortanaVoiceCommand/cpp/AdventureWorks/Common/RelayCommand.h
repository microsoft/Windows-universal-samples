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
namespace AdventureWorks
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class RelayCommand sealed : [Windows::Foundation::Metadata::Default] Windows::UI::Xaml::Input::ICommand
    {
    public:
        virtual event Windows::Foundation::EventHandler<Platform::Object^>^ CanExecuteChanged;
        virtual bool CanExecute(Platform::Object^ parameter);
        virtual void Execute(Platform::Object^ parameter);
        virtual ~RelayCommand();

    internal:
        RelayCommand(std::function<bool(Platform::Object^)> canExecuteCallback,
            std::function<void(Platform::Object^)> executeCallback);
        void RaiseCanExecuteChanged();

    private:
        std::function<bool(Platform::Object^)> _canExecuteCallback;
        std::function<void(Platform::Object^)> _executeCallback;
    };
}

