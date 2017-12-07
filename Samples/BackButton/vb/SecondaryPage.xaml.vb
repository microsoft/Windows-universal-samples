Imports System
Imports System.Collections.Generic
Imports System.IO
Imports System.Linq
Imports System.Runtime.InteropServices.WindowsRuntime
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class SecondaryPage
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            Dim optedIn As Boolean = False
            If CType(e.Parameter, Boolean) Then
                optedIn = True
            End If

            Dim rootFrame As Frame = TryCast(Window.Current.Content, Frame)
            If rootFrame.CanGoBack AndAlso optedIn Then
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Visible
            Else
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Collapsed
            End If
        End Sub
    End Class
End Namespace
