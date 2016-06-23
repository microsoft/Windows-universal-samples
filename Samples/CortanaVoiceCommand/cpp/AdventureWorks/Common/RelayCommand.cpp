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
#include "pch.h"
#include "RelayCommand.h"

using namespace AdventureWorks;

using namespace Windows::Foundation;

/// <summary>
/// Determines whether this <see cref="RelayCommand"/> can execute in its current state.
/// </summary>
/// <param name="parameter">
/// Data used by the command. If the command does not require data to be passed, this object can be set to null.
/// </param>
/// <returns>true if this command can be executed; otherwise, false.</returns>
bool RelayCommand::CanExecute(Object^ parameter)
{
    return (_canExecuteCallback)(parameter);
}

/// <summary>
/// Executes the <see cref="RelayCommand"/> on the current command target.
/// </summary>
/// <param name="parameter">
/// Data used by the command. If the command does not require data to be passed, this object can be set to null.
/// </param>
void RelayCommand::Execute(Object^ parameter)
{
    (_executeCallback)(parameter);
}

/// <summary>
/// Method used to raise the <see cref="CanExecuteChanged"/> event
/// to indicate that the return value of the <see cref="CanExecute"/>
/// method has changed.
/// </summary>
void RelayCommand::RaiseCanExecuteChanged()
{
    CanExecuteChanged(this, nullptr);
}

/// <summary>
/// RelayCommand Class Destructor.
/// </summary>
RelayCommand::~RelayCommand()
{
    _canExecuteCallback = nullptr;
    _executeCallback = nullptr;
};

/// <summary>
/// Creates a new command that can always execute.
/// </summary>
/// <param name="canExecuteCallback">The execution status logic.</param>
/// <param name="executeCallback">The execution logic.</param>
RelayCommand::RelayCommand(std::function<bool(Platform::Object^)> canExecuteCallback,
    std::function<void(Platform::Object^)> executeCallback) :
    _canExecuteCallback(canExecuteCallback),
    _executeCallback(executeCallback)
{
}
