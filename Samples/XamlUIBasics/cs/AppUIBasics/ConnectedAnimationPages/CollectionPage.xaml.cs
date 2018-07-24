using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;
using AppUIBasics.ControlPages;
using Windows.Foundation.Metadata;

namespace AppUIBasics.ConnectedAnimationPages
{
    public sealed partial class CollectionPage : Page
    {
        CustomDataObject _storeditem;

        public CollectionPage()
        {
            this.InitializeComponent();

            // Ensure that the MainPage is only created once, and cached during navigation.
            this.NavigationCacheMode = NavigationCacheMode.Enabled;

            collection.ItemsSource = AppUIBasics.ControlPages.CustomDataObject.GetDataObjects();
        }

        private async void collection_Loaded(object sender, RoutedEventArgs e)
        {
            if (_storeditem != null)
            {
                // If the connected item appears outside the viewport, scroll it into view.
                collection.ScrollIntoView(_storeditem, ScrollIntoViewAlignment.Default);
                collection.UpdateLayout();

                // Play the second connected animation. 
                ConnectedAnimation animation = ConnectedAnimationService.GetForCurrentView().GetAnimation("BackConnectedAnimation");
                if (animation != null)
                {
                    // Setup the "back" configuration if the API is present. 
                    if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
                    {
                        animation.Configuration = new DirectConnectedAnimationConfiguration();
                    }

                    await collection.TryStartConnectedAnimationAsync(animation, _storeditem, "connectedElement");
                }
            }
        }

        private void collection_ItemClick(object sender, ItemClickEventArgs e)
        {
            // Get the collection item corresponding to the clicked item.
            var container = collection.ContainerFromItem(e.ClickedItem) as ListViewItem;
            if (container != null)
            {
                // Stash the clicked item for use later. We'll need it when we connect back from the detailpage.
                _storeditem = container.Content as CustomDataObject;

                // Prepare the connected animation.
                // Notice that the stored item is passed in, as well as the name of the connected element. 
                // The animation will actually start on the Detailed info page.
                var animation = collection.PrepareConnectedAnimation("ForwardConnectedAnimation", _storeditem, "connectedElement");

            }

            // Navigate to the DetailedInfoPage.
            // Note that we suppress the default animation. 
            Frame.Navigate(typeof(DetailedInfoPage), _storeditem, new SuppressNavigationTransitionInfo());
        }

    }
}
