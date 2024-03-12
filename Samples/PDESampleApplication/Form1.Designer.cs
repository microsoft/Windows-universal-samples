namespace PdeTest
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.btnProtectBufferL1 = new System.Windows.Forms.Button();
            this.btnUnprotectBuffer = new System.Windows.Forms.Button();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.txtSelectedFiles = new System.Windows.Forms.TextBox();
            this.btnSelectFiles = new System.Windows.Forms.Button();
            this.btnProtectBufferL2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(12, 230);
            this.button1.Margin = new System.Windows.Forms.Padding(2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(180, 50);
            this.button1.TabIndex = 0;
            this.button1.Text = "Protect file(s) to AfterFirstUnlock";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // textBox1
            // 
            this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox1.BackColor = System.Drawing.Color.Black;
            this.textBox1.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.ForeColor = System.Drawing.Color.Lime;
            this.textBox1.Location = new System.Drawing.Point(12, 284);
            this.textBox1.Margin = new System.Windows.Forms.Padding(2);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(703, 184);
            this.textBox1.TabIndex = 1;
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.Multiselect = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(196, 230);
            this.button2.Margin = new System.Windows.Forms.Padding(2);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(180, 50);
            this.button2.TabIndex = 2;
            this.button2.Text = "Protect file(s) to WhileUnlocked";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(381, 230);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(180, 50);
            this.button3.TabIndex = 3;
            this.button3.Text = "Unprotect file(s)";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.button4.Location = new System.Drawing.Point(566, 230);
            this.button4.Margin = new System.Windows.Forms.Padding(2);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(149, 51);
            this.button4.TabIndex = 4;
            this.button4.Text = "Query Data Availability";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // textBox2
            // 
            this.textBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox2.Location = new System.Drawing.Point(382, 11);
            this.textBox2.Multiline = true;
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(332, 51);
            this.textBox2.TabIndex = 5;
            this.textBox2.Text = "Protect my world!";
            // 
            // btnProtectBufferL1
            // 
            this.btnProtectBufferL1.Location = new System.Drawing.Point(11, 11);
            this.btnProtectBufferL1.Margin = new System.Windows.Forms.Padding(2);
            this.btnProtectBufferL1.Name = "btnProtectBufferL1";
            this.btnProtectBufferL1.Size = new System.Drawing.Size(181, 51);
            this.btnProtectBufferL1.TabIndex = 6;
            this.btnProtectBufferL1.Text = "Protect string to AfterFirstUnlock";
            this.btnProtectBufferL1.UseVisualStyleBackColor = true;
            this.btnProtectBufferL1.Click += new System.EventHandler(this.btnProtectBufferL1_Click);
            // 
            // btnUnprotectBuffer
            // 
            this.btnUnprotectBuffer.Location = new System.Drawing.Point(12, 66);
            this.btnUnprotectBuffer.Margin = new System.Windows.Forms.Padding(2);
            this.btnUnprotectBuffer.Name = "btnUnprotectBuffer";
            this.btnUnprotectBuffer.Size = new System.Drawing.Size(364, 105);
            this.btnUnprotectBuffer.TabIndex = 7;
            this.btnUnprotectBuffer.Text = "Unprotect string:";
            this.btnUnprotectBuffer.UseVisualStyleBackColor = true;
            this.btnUnprotectBuffer.Click += new System.EventHandler(this.btnUnprotectBuffer_Click);
            // 
            // textBox3
            // 
            this.textBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox3.Location = new System.Drawing.Point(382, 68);
            this.textBox3.Multiline = true;
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(333, 103);
            this.textBox3.TabIndex = 8;
            // 
            // txtSelectedFiles
            // 
            this.txtSelectedFiles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSelectedFiles.Location = new System.Drawing.Point(196, 175);
            this.txtSelectedFiles.Multiline = true;
            this.txtSelectedFiles.Name = "txtSelectedFiles";
            this.txtSelectedFiles.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtSelectedFiles.Size = new System.Drawing.Size(519, 51);
            this.txtSelectedFiles.TabIndex = 10;
            // 
            // btnSelectFiles
            // 
            this.btnSelectFiles.Location = new System.Drawing.Point(12, 175);
            this.btnSelectFiles.Margin = new System.Windows.Forms.Padding(2);
            this.btnSelectFiles.Name = "btnSelectFiles";
            this.btnSelectFiles.Size = new System.Drawing.Size(180, 51);
            this.btnSelectFiles.TabIndex = 11;
            this.btnSelectFiles.Text = "Select files..";
            this.btnSelectFiles.UseVisualStyleBackColor = true;
            this.btnSelectFiles.Click += new System.EventHandler(this.btnSelectFiles_Click);
            // 
            // btnProtectBufferL2
            // 
            this.btnProtectBufferL2.Location = new System.Drawing.Point(196, 11);
            this.btnProtectBufferL2.Margin = new System.Windows.Forms.Padding(2);
            this.btnProtectBufferL2.Name = "btnProtectBufferL2";
            this.btnProtectBufferL2.Size = new System.Drawing.Size(181, 51);
            this.btnProtectBufferL2.TabIndex = 12;
            this.btnProtectBufferL2.Text = "Protect string to WhileUnlocked";
            this.btnProtectBufferL2.UseVisualStyleBackColor = true;
            this.btnProtectBufferL2.Click += new System.EventHandler(this.btnProtectBufferL2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(727, 479);
            this.Controls.Add(this.btnProtectBufferL2);
            this.Controls.Add(this.btnSelectFiles);
            this.Controls.Add(this.txtSelectedFiles);
            this.Controls.Add(this.textBox3);
            this.Controls.Add(this.btnUnprotectBuffer);
            this.Controls.Add(this.btnProtectBufferL1);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button1);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "Form1";
            this.Text = "Personal Data Encryption Test";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button btnProtectBufferL1;
        private System.Windows.Forms.Button btnUnprotectBuffer;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox txtSelectedFiles;
        private System.Windows.Forms.Button btnSelectFiles;
        private System.Windows.Forms.Button btnProtectBufferL2;
    }
}

