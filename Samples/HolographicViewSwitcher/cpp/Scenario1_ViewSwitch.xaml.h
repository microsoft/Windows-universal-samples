//
// Scenario1_ViewSwitch.xaml.h
// Declaration of the Scenario1_ViewSwitch class
//

#pragma once

#include "Scenario1_ViewSwitch.g.h"

namespace SDKTemplate
{
  /// <summary>
  /// A basic page holding a button to launch a HolographicSpace
  /// </summary>
  [Windows::Foundation::Metadata::WebHostHidden]
  public ref class Scenario1_ViewSwitch sealed
  {
  public:
    Scenario1_ViewSwitch();
  private:
    void SwitchButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

  private:
    Windows::ApplicationModel::Core::CoreApplicationView^ m_exclusiveView = nullptr;
  };
}
