Public Class Front
    'Constants
    Dim MEDNAFEN As String = "mednafen.exe "
    Dim CONFIGFILE As String = "med-front.cfg"
    Dim MOVIE As String = "-mov "
    Dim STATE As String = "-loadstate "
    Dim PAUSE As String = "-pause 1"

    Friend EMUVERSION As String = "Mednafen Rerecording 1.1"
    Friend FRONTVERSION As String = "Frontend 1.0"

    'Globals
    Dim ShellHandle As Integer
    Dim ConfigInfo As String
    Dim MednafenIsRunning As Boolean = False
    Dim CloseMednafenOnExit As Boolean = False

    'Startup routine
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Text = EMUVERSION + " " + FRONTVERSION
        CommandBox.Text = MEDNAFEN
        LengthLabel.Text = CommandBox.Text.Length()
        LoadConfig()
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
        'Debug
        UpdateCommandLine()

        If (MednafenIsRunning) Then
            Try
                Dim tempProc As Process = Process.GetProcessById(ShellHandle)
                tempProc.CloseMainWindow()
                LaunchButton.Text = "Launch Mednafen"
                MednafenIsRunning = False
            Catch
                MessageBox.Show("Could not close mednafen.exe", "Mednafen error")
            End Try
        Else
            'Launch mednafen using CommandLine
            Try
                ShellHandle = Shell(CommandBox.Text(), AppWinStyle.NormalFocus, False, -1)
                LaunchButton.Text = "Close Mednafen"
                MednafenIsRunning = True
            Catch
                MessageBox.Show("Could not locate mednafen.exe", "File Error")
            End Try
        End If

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
        GetRom()
    End Sub

    Private Sub MovieToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieToolStripMenuItem.Click
        GetMovie()
    End Sub
    Private Sub ClearButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ClearButton.Click
        ClearAllFields()
    End Sub

    Private Sub ClearToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ClearToolStripMenuItem.Click
        ClearAllFields()
    End Sub

    Private Sub HelpToolStripMenuItem1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles HelpToolStripMenuItem1.Click
        'TODO: open this: http://code.google.com/p/mednafen-rr/
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

        Dim movieCommand As String = ""
        If (MovieBox.Text.Length()) Then
            movieCommand = MOVIE & """" & MovieBox.Text() & """"
        End If

        Dim stateCommand As String = ""
        If (StateBox.Text.Length()) Then
            stateCommand = STATE & """" & StateBox.Text() & """"
        End If

        'Add Flags
        Dim Flags As String = ""
        If (PauseCheckBox.Checked) Then
            Flags = PAUSE
        End If

        If (RomBox.Text.Length()) Then
            CommandBox.Text = MEDNAFEN + movieCommand + " " + stateCommand + " " + OtherCommands.Text() + " " + Flags + " " + """" + RomBox.Text() + """"
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
        OpenMovie.Filter = "Mednafen movie file (*.mcm)|*.mcm"
        OpenMovie.RestoreDirectory = False

        OpenMovie.ShowDialog()
        If (OpenMovie.FileName.Length) Then
            MovieBox.Text = OpenMovie.FileName
        End If
    End Sub

    Private Sub GetState()
        Dim OpenState As New OpenFileDialog
        OpenState.InitialDirectory = System.Environment.CurrentDirectory
        OpenState.Filter = "Savestate Files(*.ncs)|*.nc*"
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
        RomBox.Text = ""
        MovieBox.Text = ""
        OtherCommands.Text = ""
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
End Class
