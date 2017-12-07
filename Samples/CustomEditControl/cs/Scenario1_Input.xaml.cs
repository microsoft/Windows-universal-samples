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
using System.Diagnostics;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Text.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_Input : Page
    {
        public Scenario1_Input()
        {
            this.InitializeComponent();
#if x
            // Handle touch event to manage focus.
            CoreWindow.GetForCurrentThread().PointerPressed += Page_PointerPressed;
#endif
        }

#if x
        //Used to check if the pointer is within the bounds of the edit control.
        //If it is, focus should go to the edit control.  If it is outside the bounds
        //Focus should not be in the edit control.
        private void Page_PointerPressed(CoreWindow sender, PointerEventArgs args)
        {
            Rect _boundingbox = EditControl.GetLayout();
            if (_boundingbox.Contains(args.CurrentPoint.Position))
            {
                _textEditContext.InternalSetFocus();
                EditControl.Focus(FocusState.Programmatic);
            }
            else
            {
                _textEditContext.InternalRemoveFocus();
            }
        }
#endif

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
#if x
            //Destroy the pointer pressed handlers when navigating to another page.
            CoreWindow.GetForCurrentThread().PointerPressed -= Page_PointerPressed;
            _textEditContext.ShutDown();
            base.OnNavigatingFrom(e);
#endif
        }
        
    }
}
