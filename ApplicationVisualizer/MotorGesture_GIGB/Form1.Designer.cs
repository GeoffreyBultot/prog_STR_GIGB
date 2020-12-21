namespace MotorGesture_GIGB
{
    partial class Form1
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.btn_SenAngle = new System.Windows.Forms.Button();
            this.btn_Connect = new System.Windows.Forms.Button();
            this.label_ServerIP = new System.Windows.Forms.Label();
            this.lblStatus = new System.Windows.Forms.Label();
            this.txtBox_ServerIP = new System.Windows.Forms.TextBox();
            this.txtBox_Angle = new System.Windows.Forms.TextBox();
            this.pictBox_Wheel = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.richTextBox_Telemetries = new System.Windows.Forms.RichTextBox();
            this.textBox_Telemetries = new System.Windows.Forms.TextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label_SetAngle = new System.Windows.Forms.Label();
            this.label_Telemetries = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictBox_Wheel)).BeginInit();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // btn_SenAngle
            // 
            this.btn_SenAngle.Font = new System.Drawing.Font("Microsoft YaHei", 14F, System.Drawing.FontStyle.Bold);
            this.btn_SenAngle.Location = new System.Drawing.Point(275, 7);
            this.btn_SenAngle.Margin = new System.Windows.Forms.Padding(4);
            this.btn_SenAngle.Name = "btn_SenAngle";
            this.btn_SenAngle.Size = new System.Drawing.Size(100, 41);
            this.btn_SenAngle.TabIndex = 3;
            this.btn_SenAngle.Text = "Send";
            this.btn_SenAngle.UseVisualStyleBackColor = true;
            this.btn_SenAngle.Click += new System.EventHandler(this.btn_SendAngle_Click);
            // 
            // btn_Connect
            // 
            this.btn_Connect.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Bold);
            this.btn_Connect.Location = new System.Drawing.Point(275, 12);
            this.btn_Connect.Margin = new System.Windows.Forms.Padding(4);
            this.btn_Connect.Name = "btn_Connect";
            this.btn_Connect.Size = new System.Drawing.Size(120, 27);
            this.btn_Connect.TabIndex = 1;
            this.btn_Connect.Text = "Connect";
            this.btn_Connect.UseVisualStyleBackColor = true;
            this.btn_Connect.Click += new System.EventHandler(this.btn_Connect_Click);
            // 
            // label_ServerIP
            // 
            this.label_ServerIP.AutoSize = true;
            this.label_ServerIP.Font = new System.Drawing.Font("Microsoft YaHei", 12F, System.Drawing.FontStyle.Bold);
            this.label_ServerIP.ForeColor = System.Drawing.Color.White;
            this.label_ServerIP.Location = new System.Drawing.Point(12, 12);
            this.label_ServerIP.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label_ServerIP.Name = "label_ServerIP";
            this.label_ServerIP.Size = new System.Drawing.Size(114, 27);
            this.label_ServerIP.TabIndex = 20;
            this.label_ServerIP.Text = "SERVER IP";
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Font = new System.Drawing.Font("Berlin Sans FB", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblStatus.ForeColor = System.Drawing.Color.Red;
            this.lblStatus.Location = new System.Drawing.Point(11, 59);
            this.lblStatus.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(337, 26);
            this.lblStatus.TabIndex = 20;
            this.lblStatus.Text = "Connection status : Disconnected";
            // 
            // txtBox_ServerIP
            // 
            this.txtBox_ServerIP.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Bold);
            this.txtBox_ServerIP.Location = new System.Drawing.Point(133, 11);
            this.txtBox_ServerIP.Margin = new System.Windows.Forms.Padding(4);
            this.txtBox_ServerIP.Name = "txtBox_ServerIP";
            this.txtBox_ServerIP.Size = new System.Drawing.Size(132, 27);
            this.txtBox_ServerIP.TabIndex = 0;
            this.txtBox_ServerIP.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.txtBox_ServerIP_PreviewKeyDown);
            // 
            // txtBox_Angle
            // 
            this.txtBox_Angle.Font = new System.Drawing.Font("Microsoft YaHei", 12F, System.Drawing.FontStyle.Bold);
            this.txtBox_Angle.Location = new System.Drawing.Point(147, 11);
            this.txtBox_Angle.Margin = new System.Windows.Forms.Padding(4);
            this.txtBox_Angle.Name = "txtBox_Angle";
            this.txtBox_Angle.Size = new System.Drawing.Size(119, 34);
            this.txtBox_Angle.TabIndex = 2;
            this.txtBox_Angle.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.txtBox_Angle_PreviewKeyDown);
            // 
            // pictBox_Wheel
            // 
            this.pictBox_Wheel.BackColor = System.Drawing.Color.Black;
            this.pictBox_Wheel.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pictBox_Wheel.BackgroundImage")));
            this.pictBox_Wheel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictBox_Wheel.ErrorImage = null;
            this.pictBox_Wheel.InitialImage = null;
            this.pictBox_Wheel.Location = new System.Drawing.Point(505, 20);
            this.pictBox_Wheel.Margin = new System.Windows.Forms.Padding(4);
            this.pictBox_Wheel.Name = "pictBox_Wheel";
            this.pictBox_Wheel.Size = new System.Drawing.Size(667, 615);
            this.pictBox_Wheel.TabIndex = 20;
            this.pictBox_Wheel.TabStop = false;
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Controls.Add(this.richTextBox_Telemetries);
            this.panel1.Controls.Add(this.textBox_Telemetries);
            this.panel1.Location = new System.Drawing.Point(36, 362);
            this.panel1.Margin = new System.Windows.Forms.Padding(4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(437, 256);
            this.panel1.TabIndex = 20;
            // 
            // richTextBox_Telemetries
            // 
            this.richTextBox_Telemetries.BackColor = System.Drawing.Color.Black;
            this.richTextBox_Telemetries.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.richTextBox_Telemetries.Font = new System.Drawing.Font("Microsoft YaHei", 12F, System.Drawing.FontStyle.Bold);
            this.richTextBox_Telemetries.ForeColor = System.Drawing.Color.Red;
            this.richTextBox_Telemetries.Location = new System.Drawing.Point(247, 4);
            this.richTextBox_Telemetries.Margin = new System.Windows.Forms.Padding(4);
            this.richTextBox_Telemetries.Name = "richTextBox_Telemetries";
            this.richTextBox_Telemetries.ReadOnly = true;
            this.richTextBox_Telemetries.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.richTextBox_Telemetries.Size = new System.Drawing.Size(189, 251);
            this.richTextBox_Telemetries.TabIndex = 20;
            this.richTextBox_Telemetries.Text = "0°\n0°\n0°\n0%\nOFF\nOFF\nOFF\nOFF\nRUNNING";
            // 
            // textBox_Telemetries
            // 
            this.textBox_Telemetries.BackColor = System.Drawing.Color.Black;
            this.textBox_Telemetries.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox_Telemetries.Font = new System.Drawing.Font("Microsoft YaHei", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_Telemetries.ForeColor = System.Drawing.Color.White;
            this.textBox_Telemetries.Location = new System.Drawing.Point(4, 4);
            this.textBox_Telemetries.Margin = new System.Windows.Forms.Padding(4);
            this.textBox_Telemetries.Multiline = true;
            this.textBox_Telemetries.Name = "textBox_Telemetries";
            this.textBox_Telemetries.ReadOnly = true;
            this.textBox_Telemetries.Size = new System.Drawing.Size(235, 251);
            this.textBox_Telemetries.TabIndex = 20;
            this.textBox_Telemetries.Text = "SET POINT :\r\nMEASURE :\r\nERROR :\r\nCOMMAND :\r\nREGULATION :\r\nCOMMUNICATION :\r\nANALOG" +
    " SUPPLY :\r\nNUMERIC SUPPLY :\r\nMOTOR STATE :";
            this.textBox_Telemetries.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.textBox_Telemetries.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel2.Controls.Add(this.label_SetAngle);
            this.panel2.Controls.Add(this.txtBox_Angle);
            this.panel2.Controls.Add(this.btn_SenAngle);
            this.panel2.Location = new System.Drawing.Point(36, 230);
            this.panel2.Margin = new System.Windows.Forms.Padding(4);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(437, 64);
            this.panel2.TabIndex = 20;
            // 
            // label_SetAngle
            // 
            this.label_SetAngle.AutoSize = true;
            this.label_SetAngle.Font = new System.Drawing.Font("Microsoft YaHei", 14F, System.Drawing.FontStyle.Bold);
            this.label_SetAngle.ForeColor = System.Drawing.Color.White;
            this.label_SetAngle.Location = new System.Drawing.Point(9, 15);
            this.label_SetAngle.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label_SetAngle.Name = "label_SetAngle";
            this.label_SetAngle.Size = new System.Drawing.Size(126, 31);
            this.label_SetAngle.TabIndex = 20;
            this.label_SetAngle.Text = "Set angle";
            this.label_SetAngle.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label_Telemetries
            // 
            this.label_Telemetries.AutoSize = true;
            this.label_Telemetries.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Telemetries.ForeColor = System.Drawing.Color.DodgerBlue;
            this.label_Telemetries.Location = new System.Drawing.Point(31, 327);
            this.label_Telemetries.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label_Telemetries.Name = "label_Telemetries";
            this.label_Telemetries.Size = new System.Drawing.Size(167, 31);
            this.label_Telemetries.TabIndex = 20;
            this.label_Telemetries.Text = "Telemetries";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.ForeColor = System.Drawing.Color.DodgerBlue;
            this.label4.Location = new System.Drawing.Point(29, 196);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(124, 31);
            this.label4.TabIndex = 20;
            this.label4.Text = "Controls";
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel3.Controls.Add(this.label_ServerIP);
            this.panel3.Controls.Add(this.txtBox_ServerIP);
            this.panel3.Controls.Add(this.btn_Connect);
            this.panel3.Controls.Add(this.lblStatus);
            this.panel3.Location = new System.Drawing.Point(36, 54);
            this.panel3.Margin = new System.Windows.Forms.Padding(4);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(437, 120);
            this.panel3.TabIndex = 20;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.ForeColor = System.Drawing.Color.DodgerBlue;
            this.label5.Location = new System.Drawing.Point(36, 20);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(162, 31);
            this.label5.TabIndex = 20;
            this.label5.Text = "Connection";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.BackgroundImage")));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox1.Location = new System.Drawing.Point(1039, 575);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(133, 62);
            this.pictureBox1.TabIndex = 20;
            this.pictureBox1.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.ClientSize = new System.Drawing.Size(1185, 651);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label_Telemetries);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pictBox_Wheel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "5EN0601 - Motor Gesture - Bultot Geoffrey - Ishimaru Geoffrey";
            ((System.ComponentModel.ISupportInitialize)(this.pictBox_Wheel)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btn_SenAngle;
        private System.Windows.Forms.Button btn_Connect;
        private System.Windows.Forms.Label label_ServerIP;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.TextBox txtBox_ServerIP;
        private System.Windows.Forms.TextBox txtBox_Angle;
        private System.Windows.Forms.PictureBox pictBox_Wheel;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label_SetAngle;
        private System.Windows.Forms.Label label_Telemetries;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.RichTextBox richTextBox_Telemetries;
        private System.Windows.Forms.TextBox textBox_Telemetries;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

