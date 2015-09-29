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

using System.Collections.ObjectModel;
using Windows.UI.Xaml.Controls;
using System;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace DragAndDropSampleManaged
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_StartDragAsync : Page
    {
        Random _rnd = new Random();
        // Collection of symbols to display and drag
        ObservableCollection<string> _symbols;
        // Is a round running
        bool _isActive;
        // Symbol to drag
        string _symbol;
        // Timer limiting the time allowed to drag the symbol to the right target
        DispatcherTimer _timer;
        // Keeping the Drag operation will allow to cancel it after it has started
        IAsyncOperation<DataPackageOperation> _dragOperation;

        public Scenario3_StartDragAsync()
        {
            this.InitializeComponent();
            this.Loaded += Scenario3_StartDragAsync_Loaded;
        }

        private async void Scenario3_StartDragAsync_Loaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Initialize the symbols from the Text file
            var uri = new Uri("ms-appx:///Assets/Symbols.txt", UriKind.RelativeOrAbsolute);
            var file = await StorageFile.GetFileFromApplicationUriAsync(uri);
            var lines = await FileIO.ReadLinesAsync(file);
            _symbols = new ObservableCollection<string>(lines);
            DropGridView.ItemsSource = _symbols;
        }

        /// <summary>
        /// Start button clicked: select a symbol, update the display
        /// and start the timer according to the difficulty level
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StartButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (!_isActive)
            {
                int index = _rnd.Next(_symbols.Count);
                _symbol = _symbols[index];
                SourceTextBlock.Text = _symbol;
                ResultTextBlock.Text = string.Empty;
                _isActive = true;

                if (_timer == null)
                {
                    _timer = new DispatcherTimer();
                    _timer.Tick += OnTick;
                }
                bool easy = (EasyRB.IsChecked != null) && EasyRB.IsChecked.Value;
                _timer.Interval = TimeSpan.FromSeconds(easy ? 5.0 : 1.5);
                _timer.Start();
                StartButton.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                SourceTextBlock.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
        }

        /// <summary>
        ///  End of the timer: player lost and we might have to cancel the drag operation
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void OnTick(object sender, object args)
        {
            _timer.Stop();
            if (_dragOperation != null)
            {
                // Cancel the drag operation: this is equivalent of the user pressing Escape
                // or releasing the pointer on a non-target
                // DropCompleted event would return DataPackageOperation.None as the result
                _dragOperation.Cancel();
            }
            else
            {
                // We test _isActive just in case the timer expired when we were handling the drop
                if (_isActive)
                {
                    EndRound(false);
                }
            }
        }

        /// <summary>
        /// If the pointer is moved and we are not yet dragging, start a drag operation
        /// using StartDragAsync, which will allow a later cancellation
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SourceTextBlock_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (_isActive && e.Pointer.IsInContact && (_dragOperation == null))
            {
                _dragOperation = SourceTextBlock.StartDragAsync(e.GetCurrentPoint(SourceTextBlock));
                _dragOperation.Completed = DragCompleted;
            }
        }

        /// <summary>
        /// Update the user interface when the round is finished
        /// </summary>
        /// <param name="hasWon"></param>
        private void EndRound(bool hasWon)
        {
            _isActive = false;
            ResultTextBlock.Text = (hasWon ? "You win :-) !!" : "You lose :-( !!");
            StartButton.Visibility = Windows.UI.Xaml.Visibility.Visible;
            SourceTextBlock.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        /// <summary>
        /// Completion callback for the asynchronous call:
        /// it is called whether the drop succeeded, or the user released the pointer on a non-target
        /// or else we cancelled the operation from the timer's callback
        /// </summary>
        /// <param name="asyncInfo"></param>
        /// <param name="asyncStatus"></param>
        private void DragCompleted(IAsyncOperation<DataPackageOperation> asyncInfo, AsyncStatus asyncStatus)
        {
            _dragOperation = null;
            if (_timer != null) _timer.Stop();
            EndRound((asyncStatus == AsyncStatus.Completed) && (asyncInfo.GetResults() == DataPackageOperation.Copy));
        }

        /// <summary>
        /// DragStarting is called even if we are starting the Drag Operation ourselved with StartDragAsync
        /// In this sample, we don't really need to handle it
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SourceTextBlock_DragStarting(Windows.UI.Xaml.UIElement sender, Windows.UI.Xaml.DragStartingEventArgs e)
        {
            e.Data.RequestedOperation = DataPackageOperation.Copy;
            e.Data.SetText(_symbol);
        }

        /// <summary>
        /// Check if the sender is a border AND has the dragged symbol
        /// </summary>
        /// <param name="sender"></param>
        /// <returns></returns>
        private bool IsSymbolTargetBorder(object sender)
        {
            var border = sender as Border;
            return (border != null) && (border.DataContext as string == _symbol);
        }

        /// <summary>
        /// Entering a target: check if this will result in a win
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DropBorder_DragEnter(object sender, Windows.UI.Xaml.DragEventArgs e)
        {
            bool win = (_dragOperation != null) && e.DataView.Contains(StandardDataFormats.Text) && IsSymbolTargetBorder(sender);
            e.AcceptedOperation = win ? DataPackageOperation.Copy : DataPackageOperation.None;
            e.DragUIOverride.IsCaptionVisible = false;
        }

        /// <summary>
        /// Drop: check if this results in a win
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DropBorder_Drop(object sender, Windows.UI.Xaml.DragEventArgs e)
        {
            bool win = (_dragOperation != null) && e.DataView.Contains(StandardDataFormats.Text) && IsSymbolTargetBorder(sender);
            e.AcceptedOperation = win ? DataPackageOperation.Copy : DataPackageOperation.None;
        }
    }
}
