'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
' INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
' IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
' DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
' TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
' THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'
'*********************************************************
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Numerics
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Core
Imports Windows.Foundation
Imports Windows.UI
Imports Windows.UI.Composition
Imports Windows.UI.Core

Namespace Global.CompositionVisual

    Class VisualProperties
        Implements IFrameworkView

        Sub Initialize(view As CoreApplicationView) Implements IFrameworkView.Initialize
            _view = view
            _random = New Random()
        End Sub

        Sub SetWindow(window As CoreWindow) Implements IFrameworkView.SetWindow
            _window = window
            InitNewComposition()
            AddHandler _window.PointerPressed, AddressOf OnPointerPressed
            AddHandler _window.PointerMoved, AddressOf OnPointerMoved
            AddHandler _window.PointerReleased, AddressOf OnPointerReleased
        End Sub

        Sub OnPointerPressed(window As CoreWindow, args As PointerEventArgs)
            Dim position As Point = args.CurrentPoint.Position
            For Each child In _root.Children
                '
                ' Did we hit this child?
                '
                Dim offset As Vector3 = child.Offset
                Dim size As Vector2 = child.Size
                If(position.X >= offset.X) AndAlso (position.X < offset.X + size.X) AndAlso (position.Y >= offset.Y) AndAlso (position.Y < offset.Y + size.Y) Then
                    _currentVisual = TryCast(child, ContainerVisual)
                    _offsetBias = New Vector2(CType((offset.X - position.X), Single), CType((offset.Y - position.Y), Single))
                End If
            Next

            If _currentVisual IsNot Nothing Then
                Dim parent As ContainerVisual = TryCast(_currentVisual.Parent, ContainerVisual)
                parent.Children.Remove(_currentVisual)
                parent.Children.InsertAtTop(_currentVisual)
            End If
        End Sub

        Sub OnPointerMoved(window As CoreWindow, args As PointerEventArgs)
            If _currentVisual IsNot Nothing Then
                If Not _dragging Then
                    _currentVisual.Opacity = 1.0f
                    For Each child In _currentVisual.Children
                        child.RotationAngleInDegrees = 45.0F
                        child.CenterPoint = New Vector3(_currentVisual.Size.X / 2, _currentVisual.Size.Y / 2, 0)
                        Exit For
                    Next

                    '
                    ' Clip the visual to its original layout rect by using an inset
                    ' clip with a one-pixel margin all around
                    '
                    Dim clip = _compositor.CreateInsetClip()
                    clip.LeftInset = 1.0f
                    clip.RightInset = 1.0f
                    clip.TopInset = 1.0f
                    clip.BottomInset = 1.0f
                    _currentVisual.Clip = clip
                    _dragging = True
                End If

                Dim position As Point = args.CurrentPoint.Position
                _currentVisual.Offset = New Vector3(CType((position.X + _offsetBias.X), Single), CType((position.Y + _offsetBias.Y), Single), 0.0f)
            End If
        End Sub

        Sub OnPointerReleased(window As CoreWindow, args As PointerEventArgs)
            If _currentVisual IsNot Nothing Then
                If _dragging Then
                    For Each child In _currentVisual.Children
                        child.RotationAngle = 0.0f
                        child.CenterPoint = New Vector3(0.0f, 0.0f, 0.0f)
                        Exit For
                    Next

                    _currentVisual.Opacity = 0.8f
                    _currentVisual.Clip = Nothing
                    _dragging = False
                End If

                _currentVisual = Nothing
            End If
        End Sub

        Sub Load(unused As String) Implements IFrameworkView.Load
        End Sub

        Sub Run() Implements IFrameworkView.Run
            _window.Activate()
            _window.Dispatcher.ProcessEvents(CoreProcessEventsOption.ProcessUntilQuit)
        End Sub

        Sub Uninitialize() Implements IFrameworkView.Uninitialize
            _window = Nothing
            _view = Nothing
        End Sub

        Sub InitNewComposition()
            _compositor = New Compositor()
            _root = _compositor.CreateContainerVisual()
            _compositionTarget = _compositor.CreateTargetForCurrentView()
            _compositionTarget.Root = _root
            For index = 0 To 20 - 1
                _root.Children.InsertAtTop(CreateChildElement())
            Next
        End Sub

        Function CreateChildElement() As Visual
            '
            ' Each element consists of three visuals, which produce the appearance
            ' of a framed rectangle
            '
            Dim element = _compositor.CreateContainerVisual()
            element.Size = New Vector2(100.0f, 100.0f)
            element.Offset = New Vector3(CType((_random.NextDouble() * 400), Single), CType((_random.NextDouble() * 400), Single), 0.0f)
            '
            ' The outer rectangle is always white
            '
            Dim visual = _compositor.CreateSpriteVisual()
            element.Children.InsertAtTop(visual)
            visual.Brush = _compositor.CreateColorBrush(Color.FromArgb(&HFF, &HFF, &HFF, &HFF))
            visual.Size = New Vector2(100.0f, 100.0f)
            '
            ' The inner rectangle is inset from the outer by three pixels all around
            '
            Dim child = _compositor.CreateSpriteVisual()
            visual.Children.InsertAtTop(child)
            child.Offset = New Vector3(3.0f, 3.0f, 0.0f)
            child.Size = New Vector2(94.0f, 94.0f)
            '
            ' Pick a random color for every rectangle
            '
            Dim red As Byte = CType((&HFF * (0.2f + (_random.NextDouble() / 0.8f))), Byte)
            Dim green As Byte = CType((&HFF * (0.2f + (_random.NextDouble() / 0.8f))), Byte)
            Dim blue As Byte = CType((&HFF * (0.2f + (_random.NextDouble() / 0.8f))), Byte)
            child.Brush = _compositor.CreateColorBrush(Color.FromArgb(&HFF, red, green, blue))
            element.Opacity = 0.8f
            Return element
        End Function

        ' CoreWindow / CoreApplicationView
        Private _window As CoreWindow

        Private _view As CoreApplicationView

        ' Windows.UI.Composition
        Private _compositor As Compositor

        Private _compositionTarget As CompositionTarget

        Private _root As ContainerVisual

        Private _currentVisual As ContainerVisual

        Private _offsetBias As Vector2

        Private _dragging As Boolean

        ' Helpers
        Private _random As Random
    End Class

    Public NotInheritable Class VisualPropertiesFactory
        Implements IFrameworkViewSource

        Function CreateView() As IFrameworkView Implements IFrameworkViewSource.CreateView
            Return New VisualProperties()
        End Function

        '------------------------------------------------------------------------------
        '
        ' main
        '
        '------------------------------------------------------------------------------
        Shared Function Main(args As String()) As Integer
            CoreApplication.Run(New VisualPropertiesFactory())
            Return 0
        End Function
    End Class
End Namespace
