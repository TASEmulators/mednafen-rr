Public Class About

    Private Sub About_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'Load emu and front versions dynamically from Front.vb
        EmuVersionLabel.Text = Front.EMUVERSION
        FrontVersionLabel.Text = Front.FRONTVERSION
    End Sub

    Private Sub OkButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OkButton.Click
        Me.Hide()
    End Sub
End Class