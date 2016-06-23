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
Imports System.Net
Imports System.Net.Http
Imports System.Threading.Tasks

Namespace Global.LicenseRequest

    Public Class CommonLicenseRequest

        Private _lastErrorMessage As String

        Public Function GetLastErrorMessage() As String
            Return _lastErrorMessage
        End Function

        Public Sub New()
            _lastErrorMessage = String.Empty
        End Sub

        ''' <summary>
        ''' Invoked to acquire the PlayReady license.
        ''' </summary>
        ''' <param name="licenseServerUri">License Server URI to retrieve the PlayReady license.</param>
        ''' <param name="httpRequestContent">HttpContent including the Challenge transmitted to the PlayReady server.</param>
        Public Async Overridable Function AcquireLicense(licenseServerUri As Uri, httpRequestContent As HttpContent) As Task(Of HttpContent)
            Try
                Dim httpClient As HttpClient = New HttpClient()
                httpClient.DefaultRequestHeaders.Add("msprdrm_server_redirect_compat", "false")
                httpClient.DefaultRequestHeaders.Add("msprdrm_server_exception_compat", "false")
                Dim response As HttpResponseMessage = Await httpClient.PostAsync(licenseServerUri, httpRequestContent)
                response.EnsureSuccessStatusCode()
                If response.StatusCode = HttpStatusCode.OK Then
                    _lastErrorMessage = String.Empty
                    Return response.Content
                Else
                    _lastErrorMessage = "AcquireLicense - Http Response Status Code: " & response.StatusCode.ToString()
                End If
            Catch exception As Exception
                _lastErrorMessage = exception.Message
                Return Nothing
            End Try

            Return Nothing
        End Function
    End Class
End Namespace
