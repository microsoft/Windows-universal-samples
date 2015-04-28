//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DatePickerPage : Page
    {
        public DatePickerPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Set the default date to 2 months from the current date.
            Control2.Date = DateTimeOffset.Now.AddMonths(2);

            // Set the minimum year to the current year.
            Control2.MinYear = DateTimeOffset.Now;

            // Set the maximum year to 5 years in the future.
            Control2.MaxYear = DateTimeOffset.Now.AddYears(5);

        }
    }
}
