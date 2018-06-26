//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Linq;
using System.Numerics;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.System;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics
{
    /// <summary>
    /// A page that displays details for a single item within a group.
    /// </summary>
    public partial class ItemPage : Page
    {
        private Compositor _compositor;
        private ControlInfoDataItem _item;
        private ElementTheme? _currentElementTheme;

        public ControlInfoDataItem Item
        {
            get { return _item; }
            set { _item = value; }
        }
        
        public ItemPage()
        {
            this.InitializeComponent();

            LayoutVisualStates.CurrentStateChanged += (s, e) => UpdateSeeAlsoPanelVerticalTranslationAnimation();
            Loaded += (s,e) => SetInitialVisuals();
        }
        
        public void SetInitialVisuals()
        {
            NavigationRootPage.Current.PageHeader.TopCommandBar.Visibility = Visibility.Visible;
            NavigationRootPage.Current.PageHeader.ToggleThemeAction = OnToggleTheme;

            if (NavigationRootPage.Current.IsFocusSupported)
            {
                this.Focus(FocusState.Programmatic);
            }

            _compositor = ElementCompositionPreview.GetElementVisual(this).Compositor;

            UpdateSeeAlsoPanelVerticalTranslationAnimation();
        }

        private void UpdateSeeAlsoPanelVerticalTranslationAnimation()
        {
            var isEnabled = LayoutVisualStates.CurrentState == LargeLayout;

            ElementCompositionPreview.SetIsTranslationEnabled(seeAlsoPanel, true);
            
            var targetPanelVisual = ElementCompositionPreview.GetElementVisual(seeAlsoPanel);
            targetPanelVisual.Properties.InsertVector3("Translation", Vector3.Zero);

            if (isEnabled)
            {
                var scrollProperties = ElementCompositionPreview.GetScrollViewerManipulationPropertySet(svPanel);

                var expression = _compositor.CreateExpressionAnimation("ScrollManipulation.Translation.Y * -1");
                expression.SetReferenceParameter("ScrollManipulation", scrollProperties);
                expression.Target = "Translation.Y";
                targetPanelVisual.StartAnimation(expression.Target, expression);
            }
            else
            {
                targetPanelVisual.StopAnimation("Translation.Y");
            }
        }

        private void OnToggleTheme()
        {
            var currentElementTheme = ((_currentElementTheme ?? ElementTheme.Default) == ElementTheme.Default) ? App.ActualTheme : _currentElementTheme.Value;
            var newTheme = currentElementTheme == ElementTheme.Dark ? ElementTheme.Light : ElementTheme.Dark;
            SetControlExamplesTheme(newTheme);
        }

        private void SetControlExamplesTheme(ElementTheme theme)
        {
            var controlExamples = (this.contentFrame.Content as UIElement)?.GetDescendantsOfType<ControlExample>();

            if (controlExamples != null)
            {
                _currentElementTheme = theme;
                foreach (var controlExample in controlExamples)
                {
                    var exampleContent = controlExample.Example as FrameworkElement;
                    exampleContent.RequestedTheme = theme;
                    controlExample.ExampleContainer.RequestedTheme = theme;
                }
            }
        }

        private void OnRelatedControlClick(object sender, RoutedEventArgs e)
        {
            ButtonBase b = (ButtonBase)sender;

            this.Frame.Navigate(typeof(ItemPage), b.DataContext.ToString());
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            //Connected Animation

            PopOutStoryboard.Begin();
            PopOutStoryboard.Completed += (sender1_, e1_) =>
            {
                PopInStoryboard.Begin();
            };

            if (NavigationRootPage.Current.PageHeader != null)
            {
                var connectedAnimation = ConnectedAnimationService.GetForCurrentView().GetAnimation("controlAnimation");
                                
                if (connectedAnimation != null)
                {
                    var target = NavigationRootPage.Current.PageHeader.TitlePanel;
                    connectedAnimation.TryStart(target, new UIElement[] { subTitleText });
                }
            }

            var item = await ControlInfoDataSource.Instance.GetItemAsync((String)e.Parameter);

            if (item != null)
            {
                Item = item;

                // Load control page into frame.
                var loader = ResourceLoader.GetForCurrentView();

                string pageRoot = loader.GetString("PageStringRoot");

                string pageString = pageRoot + item.UniqueId + "Page";
                Type pageType = Type.GetType(pageString);

                if (pageType != null)
                {
                    this.contentFrame.Navigate(pageType);
                }

                NavigationRootPage.Current.NavigationView.Header = item?.Title;
                if (item.IsNew && NavigationRootPage.Current.CheckNewControlSelected())
                {
                    return;
                }

                ControlInfoDataGroup group = await ControlInfoDataSource.Instance.GetGroupFromItemAsync((String)e.Parameter);
                var menuItem = NavigationRootPage.Current.NavigationView.MenuItems.Cast<NavigationViewItem>().FirstOrDefault(m => m.Tag?.ToString() == group.UniqueId);
                if (menuItem != null)
                {
                    menuItem.IsSelected = true;
                }
            }

            base.OnNavigatedTo(e);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            SetControlExamplesTheme(App.ActualTheme);

            base.OnNavigatingFrom(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            NavigationRootPage.Current.PageHeader.TopCommandBar.Visibility = Visibility.Collapsed;
            NavigationRootPage.Current.PageHeader.ToggleThemeAction = null;

            //Reverse Connected Animation
            if (e.SourcePageType != typeof(ItemPage))
            {
                var target = NavigationRootPage.Current.PageHeader.TitlePanel;
                ConnectedAnimationService.GetForCurrentView().PrepareToAnimate("controlAnimation", target);
            }

            base.OnNavigatedFrom(e);
        }
                
        private void OnSvPanelLoaded(object sender, RoutedEventArgs e)
        {
            svPanel.XYFocusDown = contentFrame.GetDescendantsOfType<Control>().FirstOrDefault(c => c.IsTabStop) ?? svPanel.GetDescendantsOfType<Control>().FirstOrDefault(c => c.IsTabStop);
        }

        private void OnSvPanelGotFocus(object sender, RoutedEventArgs e)
        {
            if (e.OriginalSource == sender && NavigationRootPage.Current.IsFocusSupported)
            {
                bool isElementFound = false;
                var elements = contentFrame.GetDescendantsOfType<Control>().Where(c => c.IsTabStop);
                foreach (var element in elements)
                {
                    Rect elementRect = element.TransformToVisual(svPanel).TransformBounds(new Rect(0.0, 0.0, element.ActualWidth, element.ActualHeight));
                    Rect panelRect = new Rect(0.0, 70.0, svPanel.ActualWidth, svPanel.ActualHeight);

                    if (elementRect.Top < panelRect.Bottom)
                    {
                        element.Focus(FocusState.Programmatic);
                        isElementFound = true;
                        break;
                    }
                }
                if (!isElementFound)
                {
                    svPanel.UseSystemFocusVisuals = true;
                }
            }
        }

        private void OnSvPanelKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Up)
            {
                var nextElement = FocusManager.FindNextElement(FocusNavigationDirection.Up);
                if (nextElement.GetType() == typeof(NavigationViewItem))
                {
                    NavigationRootPage.Current.PageHeader.Focus(FocusState.Programmatic);
                }
                else
                {
                    FocusManager.TryMoveFocus(FocusNavigationDirection.Up);
                }
            }
        }

        private void OnContentRootSizeChanged(object sender, SizeChangedEventArgs e)
        {
            string targetState = "NormalFrameContent";

            if ((contentColumn.ActualWidth) >= 1000)
            {
                targetState = "WideFrameContent";
            }

            VisualStateManager.GoToState(this, targetState, false);
        }
    }
}