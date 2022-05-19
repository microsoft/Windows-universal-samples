Imports System
Imports System.Collections.Generic
Imports System.IO
Imports System.Linq
Imports System.Runtime.Serialization
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.ApplicationModel
Imports Windows.Storage
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate.Common

    ''' <summary>
    ''' SuspensionManager captures global session state to simplify process lifetime management
    ''' for an application.  Note that session state will be automatically cleared under a variety
    ''' of conditions and should only be used to store information that would be convenient to
    ''' carry across sessions, but that should be discarded when an application crashes or is
    ''' upgraded.
    ''' </summary>
    Friend NotInheritable Class SuspensionManager

        Private Shared _sessionState As Dictionary(Of String, Object) = New Dictionary(Of String, Object)()

        Private Shared _knownTypes As List(Of Type) = New List(Of Type)()

        Private Const sessionStateFilename As String = "_sessionState.xml"

        Public Shared ReadOnly Property SessionState As Dictionary(Of String, Object)
            Get
                Return _sessionState
            End Get
        End Property

        Public Shared ReadOnly Property KnownTypes As List(Of Type)
            Get
                Return _knownTypes
            End Get
        End Property

        ''' <summary>
        ''' Save the current <see cref="SessionState"/> .  Any <see cref="Frame"/>  instances
        ''' registered with <see cref="RegisterFrame"/>  will also preserve their current
        ''' navigation stack, which in turn gives their active <see cref="Page"/>  an opportunity
        ''' to save its state.
        ''' </summary>
        ''' <returns>An asynchronous task that reflects when session state has been saved.</returns>
        Public Shared Async Function SaveAsync() As Task
            Try
                For Each weakFrameReference In _registeredFrames
                    Dim frame As Frame = Nothing
                    If weakFrameReference.TryGetTarget(frame) Then
                        SaveFrameNavigationState(frame)
                    End If
                Next

                ' Serialize the session state synchronously to avoid asynchronous access to shared
                ' state
                Dim sessionData As MemoryStream = New MemoryStream()
                Dim serializer As DataContractSerializer = New DataContractSerializer(GetType(Dictionary(Of String, Object)), _knownTypes)
                serializer.WriteObject(sessionData, _sessionState)
                ' Get an output stream for the SessionState file and write the state asynchronously
                Dim file As StorageFile = Await ApplicationData.Current.LocalFolder.CreateFileAsync(sessionStateFilename, CreationCollisionOption.ReplaceExisting)
                Using fileStream As Stream = Await file.OpenStreamForWriteAsync()
                    sessionData.Seek(0, SeekOrigin.Begin)
                    Await sessionData.CopyToAsync(fileStream)
                End Using
            Catch e As Exception
                Throw New SuspensionManagerException(e)
            End Try
        End Function

        ''' <summary>
        ''' Restores previously saved <see cref="SessionState"/> .  Any <see cref="Frame"/>  instances
        ''' registered with <see cref="RegisterFrame"/>  will also restore their prior navigation
        ''' state, which in turn gives their active <see cref="Page"/>  an opportunity restore its
        ''' state.
        ''' </summary>
        ''' <param name="sessionBaseKey">An optional key that identifies the type of session.
        ''' This can be used to distinguish between multiple application launch scenarios.</param>
        ''' <returns>An asynchronous task that reflects when session state has been read.  The
        ''' content of <see cref="SessionState"/>  should not be relied upon until this task
        ''' completes.</returns>
        Public Shared Async Function RestoreAsync(Optional sessionBaseKey As String = Nothing) As Task
            _sessionState = New Dictionary(Of String, [Object])()
            Try
                ' Get the input stream for the SessionState file
                Dim file As StorageFile = Await ApplicationData.Current.LocalFolder.GetFileAsync(sessionStateFilename)
                Using inStream As IInputStream = Await file.OpenSequentialReadAsync()
                    ' Deserialize the Session State
                    Dim serializer As DataContractSerializer = New DataContractSerializer(GetType(Dictionary(Of String, Object)), _knownTypes)
                    _sessionState = CType(serializer.ReadObject(inStream.AsStreamForRead()), Dictionary(Of String, Object))
                End Using

                For Each weakFrameReference In _registeredFrames
                    Dim frame As Frame = Nothing
                    If weakFrameReference.TryGetTarget(frame) AndAlso CType(frame.GetValue(FrameSessionBaseKeyProperty), String) = sessionBaseKey Then
                        frame.ClearValue(FrameSessionStateProperty)
                        RestoreFrameNavigationState(frame)
                    End If
                Next
            Catch e As Exception
                Throw New SuspensionManagerException(e)
            End Try
        End Function

        Private Shared FrameSessionStateKeyProperty As DependencyProperty = DependencyProperty.RegisterAttached("_FrameSessionStateKey", GetType(String), GetType(SuspensionManager), Nothing)

        Private Shared FrameSessionBaseKeyProperty As DependencyProperty = DependencyProperty.RegisterAttached("_FrameSessionBaseKeyParams", GetType(String), GetType(SuspensionManager), Nothing)

        Private Shared FrameSessionStateProperty As DependencyProperty = DependencyProperty.RegisterAttached("_FrameSessionState", GetType(Dictionary(Of String, [Object])), GetType(SuspensionManager), Nothing)

        Private Shared _registeredFrames As List(Of WeakReference(Of Frame)) = New List(Of WeakReference(Of Frame))()

        ''' <summary>
        ''' Registers a <see cref="Frame"/>  instance to allow its navigation history to be saved to
        ''' and restored from <see cref="SessionState"/> .  Frames should be registered once
        ''' immediately after creation if they will participate in session state management.  Upon
        ''' registration if state has already been restored for the specified key
        ''' the navigation history will immediately be restored.  Subsequent invocations of
        ''' <see cref="RestoreAsync"/>  will also restore navigation history.
        ''' </summary>
        ''' <param name="frame">An instance whose navigation history should be managed by
        ''' <see cref="SuspensionManager"/></param>
        ''' <param name="sessionStateKey">A unique key into <see cref="SessionState"/>  used to
        ''' store navigation-related information.</param>
        ''' <param name="sessionBaseKey">An optional key that identifies the type of session.
        ''' This can be used to distinguish between multiple application launch scenarios.</param>
        Public Shared Sub RegisterFrame(frame As Frame, sessionStateKey As String, Optional sessionBaseKey As String = Nothing)
            If frame.GetValue(FrameSessionStateKeyProperty) IsNot Nothing Then
                Throw New InvalidOperationException("Frames can only be registered to one session state key")
            End If

            If frame.GetValue(FrameSessionStateProperty) IsNot Nothing Then
                Throw New InvalidOperationException("Frames must be either be registered before accessing frame session state, or not registered at all")
            End If

            If Not String.IsNullOrEmpty(sessionBaseKey) Then
                frame.SetValue(FrameSessionBaseKeyProperty, sessionBaseKey)
                sessionStateKey = sessionBaseKey & "_" & sessionStateKey
            End If

            frame.SetValue(FrameSessionStateKeyProperty, sessionStateKey)
            _registeredFrames.Add(New WeakReference(Of Frame)(frame))
            RestoreFrameNavigationState(frame)
        End Sub

        ''' <summary>
        ''' Disassociates a <see cref="Frame"/>  previously registered by <see cref="RegisterFrame"/> 
        ''' from <see cref="SessionState"/> .  Any navigation state previously captured will be
        ''' removed.
        ''' </summary>
        ''' <param name="frame">An instance whose navigation history should no longer be
        ''' managed.</param>
        Public Shared Sub UnregisterFrame(frame As Frame)
            SessionState.Remove(CType(frame.GetValue(FrameSessionStateKeyProperty), String))
            _registeredFrames.RemoveAll(Function(weakFrameReference)
                                            Dim testFrame As Frame = Nothing
                                            Return Not weakFrameReference.TryGetTarget(testFrame) OrElse testFrame Is frame
                                        End Function)
        End Sub

        ''' <summary>
        ''' Provides storage for session state associated with the specified <see cref="Frame"/> .
        ''' Frames that have been previously registered with <see cref="RegisterFrame"/>  have
        ''' their session state saved and restored automatically as a part of the global
        ''' <see cref="SessionState"/> .  Frames that are not registered have transient state
        ''' that can still be useful when restoring pages that have been discarded from the
        ''' navigation cache.
        ''' </summary>
        ''' <remarks>Apps may choose to rely on <see cref="NavigationHelper"/>  to manage
        ''' page-specific state instead of working with frame session state directly.</remarks>
        ''' <param name="frame">The instance for which session state is desired.</param>
        ''' <returns>A collection of state subject to the same serialization mechanism as
        ''' <see cref="SessionState"/> .</returns>
        Public Shared Function SessionStateForFrame(frame As Frame) As Dictionary(Of String, [Object])
            Dim frameState = CType(frame.GetValue(FrameSessionStateProperty), Dictionary(Of String, [Object]))
            If frameState Is Nothing Then
                Dim frameSessionKey = CType(frame.GetValue(FrameSessionStateKeyProperty), String)
                If frameSessionKey IsNot Nothing Then
                    If Not _sessionState.ContainsKey(frameSessionKey) Then
                        _sessionState(frameSessionKey) = New Dictionary(Of String, [Object])()
                    End If

                    frameState = CType(_sessionState(frameSessionKey), Dictionary(Of String, [Object]))
                Else
                    frameState = New Dictionary(Of String, [Object])()
                End If

                frame.SetValue(FrameSessionStateProperty, frameState)
            End If

            Return frameState
        End Function

        Private Shared Sub RestoreFrameNavigationState(frame As Frame)
            Dim frameState = SessionStateForFrame(frame)
            If frameState.ContainsKey("Navigation") Then
                frame.SetNavigationState(CType(frameState("Navigation"), String))
            End If
        End Sub

        Private Shared Sub SaveFrameNavigationState(frame As Frame)
            Dim frameState = SessionStateForFrame(frame)
            frameState("Navigation") = frame.GetNavigationState()
        End Sub
    End Class

    Public Class SuspensionManagerException
        Inherits Exception

        Public Sub New()
        End Sub

        Public Sub New(e As Exception)
            MyBase.New("SuspensionManager failed", e)
        End Sub
    End Class
End Namespace
