Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Resources

Namespace Global.AppResourceClassLibrary

    Public Module LocalizedNamesLibrary

        Dim resourceLoader As ResourceLoader = Nothing

        Public ReadOnly Property LibraryName As String
            Get
                Dim name As String = Nothing
                GetLibraryName("string1", name)
                Return name
            End Get
        End Property

        Private Sub GetLibraryName(resourceName As String, ByRef resourceValue As String)
            If resourceLoader Is Nothing Then
                resourceLoader = ResourceLoader.GetForCurrentView("AppResourceClassLibrary/Resources")
            End If

            resourceValue = resourceLoader.GetString(resourceName)
        End Sub
    End Module
End Namespace
