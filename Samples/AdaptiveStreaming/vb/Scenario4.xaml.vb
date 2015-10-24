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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports System
Imports Windows.Media.Streaming.Adaptive
Imports Windows.UI.Core
'Simplifies call to the MediaProtectionManager
Imports Windows.Media.Protection
Imports Windows.Media.Protection.PlayReady
Imports System.Net.Http
Imports Windows.Foundation.Collections
Imports System.Net.Http.Headers
Imports LicenseRequest
Imports System.Runtime.InteropServices
Imports System.Threading.Tasks

Namespace Global.AdaptiveStreaming

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario4
        Inherits Page

        Private rootPage As MainPage

        Dim ams As AdaptiveMediaSource = Nothing

        Private protectionManager As MediaProtectionManager = Nothing

        Dim serviceCompletionNotifier As MediaProtectionServiceCompletion = Nothing

        Private playReadyLicenseUrl As String = ""

        Private playReadyChallengeCustomData As String = ""

        Private Const MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED As Integer = -2147174251

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Async Private Sub log(s As String)
            Await Me.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, New DispatchedHandler(Sub()
                                                                                                  Dim text As TextBlock = New TextBlock()
                                                                                                  text.Text = s
                                                                                                  text.TextWrapping = TextWrapping.WrapWholeWords
                                                                                                  stkOutput.Children.Add(text)
                                                                                              End Sub))
        End Sub

        Private Sub btnPlay_Click(sender As Object, e As Windows.UI.Xaml.RoutedEventArgs)
            If String.IsNullOrEmpty(txtInputURL.Text) Then
                rootPage.NotifyUser("Specify a URI to play", NotifyType.ErrorMessage)
                Return
            End If

            InitializeAdaptiveMediaSource(New System.Uri(txtInputURL.Text), mePlayer)
        End Sub

        Async Private Sub InitializeAdaptiveMediaSource(uri As System.Uri, m As MediaElement)
            Dim result As AdaptiveMediaSourceCreationResult = Await AdaptiveMediaSource.CreateFromUriAsync(uri)
            If result.Status = AdaptiveMediaSourceCreationStatus.Success Then
                ams = result.MediaSource
                SetUpProtectionManager(m)
                m.SetMediaStreamSource(ams)
            Else
                rootPage.NotifyUser("Error creating the AdaptiveMediaSource" & vbLf & vbTab & result.Status.ToString(), NotifyType.ErrorMessage)
            End If
        End Sub

        Private Sub SetUpProtectionManager(ByRef m As MediaElement)
            log("Enter SetUpProtectionManager")
            log("Creating protection system mappings...")
            protectionManager = New MediaProtectionManager()
            AddHandler protectionManager.ComponentLoadFailed, New ComponentLoadFailedEventHandler(AddressOf ProtectionManager_ComponentLoadFailed)
            AddHandler protectionManager.ServiceRequested, New ServiceRequestedEventHandler(AddressOf ProtectionManager_ServiceRequested)
            'Setup PlayReady as the ProtectionSystem to use by MF. 
            'The code here is mandatory and should be just copied directly over to the app
            Dim cpSystems As Windows.Foundation.Collections.PropertySet = New Windows.Foundation.Collections.PropertySet()
            cpSystems.Add("{F4637010-03C3-42CD-B932-B48ADF3A6A54}", "Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput")
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemIdMapping", cpSystems)
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionSystemId", "{F4637010-03C3-42CD-B932-B48ADF3A6A54}")
            protectionManager.Properties.Add("Windows.Media.Protection.MediaProtectionContainerGuid", "{9A04F079-9840-4286-AB92-E65BE0885F95}")
            m.ProtectionManager = protectionManager
            log("Leave SetUpProtectionManager")
        End Sub

        Private Async Sub ProtectionManager_ServiceRequested(sender As MediaProtectionManager, e As ServiceRequestedEventArgs)
            log("Enter ProtectionManager_ServiceRequested")
            If TypeOf e.Request Is PlayReadyIndividualizationServiceRequest Then
                Dim IndivRequest As PlayReadyIndividualizationServiceRequest = TryCast(e.Request, PlayReadyIndividualizationServiceRequest)
                Dim bResultIndiv As Boolean = Await ReactiveIndivRequest(IndivRequest, e.Completion)
            ElseIf TypeOf e.Request Is PlayReadyLicenseAcquisitionServiceRequest Then
                Dim licenseRequest As PlayReadyLicenseAcquisitionServiceRequest = TryCast(e.Request, PlayReadyLicenseAcquisitionServiceRequest)
                LicenseAcquisitionRequest(licenseRequest, e.Completion, playReadyLicenseUrl, playReadyChallengeCustomData)
            End If

            log("Leave ProtectionManager_ServiceRequested")
        End Sub

        Private Sub ProtectionManager_ComponentLoadFailed(sender As MediaProtectionManager, e As ComponentLoadFailedEventArgs)
            log("Enter ProtectionManager_ComponentLoadFailed")
            log(e.Information.ToString())
            For i = 0 To e.Information.Items.Count - 1
                log(e.Information.Items(i).Name & vbLf & "Reasons=0x" & e.Information.Items(i).Reasons.ToString() & vbLf & "Renewal Id=" & e.Information.Items(i).RenewalId)
            Next

            e.Completion.Complete(False)
            log("Leave ProtectionManager_ComponentLoadFailed")
        End Sub

        ''' <summary>
        ''' Invoked to acquire the PlayReady License
        ''' </summary>
        Async Sub LicenseAcquisitionRequest(licenseRequest As PlayReadyLicenseAcquisitionServiceRequest, CompletionNotifier As MediaProtectionServiceCompletion, Url As String, ChallengeCustomData As String)
            Dim bResult As Boolean = False
            Dim ExceptionMessage As String = String.Empty
            Try
                If Not String.IsNullOrEmpty(Url) Then
                    log("ProtectionManager PlayReady Manual License Acquisition Service Request in progress - URL: " & Url)
                    If Not String.IsNullOrEmpty(ChallengeCustomData) Then
                        Dim encoding As System.Text.UTF8Encoding = New System.Text.UTF8Encoding()
                        Dim b As Byte() = encoding.GetBytes(ChallengeCustomData)
                        licenseRequest.ChallengeCustomData = Convert.ToBase64String(b, 0, b.Length)
                    End If

                    Dim soapMessage As PlayReadySoapMessage = licenseRequest.GenerateManualEnablingChallenge()
                    Dim messageBytes As Byte() = soapMessage.GetMessageBody()
                    Dim httpContent As HttpContent = New ByteArrayContent(messageBytes)
                    Dim propertySetHeaders As IPropertySet = soapMessage.MessageHeaders
                    For Each strHeaderName In propertySetHeaders.Keys
                        Dim strHeaderValue As String = propertySetHeaders(strHeaderName).ToString()
                        If strHeaderName.Equals("Content-Type", StringComparison.OrdinalIgnoreCase) Then
                            httpContent.Headers.ContentType = MediaTypeHeaderValue.Parse(strHeaderValue)
                        Else
                            httpContent.Headers.Add(strHeaderName.ToString(), strHeaderValue)
                        End If
                    Next

                    Dim licenseAcquision As CommonLicenseRequest = New CommonLicenseRequest()
                    Dim responseHttpContent As HttpContent = Await licenseAcquision.AcquireLicense(New Uri(Url), httpContent)
                    If responseHttpContent IsNot Nothing Then
                        Dim exResult As Exception = licenseRequest.ProcessManualEnablingResponse(Await responseHttpContent.ReadAsByteArrayAsync())
                        If exResult IsNot Nothing Then
                            Throw exResult
                        End If

                        bResult = True
                    Else
                        ExceptionMessage = licenseAcquision.GetLastErrorMessage()
                    End If
                Else
                    log("ProtectionManager PlayReady License Acquisition Service Request in progress - URL: " & licenseRequest.Uri.ToString())
                    Await licenseRequest.BeginServiceRequest()
                    bResult = True
                End If
            Catch e As Exception
                ExceptionMessage = e.Message
            End Try

            If bResult = True Then
                log(If(Not String.IsNullOrEmpty(Url), "ProtectionManager Manual PlayReady License Acquisition Service Request successful", "ProtectionManager PlayReady License Acquisition Service Request successful"))
            Else
                log(If(Not String.IsNullOrEmpty(Url), "ProtectionManager Manual PlayReady License Acquisition Service Request failed: " & ExceptionMessage, "ProtectionManager PlayReady License Acquisition Service Request failed: " & ExceptionMessage))
            End If

            CompletionNotifier.Complete(bResult)
        End Sub

        ''' <summary>
        ''' Proactive Individualization Request 
        ''' </summary>
        Async Sub ProActiveIndivRequest()
            Dim indivRequest As PlayReadyIndividualizationServiceRequest = New PlayReadyIndividualizationServiceRequest()
            log("ProtectionManager PlayReady ProActive Individualization Service Request in progress...")
            Dim bResultIndiv As Boolean = Await ReactiveIndivRequest(indivRequest, Nothing)
            If bResultIndiv = True Then
                log("ProtectionManager PlayReady ProActive Individualization Service Request successful")
            Else
                log("ProtectionManager PlayReady ProActive Individualization Service Request failed")
            End If
        End Sub

        ''' <summary>
        ''' Invoked to send the Individualization Request 
        ''' </summary>
        Async Function ReactiveIndivRequest(IndivRequest As PlayReadyIndividualizationServiceRequest, CompletionNotifier As MediaProtectionServiceCompletion) As Task(Of Boolean)
            Dim bResult As Boolean = False
            Dim exception As Exception = Nothing
            log("ProtectionManager PlayReady Individualization Service Request in progress...")
            Try
                Await IndivRequest.BeginServiceRequest()
            Catch ex As Exception
                exception = ex
            Finally
                If exception Is Nothing Then
                    bResult = True
                Else
                    Dim comException As COMException = TryCast(exception, COMException)
                    If comException IsNot Nothing AndAlso comException.HResult = MSPR_E_CONTENT_ENABLING_ACTION_REQUIRED Then
                        IndivRequest.NextServiceRequest()
                    End If
                End If
            End Try

            If bResult = True Then
                log("ProtectionManager PlayReady Individualization Service Request successful")
            Else
                log("ProtectionManager PlayReady Individualization Service Request failed")
            End If

            If CompletionNotifier IsNot Nothing Then
                CompletionNotifier.Complete(bResult)
            End If

            Return bResult
        End Function
    End Class
End Namespace
