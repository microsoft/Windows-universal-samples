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
#include "App.xaml.h"

using namespace Platform;
using namespace Simple3DGameXaml;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;


//----------------------------------------------------------------------

App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
    Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);

#if defined(_DEBUG)
    UnhandledException += ref new UnhandledExceptionEventHandler([](Object^ /* sender */, UnhandledExceptionEventArgs^ args)
    {
        String^ error = "Simple3DGameXaml::App::UnhandledException: " + args->Message + "\n";
        OutputDebugStringW(error->Data());
    });
#endif

    RequiresPointerMode = ApplicationRequiresPointerMode::WhenRequested;
}

//----------------------------------------------------------------------

void App::OnLaunched(_In_ LaunchActivatedEventArgs^ /* args */)
{
    m_mainPage = ref new DirectXPage();

    Window::Current->Content = m_mainPage;
    Window::Current->Activate();
}

//----------------------------------------------------------------------

void App::OnSuspending(
    _In_ Object^ /* sender */,
    _In_ SuspendingEventArgs^ args
    )
{
    m_mainPage->OnSuspending();
}

//----------------------------------------------------------------------

void App::OnResuming(
    _In_ Object^ /* sender */,
    _In_ Object^ /* args */
    )
{
    m_mainPage->OnResuming();
}

//----------------------------------------------------------------------