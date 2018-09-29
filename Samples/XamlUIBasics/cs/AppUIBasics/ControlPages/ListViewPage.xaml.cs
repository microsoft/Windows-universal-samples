//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Data;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics.ControlPages
{
    public sealed partial class ListViewPage : ItemsPageBase
    {
        public ListViewPage()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            Items = ControlInfoDataSource.Instance.Groups.Take(3).SelectMany(g => g.Items).ToList();
            Control4.ItemsSource = AppUIBasics.ControlPages.CustomDataObject.GetDataObjects();
            ContactsCVS.Source = await Contact.GetContactsGroupedAsync();
        }

        private void SelectionModeComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Control2 != null)
            {
                string colorName = e.AddedItems[0].ToString();
                switch (colorName)
                {
                    case "None":
                        Control2.SelectionMode = ListViewSelectionMode.None;
                        break;
                    case "Single":
                        Control2.SelectionMode = ListViewSelectionMode.Single;
                        break;
                    case "Multiple":
                        Control2.SelectionMode = ListViewSelectionMode.Multiple;
                        break;
                    case "Extended":
                        Control2.SelectionMode = ListViewSelectionMode.Extended;
                        break;
                }
            }
        }
    }

    public class Contact
    {
        #region Properties
        public string FirstName { get; private set; }
        public string LastName { get; private set; }
        public string Company { get; private set; }
        public string Name => FirstName + " " + LastName;
        #endregion

        public Contact(string firstName, string lastName, string company)
        {
            FirstName = firstName;
            LastName = lastName;
            Company = company;
        }

        #region Public Methods
        public static async Task<ObservableCollection<Contact>> GetContactsAsync()
        {
            StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/Contacts.txt"));
            IList<string> lines = await FileIO.ReadLinesAsync(file);

            var contacts = new ObservableCollection<Contact>();

            for (int i = 0; i < lines.Count; i += 3)
            {
                contacts.Add(new Contact(lines[i], lines[i + 1], lines[i + 2]));
            }

            return contacts;
        }

        public static async Task<ObservableCollection<GroupInfoList>> GetContactsGroupedAsync()
        {
            var query = from item in await GetContactsAsync()
                        group item by item.LastName.Substring(0, 1).ToUpper() into g
                        orderby g.Key
                        select new GroupInfoList(g) { Key = g.Key };

            return new ObservableCollection<GroupInfoList>(query);
        }

        public override string ToString()
        {
            return Name;
        }
        #endregion
    }

    public class GroupInfoList : List<object>
    {
        public GroupInfoList(IEnumerable<object> items) : base(items)
        {
        }
        public object Key { get; set; }
    }
}
