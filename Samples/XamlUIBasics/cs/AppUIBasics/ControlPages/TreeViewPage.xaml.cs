using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

//From ControlInfoData.json:
// {
//    "UniqueId": "TreeView",
//   "Title": "TreeView",
//   "Subtitle": "A hierarchical list with expanding and collapsing nodes that contain nested items.",
//   "ImagePath": "ms-appx:///Assets/TreeView.png",
//   "Description": "A hierarchical list with expanding and collapsing nodes that contain nested items.",
//   "Content": "",
//   "IsNew": true,
//   "Docs": [
//					{
//						"Title": "TreeView",
//						"Uri": "https://docs.microsoft.com/uwp/api/Windows.UI.Xaml.Controls.TreeView"
//					}
//   ],
//   "RelatedControls": [
//     "ListView",
//     "GridView"
//   ]
// }

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TreeViewPage : Page
    {
        TreeViewNode personalFolder;
        public TreeViewPage()
        {
            this.InitializeComponent();

            TreeViewNode workFolder = new TreeViewNode() { Data = "Folder1" };
            workFolder.Add(new TreeViewNode() { Data = "1.1" });
            workFolder.Add(new TreeViewNode() { Data = "1.2" });
            workFolder.Add(new TreeViewNode() { Data = "1.3" });

            TreeViewNode remodelFolder = new TreeViewNode() { Data = "Folder2" };
            remodelFolder.Add(new TreeViewNode() { Data = "2.1" });
            remodelFolder.Add(new TreeViewNode() { Data = "2.2" });
            remodelFolder.Add(new TreeViewNode() { Data = "2.3" });

            personalFolder = new TreeViewNode() { Data = "TopLevel", IsExpanded = true };

            personalFolder.Add(workFolder);
            personalFolder.Add(remodelFolder);
            sampleTreeView.RootNode.Add(personalFolder);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (sampleTreeView.ListControl.SelectedItem == null)
            {
                sampleTreeView.RootNode.Add(new TreeViewNode() { Data = "Inserted node" });
            }
            else
            {
                TreeViewNode selectedNode = ((TreeViewNode)sampleTreeView.ListControl.SelectedItem);
                int index = selectedNode.ParentNode.IndexOf(selectedNode);
                selectedNode.ParentNode.Insert(index + 1, (new TreeViewNode() { Data = "Inserted node" }));
            }
        }
    }
}
