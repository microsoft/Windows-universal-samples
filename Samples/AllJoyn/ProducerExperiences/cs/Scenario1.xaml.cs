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

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace AllJoynProducerExperiences
{
    public sealed partial class Scenario1 : Page
    {
        private Scenario1ViewModel m_scenario1ViewModel = null;

        public Scenario1()
        {
            InitializeComponent();
            m_scenario1ViewModel = new Scenario1ViewModel();
            DataContext = m_scenario1ViewModel;
        }

        // Cleanup scenario when it is about to be unloaded.
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (m_scenario1ViewModel != null)
            {
                m_scenario1ViewModel.ScenarioCleanup();
            }
        }
    }
}
