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
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario0_Choose : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario0_Choose()
        {
            this.InitializeComponent();
            var readingTypes = new List<SensorReadingType>()
            {
                SensorReadingType.Absolute,
                SensorReadingType.Relative,
            };
            ReadingTypeComboBox.ItemsSource = readingTypes;
            ReadingTypeComboBox.SelectedIndex = readingTypes.IndexOf(rootPage.SensorReadingType);

            var optimizationGoals = new List<SensorOptimizationGoal>()
            {
                SensorOptimizationGoal.Precision,
                SensorOptimizationGoal.PowerEfficiency,
            };
            OptimizationGoalComboBox.ItemsSource = optimizationGoals;
            OptimizationGoalComboBox.SelectedIndex = optimizationGoals.IndexOf(rootPage.SensorOptimizationGoal);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            rootPage.SensorReadingType = (SensorReadingType)ReadingTypeComboBox.SelectedValue;
            rootPage.SensorOptimizationGoal = (SensorOptimizationGoal)OptimizationGoalComboBox.SelectedValue;
        }
    }
}
