﻿<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Front
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
        Me.components = New System.ComponentModel.Container
        Me.MenuStrip1 = New System.Windows.Forms.MenuStrip
        Me.FileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.LaunchMednafenToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator3 = New System.Windows.Forms.ToolStripSeparator
        Me.RomToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.MovieToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator
        Me.ExitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.OptionsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.ClearToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.SettingsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.CloseMednafenOnExitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.AutoRunToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator4 = New System.Windows.Forms.ToolStripSeparator
        Me.DirectoryOverridesToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.HelpToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.HelpToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator2 = New System.Windows.Forms.ToolStripSeparator
        Me.AboutToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.LaunchButton = New System.Windows.Forms.Button
        Me.ExitButton = New System.Windows.Forms.Button
        Me.OtherCommands = New System.Windows.Forms.TextBox
        Me.CommandBox = New System.Windows.Forms.TextBox
        Me.Label2 = New System.Windows.Forms.Label
        Me.Update = New System.Windows.Forms.Button
        Me.MovieBox = New System.Windows.Forms.TextBox
        Me.RomBox = New System.Windows.Forms.TextBox
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label4 = New System.Windows.Forms.Label
        Me.RomBrowse = New System.Windows.Forms.Button
        Me.MovieBrowse = New System.Windows.Forms.Button
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.LengthLabel = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.ClearButton = New System.Windows.Forms.Button
        Me.TabControls = New System.Windows.Forms.TabControl
        Me.TabPage2 = New System.Windows.Forms.TabPage
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.RecordCheckBox = New System.Windows.Forms.CheckBox
        Me.PlayCheckBox = New System.Windows.Forms.CheckBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.AuthorBox = New System.Windows.Forms.TextBox
        Me.ReadonlyCheckBox = New System.Windows.Forms.CheckBox
        Me.TabPage1 = New System.Windows.Forms.TabPage
        Me.StateBrowse = New System.Windows.Forms.Button
        Me.StateBox = New System.Windows.Forms.TextBox
        Me.Label7 = New System.Windows.Forms.Label
        Me.PauseCheckBox = New System.Windows.Forms.CheckBox
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.MenuStrip1.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.TabControls.SuspendLayout()
        Me.TabPage2.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.TabPage1.SuspendLayout()
        Me.SuspendLayout()
        '
        'MenuStrip1
        '
        Me.MenuStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.FileToolStripMenuItem, Me.OptionsToolStripMenuItem, Me.SettingsToolStripMenuItem, Me.HelpToolStripMenuItem})
        Me.MenuStrip1.Location = New System.Drawing.Point(0, 0)
        Me.MenuStrip1.Name = "MenuStrip1"
        Me.MenuStrip1.Size = New System.Drawing.Size(511, 24)
        Me.MenuStrip1.TabIndex = 0
        Me.MenuStrip1.Text = "MenuStrip1"
        '
        'FileToolStripMenuItem
        '
        Me.FileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.LaunchMednafenToolStripMenuItem, Me.ToolStripSeparator3, Me.RomToolStripMenuItem, Me.MovieToolStripMenuItem, Me.ToolStripSeparator1, Me.ExitToolStripMenuItem})
        Me.FileToolStripMenuItem.Name = "FileToolStripMenuItem"
        Me.FileToolStripMenuItem.Size = New System.Drawing.Size(35, 20)
        Me.FileToolStripMenuItem.Text = "&File"
        '
        'LaunchMednafenToolStripMenuItem
        '
        Me.LaunchMednafenToolStripMenuItem.Name = "LaunchMednafenToolStripMenuItem"
        Me.LaunchMednafenToolStripMenuItem.Size = New System.Drawing.Size(170, 22)
        Me.LaunchMednafenToolStripMenuItem.Text = "Launch Mednafen"
        Me.LaunchMednafenToolStripMenuItem.ToolTipText = "Open/Closes mednafen.exe"
        '
        'ToolStripSeparator3
        '
        Me.ToolStripSeparator3.Name = "ToolStripSeparator3"
        Me.ToolStripSeparator3.Size = New System.Drawing.Size(167, 6)
        '
        'RomToolStripMenuItem
        '
        Me.RomToolStripMenuItem.Enabled = False
        Me.RomToolStripMenuItem.Name = "RomToolStripMenuItem"
        Me.RomToolStripMenuItem.Size = New System.Drawing.Size(170, 22)
        Me.RomToolStripMenuItem.Text = "&Change Rom..."
        Me.RomToolStripMenuItem.ToolTipText = "Specifies a new ROM to load into mednafen.exe.   Mednafen must be open."
        '
        'MovieToolStripMenuItem
        '
        Me.MovieToolStripMenuItem.Enabled = False
        Me.MovieToolStripMenuItem.Name = "MovieToolStripMenuItem"
        Me.MovieToolStripMenuItem.Size = New System.Drawing.Size(170, 22)
        Me.MovieToolStripMenuItem.Text = "&Change Movie..."
        Me.MovieToolStripMenuItem.ToolTipText = "Specifies a new movie for mednafen.exe to use.  Mednafen must be open."
        '
        'ToolStripSeparator1
        '
        Me.ToolStripSeparator1.Name = "ToolStripSeparator1"
        Me.ToolStripSeparator1.Size = New System.Drawing.Size(167, 6)
        '
        'ExitToolStripMenuItem
        '
        Me.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem"
        Me.ExitToolStripMenuItem.Size = New System.Drawing.Size(170, 22)
        Me.ExitToolStripMenuItem.Text = "E&xit"
        '
        'OptionsToolStripMenuItem
        '
        Me.OptionsToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ClearToolStripMenuItem})
        Me.OptionsToolStripMenuItem.Name = "OptionsToolStripMenuItem"
        Me.OptionsToolStripMenuItem.Size = New System.Drawing.Size(56, 20)
        Me.OptionsToolStripMenuItem.Text = "&Options"
        '
        'ClearToolStripMenuItem
        '
        Me.ClearToolStripMenuItem.Name = "ClearToolStripMenuItem"
        Me.ClearToolStripMenuItem.Size = New System.Drawing.Size(152, 22)
        Me.ClearToolStripMenuItem.Text = "&Clear"
        Me.ClearToolStripMenuItem.ToolTipText = "Clears all fields."
        '
        'SettingsToolStripMenuItem
        '
        Me.SettingsToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.CloseMednafenOnExitToolStripMenuItem, Me.AutoRunToolStripMenuItem, Me.ToolStripSeparator4, Me.DirectoryOverridesToolStripMenuItem})
        Me.SettingsToolStripMenuItem.Name = "SettingsToolStripMenuItem"
        Me.SettingsToolStripMenuItem.Size = New System.Drawing.Size(58, 20)
        Me.SettingsToolStripMenuItem.Text = "&Settings"
        '
        'CloseMednafenOnExitToolStripMenuItem
        '
        Me.CloseMednafenOnExitToolStripMenuItem.Name = "CloseMednafenOnExitToolStripMenuItem"
        Me.CloseMednafenOnExitToolStripMenuItem.Size = New System.Drawing.Size(198, 22)
        Me.CloseMednafenOnExitToolStripMenuItem.Text = "Close Mednafen on Exit"
        Me.CloseMednafenOnExitToolStripMenuItem.ToolTipText = "Specifies to close mednafen.exe on exit."
        '
        'AutoRunToolStripMenuItem
        '
        Me.AutoRunToolStripMenuItem.Name = "AutoRunToolStripMenuItem"
        Me.AutoRunToolStripMenuItem.Size = New System.Drawing.Size(198, 22)
        Me.AutoRunToolStripMenuItem.Text = "Auto-Run"
        Me.AutoRunToolStripMenuItem.ToolTipText = "This will cause mednafen.exe to be launched on startup."
        '
        'ToolStripSeparator4
        '
        Me.ToolStripSeparator4.Name = "ToolStripSeparator4"
        Me.ToolStripSeparator4.Size = New System.Drawing.Size(195, 6)
        '
        'DirectoryOverridesToolStripMenuItem
        '
        Me.DirectoryOverridesToolStripMenuItem.Name = "DirectoryOverridesToolStripMenuItem"
        Me.DirectoryOverridesToolStripMenuItem.Size = New System.Drawing.Size(198, 22)
        Me.DirectoryOverridesToolStripMenuItem.Text = "Directory overrides..."
        Me.DirectoryOverridesToolStripMenuItem.ToolTipText = "Set default directories such as where to find mednafen.exe"
        '
        'HelpToolStripMenuItem
        '
        Me.HelpToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.HelpToolStripMenuItem1, Me.ToolStripSeparator2, Me.AboutToolStripMenuItem})
        Me.HelpToolStripMenuItem.Name = "HelpToolStripMenuItem"
        Me.HelpToolStripMenuItem.Size = New System.Drawing.Size(40, 20)
        Me.HelpToolStripMenuItem.Text = "&Help"
        '
        'HelpToolStripMenuItem1
        '
        Me.HelpToolStripMenuItem1.Name = "HelpToolStripMenuItem1"
        Me.HelpToolStripMenuItem1.Size = New System.Drawing.Size(152, 22)
        Me.HelpToolStripMenuItem1.Text = "&Help"
        Me.HelpToolStripMenuItem1.ToolTipText = "Help and Mednafen-Front documentation."
        '
        'ToolStripSeparator2
        '
        Me.ToolStripSeparator2.Name = "ToolStripSeparator2"
        Me.ToolStripSeparator2.Size = New System.Drawing.Size(111, 6)
        '
        'AboutToolStripMenuItem
        '
        Me.AboutToolStripMenuItem.Name = "AboutToolStripMenuItem"
        Me.AboutToolStripMenuItem.Size = New System.Drawing.Size(114, 22)
        Me.AboutToolStripMenuItem.Text = "&About"
        '
        'LaunchButton
        '
        Me.LaunchButton.Location = New System.Drawing.Point(390, 266)
        Me.LaunchButton.Name = "LaunchButton"
        Me.LaunchButton.Size = New System.Drawing.Size(109, 23)
        Me.LaunchButton.TabIndex = 4
        Me.LaunchButton.Text = "Launch Mednafen"
        Me.ToolTip1.SetToolTip(Me.LaunchButton, "Opens/Closes mednafen.exe")
        Me.LaunchButton.UseVisualStyleBackColor = True
        '
        'ExitButton
        '
        Me.ExitButton.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.ExitButton.Location = New System.Drawing.Point(12, 301)
        Me.ExitButton.Name = "ExitButton"
        Me.ExitButton.Size = New System.Drawing.Size(46, 23)
        Me.ExitButton.TabIndex = 3
        Me.ExitButton.Text = "E&xit"
        Me.ExitButton.UseVisualStyleBackColor = True
        '
        'OtherCommands
        '
        Me.OtherCommands.Location = New System.Drawing.Point(12, 170)
        Me.OtherCommands.Name = "OtherCommands"
        Me.OtherCommands.Size = New System.Drawing.Size(416, 20)
        Me.OtherCommands.TabIndex = 6
        Me.ToolTip1.SetToolTip(Me.OtherCommands, "Add any additional commandlines you wish to specify here.")
        '
        'CommandBox
        '
        Me.CommandBox.Location = New System.Drawing.Point(10, 46)
        Me.CommandBox.Name = "CommandBox"
        Me.CommandBox.ReadOnly = True
        Me.CommandBox.Size = New System.Drawing.Size(411, 20)
        Me.CommandBox.TabIndex = 4
        Me.CommandBox.TabStop = False
        Me.CommandBox.Text = "mednafen.exe"
        Me.ToolTip1.SetToolTip(Me.CommandBox, "This is the commandline that will be launched")
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(9, 151)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(87, 13)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "Other commands"
        '
        'Update
        '
        Me.Update.Location = New System.Drawing.Point(344, 17)
        Me.Update.Name = "Update"
        Me.Update.Size = New System.Drawing.Size(75, 23)
        Me.Update.TabIndex = 7
        Me.Update.Text = "Update"
        Me.ToolTip1.SetToolTip(Me.Update, "Updates the commandline with the most recent values")
        Me.Update.UseVisualStyleBackColor = True
        '
        'MovieBox
        '
        Me.MovieBox.Location = New System.Drawing.Point(6, 19)
        Me.MovieBox.Name = "MovieBox"
        Me.MovieBox.Size = New System.Drawing.Size(335, 20)
        Me.MovieBox.TabIndex = 1
        Me.ToolTip1.SetToolTip(Me.MovieBox, "Specifies a movie file for mednafen to use for recording or playback")
        '
        'RomBox
        '
        Me.RomBox.Location = New System.Drawing.Point(9, 23)
        Me.RomBox.Name = "RomBox"
        Me.RomBox.Size = New System.Drawing.Size(335, 20)
        Me.RomBox.TabIndex = 1
        Me.ToolTip1.SetToolTip(Me.RomBox, "Specifies the ROM to be loaded.  This must be specified in order to launch mednaf" & _
                "en!")
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(9, 7)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(32, 13)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "ROM"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(3, 3)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(36, 13)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "Movie"
        '
        'RomBrowse
        '
        Me.RomBrowse.Location = New System.Drawing.Point(350, 21)
        Me.RomBrowse.Name = "RomBrowse"
        Me.RomBrowse.Size = New System.Drawing.Size(75, 23)
        Me.RomBrowse.TabIndex = 2
        Me.RomBrowse.Text = "Browse..."
        Me.RomBrowse.UseVisualStyleBackColor = True
        '
        'MovieBrowse
        '
        Me.MovieBrowse.Location = New System.Drawing.Point(347, 19)
        Me.MovieBrowse.Name = "MovieBrowse"
        Me.MovieBrowse.Size = New System.Drawing.Size(75, 23)
        Me.MovieBrowse.TabIndex = 2
        Me.MovieBrowse.Text = "Browse..."
        Me.MovieBrowse.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.LengthLabel)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.CommandBox)
        Me.GroupBox1.Controls.Add(Me.Update)
        Me.GroupBox1.Location = New System.Drawing.Point(12, 339)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(487, 90)
        Me.GroupBox1.TabIndex = 14
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "CommandLine"
        '
        'LengthLabel
        '
        Me.LengthLabel.AutoSize = True
        Me.LengthLabel.Location = New System.Drawing.Point(60, 22)
        Me.LengthLabel.Name = "LengthLabel"
        Me.LengthLabel.Size = New System.Drawing.Size(39, 13)
        Me.LengthLabel.TabIndex = 0
        Me.LengthLabel.Text = "Label6"
        Me.ToolTip1.SetToolTip(Me.LengthLabel, "Length of the commandline in characters.  The maximum limit is 255.")
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(11, 22)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(43, 13)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "Length:"
        '
        'ClearButton
        '
        Me.ClearButton.Location = New System.Drawing.Point(12, 266)
        Me.ClearButton.Name = "ClearButton"
        Me.ClearButton.Size = New System.Drawing.Size(46, 23)
        Me.ClearButton.TabIndex = 2
        Me.ClearButton.Text = "&Clear"
        Me.ToolTip1.SetToolTip(Me.ClearButton, "Clears all fields.")
        Me.ClearButton.UseVisualStyleBackColor = True
        '
        'TabControls
        '
        Me.TabControls.Controls.Add(Me.TabPage2)
        Me.TabControls.Controls.Add(Me.TabPage1)
        Me.TabControls.Location = New System.Drawing.Point(12, 37)
        Me.TabControls.Name = "TabControls"
        Me.TabControls.SelectedIndex = 0
        Me.TabControls.Size = New System.Drawing.Size(487, 223)
        Me.TabControls.TabIndex = 1
        '
        'TabPage2
        '
        Me.TabPage2.Controls.Add(Me.GroupBox2)
        Me.TabPage2.Controls.Add(Me.Label6)
        Me.TabPage2.Controls.Add(Me.AuthorBox)
        Me.TabPage2.Controls.Add(Me.ReadonlyCheckBox)
        Me.TabPage2.Controls.Add(Me.Label4)
        Me.TabPage2.Controls.Add(Me.MovieBox)
        Me.TabPage2.Controls.Add(Me.MovieBrowse)
        Me.TabPage2.Location = New System.Drawing.Point(4, 22)
        Me.TabPage2.Name = "TabPage2"
        Me.TabPage2.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage2.Size = New System.Drawing.Size(479, 197)
        Me.TabPage2.TabIndex = 1
        Me.TabPage2.Text = "Movie"
        Me.TabPage2.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.RecordCheckBox)
        Me.GroupBox2.Controls.Add(Me.PlayCheckBox)
        Me.GroupBox2.Location = New System.Drawing.Point(7, 119)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(122, 59)
        Me.GroupBox2.TabIndex = 6
        Me.GroupBox2.TabStop = False
        '
        'RecordCheckBox
        '
        Me.RecordCheckBox.AutoSize = True
        Me.RecordCheckBox.Location = New System.Drawing.Point(7, 34)
        Me.RecordCheckBox.Name = "RecordCheckBox"
        Me.RecordCheckBox.Size = New System.Drawing.Size(92, 17)
        Me.RecordCheckBox.TabIndex = 6
        Me.RecordCheckBox.Text = "Record movie"
        Me.ToolTip1.SetToolTip(Me.RecordCheckBox, "Specified movie will be recording on start up with this is checked.   If no movie" & _
                " is specified a default one will be made.")
        Me.RecordCheckBox.UseVisualStyleBackColor = True
        '
        'PlayCheckBox
        '
        Me.PlayCheckBox.AutoSize = True
        Me.PlayCheckBox.Location = New System.Drawing.Point(6, 10)
        Me.PlayCheckBox.Name = "PlayCheckBox"
        Me.PlayCheckBox.Size = New System.Drawing.Size(77, 17)
        Me.PlayCheckBox.TabIndex = 5
        Me.PlayCheckBox.Text = "Play movie"
        Me.ToolTip1.SetToolTip(Me.PlayCheckBox, "Specifies to begin playback of specified movie.  Movie must be specified for this" & _
                " to have any effect")
        Me.PlayCheckBox.UseVisualStyleBackColor = True
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(7, 57)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(38, 13)
        Me.Label6.TabIndex = 0
        Me.Label6.Text = "Author"
        '
        'AuthorBox
        '
        Me.AuthorBox.Location = New System.Drawing.Point(7, 76)
        Me.AuthorBox.Name = "AuthorBox"
        Me.AuthorBox.Size = New System.Drawing.Size(334, 20)
        Me.AuthorBox.TabIndex = 3
        Me.ToolTip1.SetToolTip(Me.AuthorBox, "Specifies an author's name to be added to the movie file.  This only needs to be " & _
                "included once.")
        '
        'ReadonlyCheckBox
        '
        Me.ReadonlyCheckBox.AutoSize = True
        Me.ReadonlyCheckBox.Checked = True
        Me.ReadonlyCheckBox.CheckState = System.Windows.Forms.CheckState.Checked
        Me.ReadonlyCheckBox.Location = New System.Drawing.Point(141, 129)
        Me.ReadonlyCheckBox.Name = "ReadonlyCheckBox"
        Me.ReadonlyCheckBox.Size = New System.Drawing.Size(76, 17)
        Me.ReadonlyCheckBox.TabIndex = 4
        Me.ReadonlyCheckBox.Text = "Read Only"
        Me.ToolTip1.SetToolTip(Me.ReadonlyCheckBox, "Specifies whether mednafen starts in read-only or read+write mode")
        Me.ReadonlyCheckBox.UseVisualStyleBackColor = True
        '
        'TabPage1
        '
        Me.TabPage1.Controls.Add(Me.StateBrowse)
        Me.TabPage1.Controls.Add(Me.StateBox)
        Me.TabPage1.Controls.Add(Me.Label7)
        Me.TabPage1.Controls.Add(Me.PauseCheckBox)
        Me.TabPage1.Controls.Add(Me.Label3)
        Me.TabPage1.Controls.Add(Me.RomBox)
        Me.TabPage1.Controls.Add(Me.RomBrowse)
        Me.TabPage1.Controls.Add(Me.Label2)
        Me.TabPage1.Controls.Add(Me.OtherCommands)
        Me.TabPage1.Location = New System.Drawing.Point(4, 22)
        Me.TabPage1.Name = "TabPage1"
        Me.TabPage1.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage1.Size = New System.Drawing.Size(479, 197)
        Me.TabPage1.TabIndex = 0
        Me.TabPage1.Text = "General"
        Me.TabPage1.UseVisualStyleBackColor = True
        '
        'StateBrowse
        '
        Me.StateBrowse.Location = New System.Drawing.Point(350, 73)
        Me.StateBrowse.Name = "StateBrowse"
        Me.StateBrowse.Size = New System.Drawing.Size(75, 23)
        Me.StateBrowse.TabIndex = 4
        Me.StateBrowse.Text = "Browse..."
        Me.StateBrowse.UseVisualStyleBackColor = True
        '
        'StateBox
        '
        Me.StateBox.Location = New System.Drawing.Point(12, 73)
        Me.StateBox.Name = "StateBox"
        Me.StateBox.Size = New System.Drawing.Size(332, 20)
        Me.StateBox.TabIndex = 3
        Me.ToolTip1.SetToolTip(Me.StateBox, "Specifies a state to be loaded on mednafen startup.  Warning, this is buggy when " & _
                "used in relation with Play Movie or Record Movie.")
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(9, 56)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(54, 13)
        Me.Label7.TabIndex = 0
        Me.Label7.Text = "Loadstate"
        '
        'PauseCheckBox
        '
        Me.PauseCheckBox.AutoSize = True
        Me.PauseCheckBox.Location = New System.Drawing.Point(12, 118)
        Me.PauseCheckBox.Name = "PauseCheckBox"
        Me.PauseCheckBox.Size = New System.Drawing.Size(56, 17)
        Me.PauseCheckBox.TabIndex = 5
        Me.PauseCheckBox.Text = "Pause"
        Me.PauseCheckBox.UseVisualStyleBackColor = True
        '
        'Front
        '
        Me.AcceptButton = Me.LaunchButton
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.ExitButton
        Me.ClientSize = New System.Drawing.Size(511, 456)
        Me.Controls.Add(Me.TabControls)
        Me.Controls.Add(Me.ClearButton)
        Me.Controls.Add(Me.ExitButton)
        Me.Controls.Add(Me.LaunchButton)
        Me.Controls.Add(Me.MenuStrip1)
        Me.Controls.Add(Me.GroupBox1)
        Me.MainMenuStrip = Me.MenuStrip1
        Me.Name = "Front"
        Me.Text = "Mednafen rerecording Frontend"
        Me.MenuStrip1.ResumeLayout(False)
        Me.MenuStrip1.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.TabControls.ResumeLayout(False)
        Me.TabPage2.ResumeLayout(False)
        Me.TabPage2.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.TabPage1.ResumeLayout(False)
        Me.TabPage1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents MenuStrip1 As System.Windows.Forms.MenuStrip
    Friend WithEvents FileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents RomToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents MovieToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents OptionsToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents HelpToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents HelpToolStripMenuItem1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents AboutToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ExitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents LaunchButton As System.Windows.Forms.Button
    Friend WithEvents ExitButton As System.Windows.Forms.Button
    Friend WithEvents OtherCommands As System.Windows.Forms.TextBox
    Friend WithEvents CommandBox As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Update As System.Windows.Forms.Button
    Friend WithEvents MovieBox As System.Windows.Forms.TextBox
    Friend WithEvents RomBox As System.Windows.Forms.TextBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents RomBrowse As System.Windows.Forms.Button
    Friend WithEvents MovieBrowse As System.Windows.Forms.Button
    Friend WithEvents ToolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents ToolStripSeparator2 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents LengthLabel As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents ClearButton As System.Windows.Forms.Button
    Friend WithEvents ClearToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents SettingsToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents CloseMednafenOnExitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents TabControls As System.Windows.Forms.TabControl
    Friend WithEvents TabPage2 As System.Windows.Forms.TabPage
    Friend WithEvents TabPage1 As System.Windows.Forms.TabPage
    Friend WithEvents PauseCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents ReadonlyCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents PlayCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents AuthorBox As System.Windows.Forms.TextBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents StateBrowse As System.Windows.Forms.Button
    Friend WithEvents StateBox As System.Windows.Forms.TextBox
    Friend WithEvents AutoRunToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents LaunchMednafenToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripSeparator3 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents RecordCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents ToolStripSeparator4 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents DirectoryOverridesToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolTip1 As System.Windows.Forms.ToolTip

End Class