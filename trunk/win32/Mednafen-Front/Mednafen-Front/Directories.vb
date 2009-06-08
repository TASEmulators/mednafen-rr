Public Class Directories
    'Globals
    Friend MednafenDirectoryString As String = ""


    Private Sub MedBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MedBrowse.Click
        Dim DirectoryBrowse As New FolderBrowserDialog
        DirectoryBrowse.Description = "Select the folder to look for mednafen.exe"
        DirectoryBrowse.SelectedPath = System.Environment.CurrentDirectory
        DirectoryBrowse.ShowDialog()

        If (DirectoryBrowse.SelectedPath.Length) Then
            MednafenDirectoy.Text = DirectoryBrowse.SelectedPath
        End If
    End Sub

    Private Sub Ok_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Ok.Click
        'Save All Directory Overrides
        MednafenDirectoryString = MednafenDirectoy.Text()

        'Close
        Me.Hide()
    End Sub

    Private Sub Cancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel.Click
        'Clear All directory values
        Me.Hide()
    End Sub

    Private Sub Directories_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        If (MednafenDirectoryString.Length()) Then
            MednafenDirectoy.Text = MednafenDirectoryString
        End If
    End Sub
End Class