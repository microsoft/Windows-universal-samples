using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Threading;
using Windows.UI;
using Windows.Storage.Pickers;
using Windows.Storage;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace xBindSampleCS
{
    public sealed partial class PhasingTests : Page
    {

        private xBindSampleModel.FileDataSource _dataSource;
        private xBindSampleModel.FileDataSource dataSource
        {
            get
            {
                if (_dataSource == null)
                {
                    _dataSource = new xBindSampleModel.FileDataSource();
                    _dataSource.CollectionChanged += DataSource_CollectionChanged;
                }
                return _dataSource;
            }
            set
            {
                if (value != _dataSource)
                {
                    _dataSource.CollectionChanged -= DataSource_CollectionChanged;
                    _dataSource = value;
                    _dataSource.CollectionChanged += DataSource_CollectionChanged;
                }
            }
        }


        private bool Initialized = false;

        public PhasingTests()
        {
            this.InitializeComponent();
            CreateTestData();
            Initialized = true;
        }

        async void CreateTestData()
        {
            await dataSource.SetupDataSourceUsingPicturesFolder();
            LoadingPanel.Visibility = Visibility.Collapsed;
            myGridView.ItemsSource = dataSource;
        }

        private void DataSource_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            NoItemsPanel.Visibility = (dataSource.Count == 0) ? Visibility.Visible : Visibility.Collapsed;
        }

        private void Reset_Click(object sender, RoutedEventArgs e)
        {
            dataSource.ResetCollection();
        }

        private async void ChangeFolderClick(object sender, RoutedEventArgs e)
        {
            Windows.Storage.Pickers.FolderPicker picker = new Windows.Storage.Pickers.FolderPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.PicturesLibrary;
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".png");
            StorageFolder f = await picker.PickSingleFolderAsync();
            dataSource = new xBindSampleModel.FileDataSource();
            myGridView.ItemsSource = dataSource;
            LoadingPanel.Visibility = Visibility.Visible;
            await dataSource.SetupDataSource(f);
            LoadingPanel.Visibility = Visibility.Collapsed;
        }

        private void myGridView_ContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            Sleep(1);
            if (args.Phase < 10) args.RegisterUpdateCallback(myGridView_ContainerContentChanging);
        }

        private void Sleep(int msTime)
        {
            AutoResetEvent h = new AutoResetEvent(false);
            h.WaitOne(msTime);
        }

        private void SlowPhasing_UnChecked(object sender, RoutedEventArgs e)
        {
            SlowPhasing_Checked(sender, e);
        }

        private void SlowPhasing_Checked(object sender, RoutedEventArgs e)
        {
            if (Initialized)
            {
                if (SlowPhasing.IsChecked.Value)
                {
                    myGridView.ContainerContentChanging += myGridView_ContainerContentChanging;
                }
                else
                {
                    myGridView.ContainerContentChanging -= myGridView_ContainerContentChanging;
                }
            }
        }


        private void PhasedTemplate_Checked(object sender, RoutedEventArgs e)
        {
            if (Initialized)
            {
                if (RadioPhasedTempl.IsChecked == true)
                {
                    myGridView.ItemTemplate = (DataTemplate)Resources["PhasedFileTemplate"];
                }
                else if (RadioxBindTempl.IsChecked.Value)
                {
                    myGridView.ItemTemplate = (DataTemplate)Resources["NonPhasedFileTemplate"];
                }
                else if (RadioClassicTempl.IsChecked.Value)
                {
                    myGridView.ItemTemplate = (DataTemplate)Resources["ClassicBindingFileTemplate"];
                }
            }
        }

    }
}
