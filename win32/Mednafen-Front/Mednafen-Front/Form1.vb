Public Class Form1
    'Globals
    Dim MEDNAFEN As String = "mednafen.exe "
    Dim MOVIE As String = "-mov "
    Dim CommandLine As String = ""      'Command line to run

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

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
        UpdateCommandLine()
        CommandLine = CommandBox.Text()
        ''Launch mednafen using CommandLine
        Shell(CommandBox.Text(), AppWinStyle.NormalFocus, False, -1)
    End Sub

    Private Sub RomBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RomBrowse.Click
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

    Private Sub MovieBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieBrowse.Click
        Dim OpenMovie As New SaveFileDialog
        OpenMovie.InitialDirectory = System.Environment.CurrentDirectory
        OpenMovie.Filter = "Mednafen movie file (*.mcm)|*.mcm"
        OpenMovie.RestoreDirectory = False


        OpenMovie.ShowDialog()
        If (OpenMovie.FileName.Length) Then
            MovieBox.Text = OpenMovie.FileName
        End If

    End Sub

    'Subs
    Private Sub CloseIt()
        Me.Close()
    End Sub

    Private Sub UpdateCommandLine()
        Dim movieCommand As String = ""
        If (MovieBox.Text.Length()) Then
            movieCommand = MOVIE
        End If
        CommandBox.Text = "\"""

        CommandBox.Text = MEDNAFEN + movieCommand + """" + MovieBox.Text() + """" + " " + OtherCommands.Text() + " " + """" + RomBox.Text() + """"
    End Sub
End Class
