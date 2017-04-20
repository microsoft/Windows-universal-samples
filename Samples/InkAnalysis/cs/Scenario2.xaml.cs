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
using System;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Input.Inking;
using Windows.UI.Input.Inking.Analysis;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

namespace SDKTemplate
{
    /// <summary>
    /// This scenario demostrates text recognition with InkAnalyzer.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage = MainPage.Current;
        InkPresenter inkPresenter;
        InkAnalyzer inkAnalyzer;
        InkAnalysisParagraph paragraphSelected;
        DispatcherTimer dispatcherTimer;

        public Scenario2()
        {
            this.InitializeComponent();

            inkPresenter = inkCanvas.InkPresenter;
            inkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
            inkPresenter.StrokesErased += InkPresenter_StrokesErased;
            inkPresenter.StrokeInput.StrokeStarted += StrokeInput_StrokeStarted;

            // We exclude CoreInputDeviceTypes.Touch because we use touch to select paragraphs.
            inkPresenter.InputDeviceTypes = CoreInputDeviceTypes.Pen | CoreInputDeviceTypes.Mouse;

            inkCanvas.Tapped += InkCanvas_Tapped;
            inkCanvas.DoubleTapped += InkCanvas_DoubleTapped;

            inkAnalyzer = new InkAnalyzer();
            paragraphSelected = null;

            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += DispatcherTimer_Tick;

            // We perform analysis when there has been a change to the
            // ink presenter and the user has been idle for 1 second.
            dispatcherTimer.Interval = TimeSpan.FromSeconds(1);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            dispatcherTimer.Stop();
        }

        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            dispatcherTimer.Stop();
            foreach (var stroke in args.Strokes)
            {
                inkAnalyzer.RemoveDataForStroke(stroke.Id);
            }
            dispatcherTimer.Start();
        }

        private void StrokeInput_StrokeStarted(InkStrokeInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            // Don't perform analysis while a stroke is in progress.
            dispatcherTimer.Stop();
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs args)
        {
            dispatcherTimer.Stop();
            inkAnalyzer.AddDataForStrokes(args.Strokes);
            dispatcherTimer.Start();
        }

        private async void DispatcherTimer_Tick(object sender, object e)
        {
            dispatcherTimer.Stop();
            if (!inkAnalyzer.IsAnalyzing)
            {
                var result = await inkAnalyzer.AnalyzeAsync();
            }
            else
            {
                // Ink analyzer is busy. Wait a while and try again.
                dispatcherTimer.Start();
            }
        }

        private void ClearButton_Click(object sender, RoutedEventArgs e)
        {
            // Don't run analysis when there is nothing to analyze.
            dispatcherTimer.Stop();

            inkPresenter.StrokeContainer.Clear();
            inkAnalyzer.ClearDataForAllStrokes();
            canvas.Children.Clear();
            canvas.Children.Add(SelectionRect);
            SelectionRect.Visibility = Visibility.Collapsed;
        }

        private void InkCanvas_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var position = e.GetPosition(inkCanvas);

            var paragraph = FindHitParagraph(position);
            if (paragraph == null)
            {
                // Did not tap on a paragraph.
                SelectionRect.Visibility = Visibility.Collapsed;
            }
            else
            {
                // Show the selection rect at the paragraph's bounding rect.
                Rect rect = paragraph.BoundingRect;
                SelectionRect.Width = rect.Width;
                SelectionRect.Height = rect.Height;
                Canvas.SetLeft(SelectionRect, rect.Left);
                Canvas.SetTop(SelectionRect, rect.Top);

                SelectionRect.Visibility = Visibility.Visible;
            }
            paragraphSelected = paragraph;
        }

        private void InkCanvas_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e)
        {
            // Convert the selected paragraph or list item
            if (paragraphSelected != null)
            {
                Rect rect = paragraphSelected.BoundingRect;
                var text = ExtractTextFromParagraph(paragraphSelected);

                if ((rect.X > 0) && (rect.Y > 0) && (text != string.Empty))
                {
                    // Create text box with recognized text
                    var textBlock = new TextBlock();
                    textBlock.Text = text;
                    textBlock.MaxWidth = rect.Width;
                    textBlock.MaxHeight = rect.Height;
                    Canvas.SetLeft(textBlock, rect.X);
                    Canvas.SetTop(textBlock, rect.Y);

                    // Remove strokes from InkPresenter
                    IReadOnlyList<uint> strokeIds = paragraphSelected.GetStrokeIds();
                    foreach (var strokeId in strokeIds)
                    {
                        var stroke = inkPresenter.StrokeContainer.GetStrokeById(strokeId);
                        stroke.Selected = true;
                    }
                    inkPresenter.StrokeContainer.DeleteSelected();

                    // Remove strokes from InkAnalyzer
                    inkAnalyzer.RemoveDataForStrokes(strokeIds);

                    // Hide the SelectionRect
                    SelectionRect.Visibility = Visibility.Collapsed;

                    canvas.Children.Add(textBlock);
                    paragraphSelected = null;
                }
            }
        }

        private string ExtractTextFromParagraph(InkAnalysisParagraph paragraph)
        {
            // The paragraph.RecognizedText property also returns the text,
            // but manually walking through the lines allows us to preserve
            // line breaks.
            var lines = new List<string>();
            foreach (var child in paragraph.Children)
            {
                if (child.Kind == InkAnalysisNodeKind.Line)
                {
                    var line = (InkAnalysisLine)child;
                    lines.Add(line.RecognizedText);
                }
                else if (child.Kind == InkAnalysisNodeKind.ListItem)
                {
                    var listItem = (InkAnalysisListItem)child;
                    lines.Add(listItem.RecognizedText);
                }
            }
            return String.Join("\n", lines);
        }

        private InkAnalysisParagraph FindHitParagraph(Point pt)
        {
            var paragraphs = inkAnalyzer.AnalysisRoot.FindNodes(InkAnalysisNodeKind.Paragraph);
            foreach (var paragraph in paragraphs)
            {
                // To support ink written with angle, RotatedBoundingRect should be used in hit testing.
                if (RectHelper.Contains(paragraph.BoundingRect, pt))
                {
                    return (InkAnalysisParagraph)paragraph;
                }
            }
            return null;
        }
   }
}