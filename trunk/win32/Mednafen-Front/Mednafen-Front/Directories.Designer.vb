<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Directories
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.MedBrowse = New System.Windows.Forms.Button
        Me.MednafenDirectoy = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Ok = New System.Windows.Forms.Button
        Me.Cancel = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'MedBrowse
        '
        Me.MedBrowse.Location = New System.Drawing.Point(388, 33)
        Me.MedBrowse.Name = "MedBrowse"
        Me.MedBrowse.Size = New System.Drawing.Size(75, 23)
        Me.MedBrowse.TabIndex = 2
        Me.MedBrowse.Text = "Browse..."
        Me.MedBrowse.UseVisualStyleBackColor = True
        '
        'MednafenDirectoy
        '
        Me.MednafenDirectoy.Location = New System.Drawing.Point(13, 36)
        Me.MednafenDirectoy.Name = "MednafenDirectoy"
        Me.MednafenDirectoy.Size = New System.Drawing.Size(369, 20)
        Me.MednafenDirectoy.TabIndex = 1
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(13, 13)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(77, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "mednafen.exe:"
        '
        'Ok
        '
        Me.Ok.Location = New System.Drawing.Point(275, 83)
        Me.Ok.Name = "Ok"
        Me.Ok.Size = New System.Drawing.Size(75, 23)
        Me.Ok.TabIndex = 3
        Me.Ok.Text = "&Ok"
        Me.Ok.UseVisualStyleBackColor = True
        '
        'Cancel
        '
        Me.Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Cancel.Location = New System.Drawing.Point(366, 83)
        Me.Cancel.Name = "Cancel"
        Me.Cancel.Size = New System.Drawing.Size(75, 23)
        Me.Cancel.TabIndex = 4
        Me.Cancel.Text = "&Cancel"
        Me.Cancel.UseVisualStyleBackColor = True
        '
        'Directories
        '
        Me.AcceptButton = Me.Ok
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.Cancel
        Me.ClientSize = New System.Drawing.Size(475, 118)
        Me.Controls.Add(Me.Cancel)
        Me.Controls.Add(Me.Ok)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.MednafenDirectoy)
        Me.Controls.Add(Me.MedBrowse)
        Me.Name = "Directories"
        Me.Text = "Directory Override"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents MedBrowse As System.Windows.Forms.Button
    Friend WithEvents MednafenDirectoy As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Ok As System.Windows.Forms.Button
    Friend WithEvents Cancel As System.Windows.Forms.Button
End Class
