Public Class Front
    'Constants
    Dim MEDNAFEN As String = "mednafen.exe "
    Dim MOVIE As String = "-mov "
    Friend EMUVERSION As String = "Mednafen Rerecording 1.1"
    Friend FRONTVERSION As String = "Frontend 1.0"

    'Globals
    Dim CommandLine As String = ""      'Command line to run

    'Startup routine
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Text = EMUVERSION + " " + FRONTVERSION
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
        UpdateCommandLine()
        CommandLine = CommandBox.Text()
        ''Launch mednafen using CommandLine
        Shell(CommandBox.Text(), AppWinStyle.NormalFocus, False, -1)
    End Sub

    Private Sub RomBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RomBrowse.Click
        GetRom()
    End Sub

    Private Sub MovieBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieBrowse.Click
        GetMovie()
    End Sub

    Private Sub RomToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RomToolStripMenuItem.Click
        GetRom()
    End Sub

    Private Sub MovieToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MovieToolStripMenuItem.Click
        GetMovie()
    End Sub

    'Subs
    Private Sub CloseIt()
        SaveConfig()
        Me.Close()
    End Sub

    Private Sub UpdateCommandLine()
        Dim movieCommand As String = ""
        If (MovieBox.Text.Length()) Then
            movieCommand = """" & MOVIE & """"""
        End If
        CommandBox.Text = "\"""

        CommandBox.Text = MEDNAFEN + movieCommand + MovieBox.Text() + " " + OtherCommands.Text() + " " + """" + RomBox.Text() + """"
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

    'Open config file
    Private Sub OpenConfig()
        'Dim fileReader As String
        'fileReader = My.Computer.FileSystem.ReadAllText("med-front.cfg")
        'MsgBox(fileReader)



    End Sub

    'Save config file
    Private Sub SaveConfig()
        '

    End Sub

    'Load config file
    Private Sub LoadConfig()
        'Check if med-front.cfg exists, if not create an empty file, populate with dafult values, save,  and finish
        'If exists, pull out default values



    End Sub
End Class
