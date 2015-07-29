using SDKTemplate;
using System;
using System.Collections.Generic;
using Windows.Devices.Enumeration;
using Windows.Devices.SmartCards;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Smartcard
{
    public class SmartCardListItem
    {
        public string ReaderName
        {
            get;
            set;
        }

        public string CardName
        {
            get;
            set;
        }

    }
    public sealed partial class Scenario7_ListAllCards : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario7_ListAllCards()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Click handler for the 'ListSmartCard' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ListSmartCard_Click(object sender, RoutedEventArgs e)
        {
            Button b = sender as Button;
            b.IsEnabled = false;
            try
            {
                rootPage.NotifyUser("Enumerating smart cards...", NotifyType.StatusMessage);

                // This list will be bound to our ItemListView once it has been
                // filled with SmartCardListItems.  The SmartCardListItem class
                // is defined above, and describes a reader/card pair with a
                // reader name and a card name.
                List<SmartCardListItem> cardItems = new List<SmartCardListItem>();

                // First we get the device selector for smart card readers using
                // the static GetDeviceSelector method of the SmartCardReader
                // class.  The selector is a string which describes a class of
                // devices to query for, and is used as the argument to
                // DeviceInformation.FindAllAsync.  GetDeviceSelector is
                // overloaded so that you can provide a SmartCardReaderKind
                // to specify if you are only interested in a particular type
                // of card/reader (e.g. TPM virtual smart card.)  In this case
                // we will list all cards and readers.
                string selector = SmartCardReader.GetDeviceSelector();
                DeviceInformationCollection devices = await DeviceInformation.FindAllAsync(selector);

                // DeviceInformation.FindAllAsync gives us a
                // DeviceInformationCollection, which is essentially a list
                // of DeviceInformation objects.  We must iterate through that
                // list and instantiate SmartCardReader objects from the
                // DeviceInformation objects.
                foreach (DeviceInformation device in devices)
                {
                    SmartCardReader reader = await SmartCardReader.FromIdAsync(device.Id);

                    // For each reader, we want to find all the cards associated
                    // with it.  Then we will create a SmartCardListItem for
                    // each (reader, card) pair.
                    IReadOnlyList<SmartCard> cards = await reader.FindAllCardsAsync();

                    foreach (SmartCard card in cards)
                    {
                        SmartCardProvisioning provisioning = await SmartCardProvisioning.FromSmartCardAsync(card);

                        SmartCardListItem item = new SmartCardListItem()
                        {
                            ReaderName = card.Reader.Name,
                            CardName = await provisioning.GetNameAsync()
                        };

                        cardItems.Add(item);
                    }
                }

                // Bind the source of ItemListView to our SmartCardListItem list.
                ItemListView.ItemsSource = cardItems;

                rootPage.NotifyUser("Enumerating smart cards completed.", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Enumerating smart cards failed with exception: " + ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                b.IsEnabled = true;
            }
        }
    }
}
