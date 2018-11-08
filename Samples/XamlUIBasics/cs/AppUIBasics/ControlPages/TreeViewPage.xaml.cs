using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using mux = Microsoft.UI.Xaml.Controls;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TreeViewPage : Page
    {
        mux.TreeViewNode personalFolder;
        mux.TreeViewNode personalFolder2;
        private ObservableCollection<ExplorerItem> DataSource;

        public TreeViewPage()
        {
            this.InitializeComponent();
            this.DataContext = this;
            DataSource = GetData();          

            mux.TreeViewNode workFolder = new mux.TreeViewNode() { Content = "Work Documents" };
            workFolder.IsExpanded = true;

            workFolder.Children.Add(new mux.TreeViewNode() { Content = "XYZ Functional Spec" });
            workFolder.Children.Add(new mux.TreeViewNode() { Content = "Feature Schedule" });
            workFolder.Children.Add(new mux.TreeViewNode() { Content = "Overall Project Plan" });
            workFolder.Children.Add(new mux.TreeViewNode() { Content = "Feature Resources Allocation" });

            mux.TreeViewNode remodelFolder = new mux.TreeViewNode() { Content = "Home Remodel" };
            remodelFolder.IsExpanded = true;

            remodelFolder.Children.Add(new mux.TreeViewNode() { Content = "Contractor Contact Info" });
            remodelFolder.Children.Add(new mux.TreeViewNode() { Content = "Paint Color Scheme" });
            remodelFolder.Children.Add(new mux.TreeViewNode() { Content = "Flooring woodgrain type" });
            remodelFolder.Children.Add(new mux.TreeViewNode() { Content = "Kitchen cabinet style" });

            personalFolder = new mux.TreeViewNode() { Content = "Personal Documents" };
            personalFolder.IsExpanded = true;
            personalFolder.Children.Add(remodelFolder);

            personalFolder2 = new mux.TreeViewNode() { Content = "Personal Documents" };
            personalFolder2.IsExpanded = true;
            personalFolder2.Children.Add(remodelFolder);

            sampleTreeView.RootNodes.Add(workFolder);
            sampleTreeView.RootNodes.Add(personalFolder);

            sampleTreeView2.RootNodes.Add(workFolder);
            sampleTreeView2.RootNodes.Add(personalFolder2);

        }

        private void sampleTreeView_ItemInvoked(mux.TreeView sender, mux.TreeViewItemInvokedEventArgs args)
        {
            return;
        }
            
        
        private ObservableCollection<ExplorerItem> GetData()
        {
            var list = new ObservableCollection<ExplorerItem>();
            ExplorerItem folder1 = new ExplorerItem()
            {
                Name = "Work Documents",
                Type = ExplorerItem.ExplorerItemType.Folder,
                Children =
                {
                    new ExplorerItem()
                    {
                        Name = "Functional Specifications",
                        Type = ExplorerItem.ExplorerItemType.Folder,
                        Children =
                        {
                            new ExplorerItem()
                            {
                                Name = "TreeView spec",
                                Type = ExplorerItem.ExplorerItemType.File,
                              }
                        }
                    },
                    new ExplorerItem()
                    {
                        Name = "Feature Schedule",
                        Type = ExplorerItem.ExplorerItemType.File,
                    },
                    new ExplorerItem()
                    {
                        Name = "Overall Project Plan",
                        Type = ExplorerItem.ExplorerItemType.File,
                    },
                    new ExplorerItem()
                    {
                        Name = "Feature Resources Allocation",
                        Type = ExplorerItem.ExplorerItemType.File,
                    }
                }
            };
            ExplorerItem folder2 = new ExplorerItem()
            {
                Name = "Personal Folder",
                Type = ExplorerItem.ExplorerItemType.Folder,
                Children =
                        {
                            new ExplorerItem()
                            {
                                Name = "Home Remodel Folder",
                                Type = ExplorerItem.ExplorerItemType.Folder,
                                Children =
                                {
                                    new ExplorerItem()
                                    {
                                        Name = "Contractor Contact Info",
                                        Type = ExplorerItem.ExplorerItemType.File,
                                    },
                                    new ExplorerItem()
                                    {
                                        Name = "Paint Color Scheme",
                                        Type = ExplorerItem.ExplorerItemType.File,
                                    },
                                    new ExplorerItem()
                                    {
                                        Name = "Flooring Woodgrain type",
                                        Type = ExplorerItem.ExplorerItemType.File,
                                    },
                                    new ExplorerItem()
                                    {
                                        Name = "Kitchen Cabinet Style",
                                        Type = ExplorerItem.ExplorerItemType.File,
                                    }
                                }
                            }
                        }
            };

            list.Add(folder1);
            list.Add(folder2);
            return list;
        }

    }

    public class ExplorerItem : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        public enum ExplorerItemType { Folder, File };
        public String Name { get; set; }
        public ExplorerItemType Type { get; set; }
        private ObservableCollection<ExplorerItem> m_children;
        public ObservableCollection<ExplorerItem> Children
        {
            get
            {
                if (m_children == null)
                {
                    m_children = new ObservableCollection<ExplorerItem>();
                }
                return m_children;
            }
            set
            {
                m_children = value;
            }
        }

        private bool m_isExpanded;
        public bool IsExpanded
        {
            get { return m_isExpanded; }
            set
            {
                if (m_isExpanded != value)
                {
                    m_isExpanded = value;
                    NotifyPropertyChanged("IsExpanded");
                }
            }
        }

        private bool m_isSelected;
        public bool IsSelected
        {
            get { return m_isSelected; }

            set
            {
                if (m_isSelected != value)
                {
                    m_isSelected = value;
                    NotifyPropertyChanged("IsSelected");
                }
            }

        }

        private void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    class ExplorerItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate FolderTemplate { get; set; }
        public DataTemplate FileTemplate { get; set; }

        protected override DataTemplate SelectTemplateCore(object item)
        {
            var explorerItem = (ExplorerItem)item;
            return explorerItem.Type == ExplorerItem.ExplorerItemType.Folder ? FolderTemplate : FileTemplate;
        }
    }
}
