using AppUIBasics.Common;
using AppUIBasics.Data;
using System.Collections.Generic;
using System.Linq;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TreeViewPage : Page
    {
        TreeViewNode personalFolder;
        TreeViewNode personalFolder2;

        public TreeViewPage()
        {
            this.InitializeComponent();

            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {

                TreeViewNode workFolder = new TreeViewNode() { Content = "Work Documents" };
                workFolder.IsExpanded = true;

                workFolder.Children.Add(new TreeViewNode() { Content = "XYZ Functional Spec" });
                workFolder.Children.Add(new TreeViewNode() { Content = "Feature Schedule" });
                workFolder.Children.Add(new TreeViewNode() { Content = "Overall Project Plan" });
                workFolder.Children.Add(new TreeViewNode() { Content = "Feature Rsource Allocation" });

                TreeViewNode remodelFolder = new TreeViewNode() { Content = "Home Remodel" };
                remodelFolder.IsExpanded = true;

                remodelFolder.Children.Add(new TreeViewNode() { Content = "Contractor Contact Info" });
                remodelFolder.Children.Add(new TreeViewNode() { Content = "Paint Color Scheme" });
                remodelFolder.Children.Add(new TreeViewNode() { Content = "Flooring woodgrain type" });
                remodelFolder.Children.Add(new TreeViewNode() { Content = "Kitchen cabinet style" });

                personalFolder = new TreeViewNode() { Content = "Personal Documents" };
                personalFolder.IsExpanded = true;
                personalFolder.Children.Add(remodelFolder);

                personalFolder2 = new TreeViewNode() { Content = "Personal Documents" };
                personalFolder2.IsExpanded = true;
                personalFolder2.Children.Add(remodelFolder);

                sampleTreeView.RootNodes.Add(workFolder);
                sampleTreeView.RootNodes.Add(personalFolder);

                sampleTreeView2.RootNodes.Add(workFolder);
                sampleTreeView2.RootNodes.Add(personalFolder2);
            }

        }

        private void sampleTreeView_ItemInvoked(TreeView sender, TreeViewItemInvokedEventArgs args)
        {
            return;
        }
    }
}
