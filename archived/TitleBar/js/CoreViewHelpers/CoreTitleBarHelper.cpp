#include "pch.h"
#include "CoreTitleBarHelper.h"

using namespace CoreViewHelpers;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

String^ CoreTitleBarHelper::KeyName = "CoreViewHelpers.CoreTitleBarHelper";

CoreTitleBarHelper::CoreTitleBarHelper()
{
    isVisibleChangedToken = titleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(this, &CoreTitleBarHelper::OnIsVisibleChanged);
    layoutMetricsChangedToken = titleBar->LayoutMetricsChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(this, &CoreTitleBarHelper::OnLayoutMetricsChanged);
}

CoreTitleBarHelper::~CoreTitleBarHelper()
{
    titleBar->IsVisibleChanged -= isVisibleChangedToken;
    titleBar->LayoutMetricsChanged -= layoutMetricsChangedToken;
}

void CoreTitleBarHelper::OnIsVisibleChanged(CoreApplicationViewTitleBar^ sender, Platform::Object^ args)
{
    IsVisibleChanged(this, args);
}

void CoreTitleBarHelper::OnLayoutMetricsChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args)
{
    LayoutMetricsChanged(this, args);
}

CoreTitleBarHelper^ CoreTitleBarHelper::GetForCurrentView()
{
    CoreWindow^ window = CoreWindow::GetForCurrentThread();

    Object^ titleBarHelper = nullptr;
    if (window->CustomProperties->HasKey(KeyName))
    {
        titleBarHelper = window->CustomProperties->Lookup(KeyName);
    }
    else
    {
        titleBarHelper = ref new CoreTitleBarHelper();
        window->CustomProperties->Insert(KeyName, titleBarHelper);
    }
    return dynamic_cast<CoreTitleBarHelper^>(titleBarHelper);

}