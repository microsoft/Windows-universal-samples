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

namespace AllJoynConsumerExperiences
{
    public sealed partial class Scenario2 : Page
    {
        private Scenario2ViewModel m_scenario2ViewModel = null;

        public Scenario2()
        {
            InitializeComponent();
            m_scenario2ViewModel = new Scenario2ViewModel();
            DataContext = m_scenario2ViewModel;
        }

        // Cleanup scenario when it is about to be unloaded.
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (m_scenario2ViewModel != null)
            {
                m_scenario2ViewModel.ScenarioCleanup();
            }
        }
    }
}
