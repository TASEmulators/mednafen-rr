Public Class Front
    'Constants
    Dim MEDNAFEN As String = "mednafen.exe "
    Dim CONFIGFILE As String = "med-front.cfg"
    Dim MOVIE As String = "-mov "
    Dim STATE As String = "-loadstate "
    Dim PAUSE As String = "-pause 1"
    Dim READ As String = "-readonly 0"
    Dim PLAY As String = "-play 1"
    Dim RECORD As String = "-record 1"
    Dim AUTHOR As String = "-author "

    Friend EMUVERSION As String = "Mednafen Rerecording 1.1"
    Friend FRONTVERSION As String = "Frontend 1.0"

    'Globals
    Dim ShellHandle As Integer
    Dim ConfigInfo As String
    Dim MednafenIsRunning As Boolean = False
    Dim CloseMednafenOnExit As Boolean = False
    Dim AutoRun As Boolean = False

    Dim tempProc As Process

    'Startup routine
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Text = EMUVERSION + " " + FRONTVERSION
        CommandBox.Text = MEDNAFEN
        LengthLabel.Text = CommandBox.Text.Length()
        LoadConfig()
        If (CloseMednafenOnExit) Then
            CloseMednafenOnExitToolStripMenuItem.Checked = True
        End If
        If (AutoRun) Then
            AutoRunToolStripMenuItem.Checked = True
            LaunchMednafen()
        Else
            UpdateCommandLine()
        End If
    End Sub

    Private Sub ExitToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExitToolStripMenuItem.Click
        'Exit
        CloseIt()
    End Sub

    Private Sub ExitButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExitButton.Click
        'Exit
        CloseIt()
    End Sub

    Private Sub Update_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Update.Click
        UpdateCommandLine()
    End Sub

    Private Sub LaunchButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LaunchButton.Click
        LaunchMednafen()
    End Sub

    Private Sub RomBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RomBrowse.Click
        GetRom()
    End Sub

    Private Sub MovieBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieBrowse.Click
        GetMovie()
    End Sub

    Private Sub StateBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles StateBrowse.Click
        GetState()
    End Sub

    Private Sub RomToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RomToolStripMenuItem.Click
        If (MednafenIsRunning) Then 'Just in case.  This menu event should never happen if this isn't already true though.
            If (ShellHandle) Then
                Try
                    tempProc = Process.GetProcessById(ShellHandle)
                Catch
                    ShellHandle = 0
                End Try
            End If

            Try
                Dim DummyBool As Boolean = tempProc.HasExited   'Attempt to check the process, if it fails this causes the catch routine to play out intead
                GetRom()
                LaunchMednafen()    'Close Mednafen
                LaunchMednafen()    'Open Mednafen
            Catch
                LaunchButton.Text = "Launch Mednafen"
                MednafenIsRunning = False
            End Try
        End If
    End Sub

    Private Sub MovieToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieToolStripMenuItem.Click
        If (MednafenIsRunning) Then 'Just in case.  This menu event should never happen if this isn't already true though.
            If (ShellHandle) Then
                Try
                    tempProc = Process.GetProcessById(ShellHandle)
                Catch
                    ShellHandle = 0
                End Try
            End If

            Try
                Dim DummyBool As Boolean = tempProc.HasExited   'Attempt to check the process, if it fails this causes the catch routine to play out intead
                GetMovie()
                LaunchMednafen()    'Close Mednafen
                LaunchMednafen()    'Open Mednafen
            Catch
                LaunchButton.Text = "Launch Mednafen"
                MednafenIsRunning = False
            End Try
        End If
    End Sub
    Private Sub ClearButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ClearButton.Click
        ClearAllFields()
    End Sub

    Private Sub ClearToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ClearToolStripMenuItem.Click
        ClearAllFields()
    End Sub

    Private Sub HelpToolStripMenuItem1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles HelpToolStripMenuItem1.Click
        Help.Show()
    End Sub

    Private Sub CloseMednafenOnExitToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CloseMednafenOnExitToolStripMenuItem.Click
        If (CloseMednafenOnExit) Then
            CloseMednafenOnExit = False
        Else
            CloseMednafenOnExit = True
        End If

        If (CloseMednafenOnExit) Then
            CloseMednafenOnExitToolStripMenuItem.Checked = True
        Else
            CloseMednafenOnExitToolStripMenuItem.Checked = False
        End If
    End Sub

    Private Sub AutoRunToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles AutoRunToolStripMenuItem.Click
        If (AutoRun) Then
            AutoRun = False
        Else
            AutoRun = True
        End If

        If (AutoRun) Then
            AutoRunToolStripMenuItem.Checked = True
        Else
            AutoRunToolStripMenuItem.Checked = False
        End If
    End Sub

    Private Sub LaunchMednafenToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LaunchMednafenToolStripMenuItem.Click
        LaunchMednafen()
    End Sub

    Private Sub MenuStrip1_MenuActivate(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MenuStrip1.MenuActivate
        If (MednafenIsRunning) Then
            RomToolStripMenuItem.Enabled = True
            MovieToolStripMenuItem.Enabled = True
            LaunchMednafenToolStripMenuItem.Text = "Close Mednafen"
        Else
            RomToolStripMenuItem.Enabled = False
            MovieToolStripMenuItem.Enabled = False
            LaunchMednafenToolStripMenuItem.Text = "Launch Mednafen"
        End If
    End Sub

    Private Sub PlayCheckBox_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles PlayCheckBox.Click
        If (RecordCheckBox.Checked = True) Then
            RecordCheckBox.Checked = False
        End If
    End Sub

    Private Sub RecordCheckBox_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RecordCheckBox.Click
        If (PlayCheckBox.Checked = True) Then
            PlayCheckBox.Checked = False
        End If
    End Sub

    Private Sub DirectoryOverridesToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles DirectoryOverridesToolStripMenuItem.Click
        Directories.Show()
    End Sub

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    'Subs
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Private Sub CloseIt()
       SaveConfig()
        If (CloseMednafenOnExit And MednafenIsRunning) Then
            Try
                Dim tempProc As Process = Process.GetProcessById(ShellHandle)
                tempProc.CloseMainWindow()
            Catch

            End Try
        End If
        Me.Close()
    End Sub

    Private Sub UpdateCommandLine()
        If (Directories.MednafenDirectoryString.Length()) Then
            Dim Temp As String = Directories.MednafenDirectoryString & "\" & MEDNAFEN
            MEDNAFEN = Temp
        End If

        Dim movieCommand As String = ""
        If (MovieBox.Text.Length()) Then
            movieCommand = MOVIE & """" & MovieBox.Text() & """"
        End If

        Dim stateCommand As String = ""
        If (StateBox.Text.Length()) Then
            stateCommand = STATE & """" & StateBox.Text() & """"
        End If

        Dim authorCommand As String = ""
        If (AuthorBox.Text.Length()) Then
            authorCommand = AUTHOR & """" & AuthorBox.Text() & """"
        End If

        'Add Flags
        Dim Flags As String = ""
        If (PauseCheckBox.Checked) Then
            Flags = PAUSE
        End If

        If (ReadonlyCheckBox.Checked = False) Then
            Flags = Flags & " " & READ
        End If

        If (PlayCheckBox.Checked) Then
            Flags = Flags & " " & PLAY
        ElseIf (RecordCheckBox.Checked) Then
            Flags = Flags & " " & RECORD
        End If

        If (RomBox.Text.Length()) Then
            CommandBox.Text = MEDNAFEN + movieCommand + " " + stateCommand + " " + authorCommand + " " + OtherCommands.Text() + " " + Flags + " " + """" + RomBox.Text() + """"

            If (CommandBox.Text.Length() > 255) Then
                LengthLabel.ForeColor = Color.Red
            Else
                LengthLabel.ForeColor = Color.Black
            End If
            LengthLabel.Text = CommandBox.Text.Length()
        End If

    End Sub

    Private Sub AboutToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles AboutToolStripMenuItem.Click
        About.ShowDialog()
    End Sub

    Private Sub GetRom()
        Dim OpenRom As New OpenFileDialog
        OpenRom.InitialDirectory = System.Environment.CurrentDirectory
        OpenRom.Filter = "Supported Files|*.nes;*.unf;*.fds;*.pce;*.lnx;*.o;*.ngp;*.ngc;*.ws;*.wsc|" & _
            "NES (*.nes, *.unf, *.fds)|*.nes;*.unf;*.fds|" & _
            "PCE (*.pce)|*.pce|" & _
            "Lynx (*.lnx, *.o)|*.lnx;*.o|" & _
            "NGP (*.ngp, .ngc)|*.ngp;*.ngc|" & _
            "WS (*.ws, *.wsc)|*.ws;*.wsc|" & _
            "All files (*.*)|*.*"
        OpenRom.RestoreDirectory = False

        OpenRom.ShowDialog()
        If (OpenRom.FileName.Length) Then
            RomBox.Text = OpenRom.FileName
        End If
    End Sub

    Private Sub GetMovie()
        Dim OpenMovie As New SaveFileDialog
        OpenMovie.InitialDirectory = System.Environment.CurrentDirectory
        OpenMovie.Filter = "Mednafen movie file (*.mcm)|*.mcm|All files (*.*)|*.*"
        OpenMovie.RestoreDirectory = False
        OpenMovie.CheckFileExists = False
        OpenMovie.CreatePrompt = False
        OpenMovie.CheckPathExists = False
        OpenMovie.OverwritePrompt = False
        OpenMovie.Title = "Select a movie file"

        OpenMovie.ShowDialog()
        If (OpenMovie.FileName.Length) Then
            MovieBox.Text = OpenMovie.FileName
        End If
    End Sub

    Private Sub GetState()
        Dim OpenState As New OpenFileDialog
        OpenState.InitialDirectory = System.Environment.CurrentDirectory
        OpenState.Filter = "Savestate Files(*.ncs)|*.nc*|All files (*.*)|*.*"
        OpenState.RestoreDirectory = False

        OpenState.ShowDialog()
        If (OpenState.FileName.Length) Then
            StateBox.Text = OpenState.FileName
        End If
    End Sub

    Private Sub GetConfigInfo()
        Dim Str As String = ""

        If (ConfigInfo.IndexOf(Environment.NewLine) > 0) Then
            RomBox.Text = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
        End If


        If (RomBox.Text.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(RomBox.Text.Length + 2, ConfigInfo.Length - (RomBox.Text.Length + 2))

            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                MovieBox.Text = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
            End If
        End If

        If (MovieBox.Text.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(MovieBox.Text.Length + 2, ConfigInfo.Length - (MovieBox.Text.Length + 2))

            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                StateBox.Text = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
            End If
        End If

        If (StateBox.Text.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(StateBox.Text.Length + 2, ConfigInfo.Length - (StateBox.Text.Length + 2))

            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                OtherCommands.Text = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
            End If
        End If

        If (OtherCommands.Text.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(OtherCommands.Text.Length + 2, ConfigInfo.Length - (OtherCommands.Text.Length + 2))

            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    PauseCheckBox.Checked = True
                Else
                    PauseCheckBox.Checked = False
                End If
            End If
        End If

        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    ReadonlyCheckBox.Checked = True
                Else
                    ReadonlyCheckBox.Checked = False
                End If
            End If
        End If

        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    PlayCheckBox.Checked = True
                Else
                    PlayCheckBox.Checked = False
                End If
            End If
        End If

        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    RecordCheckBox.Checked = True
                Else
                    RecordCheckBox.Checked = False
                End If
            End If
        End If

        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    CloseMednafenOnExit = True
                Else
                    CloseMednafenOnExit = False
                End If
            End If
        End If

        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Str = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
                If (Str = "True") Then
                    AutoRun = True
                Else
                    AutoRun = False
                End If
            End If
        End If

        'Directories
        If (Str.Length + 2 < ConfigInfo.Length) Then
            ConfigInfo = ConfigInfo.Substring(Str.Length + 2, ConfigInfo.Length - (Str.Length + 2))
            If (ConfigInfo.IndexOf(Environment.NewLine)) Then
                Directories.MednafenDirectoryString = ConfigInfo.Substring(0, ConfigInfo.IndexOf(Environment.NewLine))
            End If
        End If

    End Sub


    'Save config file
    Private Sub SaveConfig()
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, RomBox.Text() & Environment.NewLine, False)
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, MovieBox.Text() & Environment.NewLine, True)
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, StateBox.Text() & Environment.NewLine, True)
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, OtherCommands.Text() & Environment.NewLine, True)

        'Checkboxes
        Dim Str As String
        If (PauseCheckBox.Checked) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        If (ReadonlyCheckBox.Checked) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        If (PlayCheckBox.Checked) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        If (RecordCheckBox.Checked) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        'Front settings
        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

        If (CloseMednafenOnExit) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        If (AutoRun) = True Then
            Str = "True"
        Else
            Str = "False"
        End If
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Str & Environment.NewLine, True)

        'Directory Overrides
        My.Computer.FileSystem.WriteAllText(CONFIGFILE, Directories.MednafenDirectoryString & Environment.NewLine, True)

    End Sub

    'Load config file
    Private Sub LoadConfig()
        If (My.Computer.FileSystem.FileExists(CONFIGFILE)) Then
            ConfigInfo = My.Computer.FileSystem.ReadAllText("med-front.cfg")
            GetConfigInfo()
        Else
            ConfigInfo = ""
            My.Computer.FileSystem.WriteAllText(CONFIGFILE, ConfigInfo, False)
        End If
    End Sub

    Private Sub ClearAllFields()
        'General
        RomBox.Text = ""
        StateBox.Text = ""
        OtherCommands.Text = ""
        PauseCheckBox.Checked = False

        'Movie
        MovieBox.Text = ""
        AuthorBox.Text = ""
        ReadonlyCheckBox.Checked = False
        PlayCheckBox.Checked = False
        RecordCheckBox.Checked = False

        CommandBox.Text = MEDNAFEN

    End Sub

    Private Sub LaunchMednafen()
        If (ShellHandle) Then
            Try
                tempProc = Process.GetProcessById(ShellHandle)
            Catch
                ShellHandle = 0
            End Try
        End If

        UpdateCommandLine()

        Try
            If (MednafenIsRunning) Then
                If (tempProc.HasExited) Then
                    LaunchButton.Text = "Launch Mednafen"
                    MednafenIsRunning = False
                Else
                    Try

                        tempProc.CloseMainWindow()
                        LaunchButton.Text = "Launch Mednafen"
                        ShellHandle = 0
                        MednafenIsRunning = False
                    Catch
                        MessageBox.Show("Could not close mednafen.exe", "Mednafen error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                    End Try
                End If
            Else
                'Launch mednafen using CommandLine
                If (RomBox.Text.Length()) Then
                    Try
                        If (CommandBox.Text.Length() <= 255) Then
                            ShellHandle = Shell(CommandBox.Text(), AppWinStyle.NormalFocus, False, -1)
                            LaunchButton.Text = "Close Mednafen"
                            MednafenIsRunning = True
                        Else
                            MessageBox.Show("Argument must be less than 255 characters", "Commandline Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                        End If
                    Catch
                        MessageBox.Show("Could not locate mednafen.exe", "File Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                    End Try
                Else
                    MessageBox.Show("Must specify a ROM to launch Mednafen", "File Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                End If
            End If
        Catch
            LaunchButton.Text = "Launch Mednafen"
            MednafenIsRunning = False
        End Try
    End Sub
End Class
