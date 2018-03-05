using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace WinForm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            imgHandles = new List<IntPtr>();

            var files = Directory.GetFiles(Path.GetFullPath(@"..\..\..\..\bvh files\"), "*.bvh", SearchOption.AllDirectories);
            foreach (var file in files)
            {
                listView1.Items.Add(file);
            }
            //listView1.MouseDoubleClick += ListView1_MouseDoubleClick;
            listView1.ItemSelectionChanged += ListView1_ItemSelectionChanged;
            //listView1
            //  this.Size = new Size(512, 512);
            //GetFrame();
        }

        private void ListView1_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            //throw new NotImplementedException();
            if (e.IsSelected)
                GetFrame();
        }

        private void ListView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            GetFrame();
        }

        private List<IntPtr> imgHandles;
        const ulong BAD_POINTER = 0xcdcdcdcdcdcdcdcd;

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            buttonFreeMemory.PerformClick();

            base.OnFormClosing(e);
        }


        private void GetFrame()
        {
            if (listView1.SelectedItems.Count == 0)
                return;

            //string fileName;
            //          string fileName = " ";
            //  string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\13-down zigzag.bvh");
            // string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\15-eating-3.bvh");
            //  string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\some-motion.BVH.bvh");
            //string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\crazy_1.bvh");
            //string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\BALLET5.BVH");
            string fileName = listView1.SelectedItems[0].Text;
            // string fileName = Path.GetFullPath(@"..\..\..\..\bvh files\balet.bvh");

            int pixWidtg = pictureBox1.Width;
            int pixHight = pictureBox1.Height;
            int frameNamb = (int)numericUpDown1.Value;
            //frameNamb = 0;
            //frameNamb = GetNumFrame();
            //imgHandles.Clear();
            buttonFreeMemory.PerformClick();

            var isexist = File.Exists(fileName);
            Trace.WriteLine(isexist);
            Trace.WriteLine(fileName);
            Trace.WriteLine(this.Size);
            Trace.WriteLine("");

            var control = new Control();
            control.Size = new Size(pixWidtg, pixHight);
            //Graphics graf = this.pictureBox1.CreateGraphics();
            Graphics graf = control.CreateGraphics();

            //var img = new Bitmap(pixWidtg, pixHight);
            //var graf = Graphics.FromImage(img);

            //graf.Transform= new System.Drawing.Drawing2D.Matrix(pixWidtg as float, pixHight as float,);
            //graf.Clip = new Region(new Rectangle(0, 0, pixWidtg, pixHight));
            var hdc = graf.GetHdc();


            IntPtr handle;
            handle = (IntPtr)24653940;
            handle = pictureBox1.Handle;
            handle = control.Handle;

            //var hdc = GetWindowDC(handle);
            unsafe
            {

                if (hdc != null)
                {
                    IntPtr* hbitmapsPointer = GetHbitmapFrom(ref frameNamb, pixWidtg, pixHight, hdc, fileName);

                    if (hbitmapsPointer != null)// && hbitmapsPointer != (IntPtr*)IntPtr.Zero)
                    {
                        IntPtr res = (IntPtr)(-1);
                        int index = 0;
                        while (res != IntPtr.Zero && res != (IntPtr)BAD_POINTER)//&& index < frameNamb)
                        {
                            res = hbitmapsPointer[index];
                            if (res != IntPtr.Zero && res != (IntPtr)BAD_POINTER)
                            {
                                imgHandles.Add(res);
                                index++;
                            }
                        }
                    }
                    else
                    {
                        //int error = 0;
                        //var ptrErr = GetErrInfo(ref error);

                        //Trace.WriteLine(error);
                        //Trace.WriteLine(ptrErr);
                    }
                    graf.ReleaseHdc(hdc);

                    //ReleaseDC(pictureBox1.Handle, hdc);
                }
                graf.Dispose();

                int error = 0;
                int* ptrErr = GetErrInfo(ref error);
                int* ptrnull = &error;
                if (ptrErr == null)
                {
                    ptrErr = ptrnull;
                }
                labelInfo.Text = string.Format("Number of Frames:{0} | Errors Count:{1} | First Error:{2}", frameNamb, error, *ptrErr);
                int frame = 0;
                if (imgHandles != null && imgHandles.Count > frame && imgHandles[frame] != IntPtr.Zero)
                {
                    IntPtr res = imgHandles[frame];
                    var image = System.Drawing.Image.FromHbitmap(res);
                    this.pictureBox1.Image = image;
                    //image.Save("1.png");
                }
            }
        }

        #region DllImport

        [DllImport("user32.dll")]
        private static extern IntPtr GetWindowDC(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = false)]
        private static extern IntPtr GetDesktopWindow();


        [DllImport("user32.dll")]
        private static extern bool ReleaseDC(IntPtr hWnd, IntPtr hDC);


        [DllImport("BvhRender.dll")]
        private static extern unsafe IntPtr* GetHbitmapFrom(ref int frameQuantity, int pixWidtg, int pixHight, IntPtr hdc, string fileName);

        [DllImport("BvhRender.dll")]
        private static extern IntPtr GetHbitmap();


        [DllImport("BvhRender.dll")]
        private static extern void FreeMemory();

        [DllImport("BvhRender.dll")]
        private static extern unsafe int* GetErrInfo(ref int errNo);

        [DllImport("BvhRender.dll")]
        private static extern int GetInterval();

        [DllImport("BvhRender.dll")]
        private static extern int GetNumFrame();

        #endregion DllImport


        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            //GetFrame();

            int frame = (int)numericUpDown1.Value;

            if (imgHandles.Count > frame)
            {
                PlayFrame(ref frame);
            }
        }

        private void buttonFreeMemory_Click(object sender, EventArgs e)
        {

            Trace.WriteLine("FreeAllMemory: ");
            checkBoxPlay.Checked = false;

            if (imgHandles.Count == 0)
                return;


            imgHandles.Clear();

            if (this.pictureBox1.Image != null)
            {
                this.pictureBox1.Image.Dispose();
                this.pictureBox1.Image = null;
            }

            FreeMemory();

            //foreach (var handle in imgHandles)
            //{
            //    if (!DeleteObject(handle))
            //        Trace.WriteLine("  NOT DeleteObject !!!!!!!!!!");
            //}
            //UnloadModule("freeglut.dll");
            //UnloadModule("glew32.dll");
            //UnloadModule("glfw3.dll");
            //UnloadModule("ConsoleClassPlayer.dll");

            GC.Collect();
            //GC.WaitForPendingFinalizers();
            //GC.Collect();

            labelInfo.Text = string.Empty;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //for (int index = 0; index < 125; index++)
            //{
            //    Trace.WriteLine("Process: " + index);
            //    GetFrame();
            //}
            GetFrame();

            // GC.Collect();
            // GC.WaitForPendingFinalizers();
            // GC.Collect();
        }

        private void checkBoxPlay_CheckedChanged(object sender, EventArgs e)
        {
            if (!checkBoxPlay.Checked)
                return;

            int frame = 0;

            while (imgHandles.Count - 1 > frame && checkBoxPlay.Checked)
            {
                PlayFrame(ref frame);

                Thread.Sleep(100);
                if (imgHandles.Count - 1 <= frame)
                    frame = 0;

                Application.DoEvents();
            }
        }

        private void PlayFrame(ref int frame)
        {
            IntPtr res = imgHandles[frame];
            if (res != IntPtr.Zero && res != (IntPtr)BAD_POINTER)
            {
                try
                {
                    var image = Image.FromHbitmap(res);
                    if (image != null)
                    {
                        if (this.pictureBox1.Image != null)
                            this.pictureBox1.Image.Dispose();

                        this.pictureBox1.Image = image;
                    }
                }
                catch (Exception ex)
                {
                    Trace.WriteLine(ex);
                }
            }
            this.pictureBox1.Refresh();
            frame++;
        }
    }


}
