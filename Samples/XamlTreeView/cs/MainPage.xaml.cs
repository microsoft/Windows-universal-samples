using System;
using TreeViewControl;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            TreeNode workFolder = CreateFolderNode("Work Documents");
            workFolder.Add(CreateFileNode("Feature Functional Spec"));
            workFolder.Add(CreateFileNode("Feature Schedule"));
            workFolder.Add(CreateFileNode("Overall Project Plan"));
            workFolder.Add(CreateFileNode("Feature Resource allocation"));
            sampleTreeView.RootNode.Add(workFolder);

            TreeNode remodelFolder = CreateFolderNode("Home Remodel");
            remodelFolder.IsExpanded = true;
            remodelFolder.Add(CreateFileNode("Contactor Contact Information"));
            remodelFolder.Add(CreateFileNode("Paint Color Scheme"));
            remodelFolder.Add(CreateFileNode("Flooring woodgrain types"));
            remodelFolder.Add(CreateFileNode("Kitchen cabinet styles"));

            TreeNode personalFolder = CreateFolderNode("Personal Documents");
            personalFolder.IsExpanded = true;
            personalFolder.Add(remodelFolder);

            sampleTreeView.RootNode.Add(personalFolder);

            sampleTreeView.ContainerContentChanging += SampleTreeView_ContainerContentChanging;
        }

        private void SampleTreeView_ContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            var node = args.Item as TreeNode;
            if (node != null)
            {
                var data = node.Data as FileSystemData;
                if (data != null)
                {
                    args.ItemContainer.AllowDrop = data.IsFolder;
                }
            }
        }

        private static TreeNode CreateFileNode(string name)
        {
            return new TreeNode() { Data = new FileSystemData(name) };
        }

        private static TreeNode CreateFolderNode(string name)
        {
            return new TreeNode() { Data = new FileSystemData(name) { IsFolder = true } };
        }
   }
}