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
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using xBindSampleCS;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public string SAMPLE_NAME = "x:Bind Sample in C#";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario(){
                Title = "Basic Bindings",
                Description = "Demonstrates simple OneTime and OneWay bindings against a model. Buttons are provided to adjust the model which will cause the OneWay bindings against properties with notifications to update.",
                ClassType=typeof(BasicTests)
            },
           new Scenario() {
                Title = "Other Bindings",
                Description = "Demonstrates a more complex set of bindings including TwoWay, Converters, TargetNullValue, FallbackValue and bindings used in a variety of places in markup.",
                ClassType=typeof(OtherBindings)
            },
            new Scenario() {
                Title = "List & Templates",
                Description = "Demonstrates using bindings in data templates, consumed by a list and as the content template of a button. The templates are defined inline to the list, in the page's resources and in a seperate resource dictionary.",
                ClassType=typeof(ListsAndTemplates)
            },
            new Scenario() {
                Title = "List phasing using x:Phase",
                Description = "Demonstrates using x:Phase to achieve incremenatal rendering of the list items.",
                ClassType=typeof(PhasingTests)
            },
            new Scenario() {
                Title = "Event Binding",
                Description = "Demonstrates using x:Bind with Events",
                ClassType=typeof(EventTests)
            },
            new Scenario() {
                Title = "Function Binding",
                Description = "Demonstrates using x:Bind to bind to functions which is an easier way to do value conversions than using IValueConverter",
                ClassType=typeof(Functions)
    }
};
    }
    public class Scenario
    {
        public string Title { get; set; }
        public string Description { get; set; }
        public Type ClassType { get; set; }
    }
}

