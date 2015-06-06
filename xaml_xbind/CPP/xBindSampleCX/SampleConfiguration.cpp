// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace xBindSampleCX;

Platform::Collections::Vector<Scenario^>^ MainPage::scenariosInner = ref new Platform::Collections::Vector<Scenario^>(
    ref new Platform::Array<Scenario^>
{
    //{ "Add Scenario 1 here", "xBindSampleCX.Scenario1_ShortName" },
    //{ "Add Scenario 2 here", "xBindSampleCX.Scenario2_ShortName" },
    //{ "Add Scenario 3 here", "xBindSampleCX.Scenario3_ShortName" }
    ref new Scenario(
        "Basic Bindings",
        "Demonstrates simple OneTime and OneWay bindings against a model. Buttons are provided to adjust the model which will cause the OneWay bindings against properties with notifications to update.",
        "xBindSampleCX.BasicTests"
    ),
    ref new Scenario(
        "Other Bindings",
        "Demonstrates a more complex set of bindings including TwoWay, Converters, TargetNullValue, FallbackValue and bindings used in a variety of places in markup.",
        "xBindSampleCX.OtherBindings"
    ),
        ref new Scenario(
        "List & Templates",
        "Demonstrates using bindings in data templates, consumed by a list and as the content template of a button. The templates are defined inline to the list, in the page's resources and in a seperate resource dictionary.",
        "xBindSampleCX.ListsAndTemplates"
    ),
    ref new Scenario(
        "List phasing using x:Phase",
        "Demonstrates using x:Phase to achieve incremenatal rendering of the list items.",
        "xBindSampleCX.PhasingTests"
    ),
    ref new Scenario(
        "Event Binding",
        "Demonstrates using x:Bind with Events",
        "xBindSampleCX.EventTests"
        )
});

Scenario::Scenario(Platform::String^ title, Platform::String^ description, Platform::String^ className)
{
    this->Title = title;
    this->Description = description;
    this->ClassName = className;
}