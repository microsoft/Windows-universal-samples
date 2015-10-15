'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System
Imports System.Collections.Generic
Imports Windows.ApplicationModel.DataTransfer
Imports Windows.Data.Html
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Partial Public NotInheritable Class CopyText
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            Me.Init()
        End Sub

#Region " Scenario Specific Code "

        Sub Init()
            AddHandler CopyButton.Click, New RoutedEventHandler(AddressOf CopyButton_Click)
            AddHandler PasteButton.Click, New RoutedEventHandler(AddressOf PasteButton_Click)
            Description.NavigateToString(Me.htmlFragment)
        End Sub

#End Region

#Region " Button click "

        Sub CopyButton_Click(sender As Object, e As RoutedEventArgs)
            OutputText.Text = ""
            OutputResourceMapKeys.Text = ""
            OutputHtml.NavigateToString("<HTML></HTML>")
            ' Set the content to DataPackage as html format
            Dim htmlFormat As String = HtmlFormatHelper.CreateHtmlFormat(Me.htmlFragment)
            Dim dataPackage = New DataPackage()
            dataPackage.SetHtmlFormat(htmlFormat)
            ' Set the content to DataPackage as (plain) text format
            Dim plainText As String = HtmlUtilities.ConvertToText(Me.htmlFragment)
            dataPackage.SetText(plainText)
            ' Populate resourceMap with StreamReference objects corresponding to local image files embedded in HTML
            Dim imgUri = New Uri(imgSrc)
            Dim imgRef = RandomAccessStreamReference.CreateFromUri(imgUri)
            dataPackage.ResourceMap(imgSrc) = imgRef
            Try
                Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage)
                OutputText.Text = "Text and HTML formats have been copied to clipboard. "
            Catch ex As Exception
                rootPage.NotifyUser("Error copying content to Clipboard: " & ex.Message & ". Try again", NotifyType.ErrorMessage)
            End Try
        End Sub

        Async Sub PasteButton_Click(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            OutputText.Text = "Content in the clipboard: "
            OutputResourceMapKeys.Text = ""
            OutputHtml.NavigateToString("<HTML></HTML>")
            Dim dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent()
            If dataPackageView.Contains(StandardDataFormats.Text) Then
                Try
                    Dim text = Await dataPackageView.GetTextAsync()
                    OutputText.Text = "Text: " & Environment.NewLine & text
                Catch ex As Exception
                    rootPage.NotifyUser("Error retrieving Text format from Clipboard: " & ex.Message, NotifyType.ErrorMessage)
                End Try
            Else
                OutputText.Text = "Text: " & Environment.NewLine & "Text format is not available in clipboard"
            End If

            If dataPackageView.Contains(StandardDataFormats.Html) Then
                Me.DisplayResourceMapAsync(dataPackageView)
                Dim htmlFormat As String = Nothing
                Try
                    htmlFormat = Await dataPackageView.GetHtmlFormatAsync()
                Catch ex As Exception
                    rootPage.NotifyUser("Error retrieving HTML format from Clipboard: " & ex.Message, NotifyType.ErrorMessage)
                End Try

                If htmlFormat IsNot Nothing Then
                    Dim htmlFragment As String = HtmlFormatHelper.GetStaticFragment(htmlFormat)
                    OutputHtml.NavigateToString("HTML:<br/ > " & htmlFragment)
                End If
            Else
                OutputHtml.NavigateToString("HTML:<br/ > HTML format is not available in clipboard")
            End If
        End Sub

#End Region

#Region " private member variables "
        Private htmlFragment As String = "Use clipboard to copy and paste text in different formats, including plain text, and formatted text (HTML). <br />" & " To <b>copy</b>, add text formats to a <i>DataPackage</i>, and then place <i>DataPackage</i> to Clipboard.<br /> " & "To <b>paste</b>, get <i>DataPackageView</i> from clipboard, and retrieve the content of the desired text format from it.<br />" & "To handle local image files in the formatted text (such as the one below), use ResourceMap collection." & "<br /><img id=""scenario1LocalImage"" src=""" & imgSrc & """ /><br />" & "<i>DataPackage</i> class is also used to share or send content between applications. <br />" & "See the Share SDK sample for more information."

        Private Const imgSrc As String = "ms-appx-web:///assets/windows-sdk.png"

#End Region

#Region " helper functions "
        ' Note: this sample is not trying to resolve and render the HTML using resource map.
        ' Please refer to the Clipboard JavaScript sample for an example of how to use resource map
        ' for local images display within an HTML format. This sample will only demonstrate how to
        ' get a resource map object and extract its key values
        Async Sub DisplayResourceMapAsync(dataPackageView As DataPackageView)
            OutputResourceMapKeys.Text = Environment.NewLine & "Resource map: "
            Dim resMap As IReadOnlyDictionary(Of String, RandomAccessStreamReference) = Nothing
            Try
                resMap = Await dataPackageView.GetResourceMapAsync()
            Catch ex As Exception
                rootPage.NotifyUser("Error retrieving Resource map from Clipboard: " & ex.Message, NotifyType.ErrorMessage)
            End Try

            If resMap IsNot Nothing Then
                If resMap.Count > 0 Then
                    For Each item In resMap
                        OutputResourceMapKeys.Text &= Environment.NewLine & "Key: " & item.Key
                    Next
                Else
                    OutputResourceMapKeys.Text &= Environment.NewLine & "Resource map is empty"
                End If
            End If
        End Sub
#End Region
    End Class
End Namespace
