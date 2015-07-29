//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1_Identity.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel;

Scenario1::Scenario1()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

Platform::String^ Scenario1::VersionString(PackageVersion version)
{
    return version.Major.ToString() + "." + version.Minor.ToString() + "." +
           version.Build.ToString() + "." + version.Revision.ToString();
}

Platform::String^ Scenario1::ArchitectureString(Windows::System::ProcessorArchitecture architecture)
{
    switch (architecture)
    {
        case Windows::System::ProcessorArchitecture::X86:
            return "x86";
        case Windows::System::ProcessorArchitecture::Arm:
            return "arm";
        case Windows::System::ProcessorArchitecture::X64:
            return "x64";
        case Windows::System::ProcessorArchitecture::Neutral:
            return "neutral";
        case Windows::System::ProcessorArchitecture::Unknown:
            return "unknown";
        default:
            return "???";
    }
}

void Scenario1::GetPackage_Click(Object^ sender, RoutedEventArgs^ e)
{
    Package^ package = Package::Current;
    PackageId^ packageId = package->Id;

    String^ output = "Name: \"" + packageId->Name + "\"\n" +
                     "Version: " + VersionString(packageId->Version) + "\n" +
                     "Architecture: " + ArchitectureString(packageId->Architecture) + "\n" +
                     "ResourceId: \"" + packageId->ResourceId + "\"\n" +
                     "Publisher: \"" + packageId->Publisher + "\"\n" +
                     "PublisherId: \"" + packageId->PublisherId + "\"\n" +
                     "FullName: \"" + packageId->FullName + "\"\n" +
                     "FamilyName: \"" + packageId->FamilyName + "\"\n" +
                     "IsFramework: " + package->IsFramework.ToString() + "\n";
#if WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
    output += "IsResourcePackage: " + package->IsResourcePackage.ToString() + "\n" +
              "IsBundle: " + package->IsBundle.ToString() + "\"\n" +
              "IsDevelopmentMode: " + package->IsDevelopmentMode.ToString() + "\n" +
              "DisplayName: \"" + package->DisplayName + "\"\n" +
              "PublisherDisplayName: \"" + package->PublisherDisplayName + "\"\n" +
              "Description: \"" + package->Description + "\"\n" +
              "Logo: \"" + package->Logo->AbsoluteUri + "\"\n";
#endif

    OutputTextBlock->Text = output;
}
