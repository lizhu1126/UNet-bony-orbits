using System;
using System.Text;
using System.Windows;
using System.IO;
using System.Runtime.InteropServices;
//using System.Drawing;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;
using System.Collections.Generic;

namespace bonyImageResize
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {

        public static string m_sMultiImagesPath;
        static List<String> framePathList = new List<String>();
        int nSumFrame = 0;       //硬盘中，总的frame数
        public static string sIniFilePath1;
        List<String> noPrcsImage = new List<String>();      //未处理的图像
        List<String> calcuEratio = new List<String>();      //计算对角线时出错的图
        //List<String> ecEartAreas = new List<String>();      //计算分区面积中出错的图

        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string value, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int readImage(string imagePath);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int processImage(int length, string savePath);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int imageReady(string imagePath);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int calcuArea(string saveName); 
        [DllImport("bonyImage_DLL.dll")]
        private static extern int diagonalRatio(string saveName);
        [DllImport("bonyImage_DLL.dll")]
        private static extern void postProcess();
        [DllImport("bonyImage_DLL.dll")]
        private static extern int partAreas(string saveName);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int apertureWidth(string saveName);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int calcu1vec(string imagePath, string saveName);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int bboxWidth(string saveName);
        [DllImport("bonyImage_DLL.dll")]
        private static extern int bboxInfo(string saveName);

        [DllImport("bonyImage_DLL.dll")]
        private static extern int dataAugment(string imagePath, int cnt);

        


        //数据集预处理中的缩放函数
        private void bttn_MW_Process_Click(object sender, RoutedEventArgs e)
        {
            int ret = 0;
            //清除前一次选择的路径信息。再读入新选择的路径
            framePathList.Clear();
            director(m_sMultiImagesPath, ref framePathList);
            nSumFrame = framePathList.Count;

            //创建文件夹
            string folderName = "processed\\";
            string savePath = System.IO.Path.Combine(m_sMultiImagesPath, folderName);
            System.IO.Directory.CreateDirectory(savePath);

            //模板长度
            int tmpltLength = int.Parse(this.txtBx_MW_InputLength.Text);
            if (tmpltLength <= 0) System.Windows.Forms.MessageBox.Show("input length should be positive");

            for (int i = 0; i < nSumFrame; i++)
            {
                readImage(framePathList[i]);
                ret = processImage(tmpltLength, savePath);
                if(ret != 0)
                {
                    noPrcsImage.Add(framePathList[i]);
                }
            }

            if (noPrcsImage.Count == 0)
            {
                System.Windows.Forms.MessageBox.Show("some images have not processed");
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("end of calculation");
            }
            
        }

        //计算骨性眶缘的各项结构参数
        private void bttn_MW_Calcu_Click(object sender, RoutedEventArgs e)
        {
            int ret = 0;

            //清除前一次选择的路径信息。再读入新选择的路径
            framePathList.Clear();
            director(m_sMultiImagesPath, ref framePathList);
            nSumFrame = framePathList.Count;

            DateTime dt = DateTime.Now;
            string timeName = dt.Millisecond.ToString();
            string saveName = txtBx_MW_SaveName.Text + '_' + timeName;

            for (int i = 0; i < nSumFrame; i++)
            {
                ret = imageReady(framePathList[i]);     //准备图像，分为左右眼眶两图像
                if (ret != 0) System.Windows.Forms.MessageBox.Show("error in imageReady");

                //ret = diagonalRatio(saveName);             //上下、左右对角线及其比值
                //if (ret != 0)   calcuEratio.Add(framePathList[i]);

                ret = calcuArea(saveName);              //计算眼眶面积
                if (ret != 0) System.Windows.Forms.MessageBox.Show("error in calcuArea");

                //ret = partAreas(saveName);              //计算四分区面积
                //if (ret != 0) System.Windows.Forms.MessageBox.Show("error in partAreas");

                //ret = apertureWidth(saveName);          //计算眶口宽度等
                //if (ret != 0) System.Windows.Forms.MessageBox.Show("error in apertureWidth");

                //ret = bboxInfo(saveName);          //计算BBOX的眶口宽度、高度2021-5-21
                //if (ret != 0) System.Windows.Forms.MessageBox.Show("error in bboxInfo");

                ret = bboxWidth(saveName);          //计算BBOX的9等分2021-5-21
                if (ret != 0) System.Windows.Forms.MessageBox.Show("error in bboxWidth");

                postProcess();               //后处理函数，释放内存等
            }



            //判断是否中途有错误
            if (calcuEratio.Count != 0)
            {
                System.Windows.Forms.MessageBox.Show("maybe some errors in diagonalRatio");
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("end of calculation");
            }



            
        }



        //扩充数据集（抠取方案）
        private void bttn_MW_DataAug_Click(object sender, RoutedEventArgs e)
        {
            int ret = 0;

            //清除前一次选择的路径信息。再读入新选择的路径
            framePathList.Clear();
            director(m_sMultiImagesPath, ref framePathList);
            nSumFrame = framePathList.Count;

            for (int i = 0; i < nSumFrame; i++)
            {
                //对每张图片扩充并保存到文件夹中
                ret = dataAugment(framePathList[i], i);
            }



            System.Windows.Forms.MessageBox.Show("end of data augmentation");

        }







        public MainWindow()
        {
            InitializeComponent();

            this.Left = (SystemParameters.PrimaryScreenWidth - this.Width) / 2;
            this.Top = SystemParameters.PrimaryScreenHeight / 8;

            string sAppPath = System.Environment.CurrentDirectory; ;
            string sIniFilePath = sAppPath + "\\Config.ini";

            sIniFilePath1 = sIniFilePath;

            if (!File.Exists(sIniFilePath))
            {
                //如果不存在
                System.Windows.MessageBox.Show("配置文件不存在，创建配置文件");
                FileStream ConfigFile = new FileStream(@sIniFilePath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
                //TODO:文件保存权限等引起的创建异常
                ConfigFile.Close();
            }
            else
            {
                writeIni2Dlg();
            }

        }

        private void bttn_readMultiImgs_Click(object sender, RoutedEventArgs e)
        {
            //连续处理多张图像时，输入路径
            FolderBrowserDialog Dialog = new FolderBrowserDialog();
            DialogResult result = Dialog.ShowDialog();

            if (result == System.Windows.Forms.DialogResult.Cancel)
            {
                return;
            }
            m_sMultiImagesPath = Dialog.SelectedPath.Trim();
            this.txtBx_MW_multiImgs.Text = m_sMultiImagesPath;
        }


        /********************************************************
        -功能
        获取该路径下所有文件的文件路径
        -返回值
        -备注
        ********************************************************/
        public static void director(string dirs, ref List<String> fileList)
        {
            //绑定到指定的文件夹目录
            DirectoryInfo dir = new DirectoryInfo(dirs);
            //检索表示当前目录的文件和子目录
            FileSystemInfo[] fsinfos = dir.GetFileSystemInfos();
            //遍历检索的文件和子目录
            foreach (FileSystemInfo fsinfo in fsinfos)
            {
                //判断是否为空文件夹　　
                if (fsinfo is DirectoryInfo)
                {
                    //递归调用
                    director(fsinfo.FullName, ref fileList);
                }
                else
                {
                    //Console.WriteLine(fsinfo.FullName);
                    //将得到的文件全路径放入到集合中
                    fileList.Add(fsinfo.FullName);
                }
            }
        }


        private void writeIni2Dlg()
        {
            //配置文件信息写入对话框
            //获得当前目录,创建ini文件路径
            txtBx_MW_multiImgs.Text = ReadIni(sIniFilePath1, "Dialog_Parameters", "Muli_Images_Path");
            m_sMultiImagesPath = txtBx_MW_multiImgs.Text;

            txtBx_MW_InputLength.Text = ReadIni(sIniFilePath1, "Dialog_Parameters", "Input_Path");
            txtBx_MW_SaveName.Text = ReadIni(sIniFilePath1, "Dialog_Parameters", "SaveName");
        }


        private void writeDlg2Ini()
        {
            //对话框写入配置文件信息
            WritePrivateProfileString("Dialog_Parameters", "Muli_Images_Path", txtBx_MW_multiImgs.Text, sIniFilePath1);
            WritePrivateProfileString("Dialog_Parameters", "Input_Path", txtBx_MW_InputLength.Text, sIniFilePath1);
            WritePrivateProfileString("Dialog_Parameters", "SaveName", txtBx_MW_SaveName.Text, sIniFilePath1);

        }

        public static string ReadIni(string Path, string section, string key)
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(section, key, "", temp, 255, Path);
            return temp.ToString();
        }

        private void Bttn_MW_Close_Click(object sender, RoutedEventArgs e)
        {
            //关闭窗口
            writeDlg2Ini();
            System.Windows.Application.Current.Shutdown();

        }

        //计算一维向量，距离
        private void bttn_MW_Calcu1Vec_Click(object sender, RoutedEventArgs e)
        {
            int ret = 0;

            //清除前一次选择的路径信息。再读入新选择的路径
            framePathList.Clear();
            director(m_sMultiImagesPath, ref framePathList);
            nSumFrame = framePathList.Count;

            DateTime dt = DateTime.Now;
            string timeName = dt.Millisecond.ToString();
            string saveName = txtBx_MW_SaveName.Text + '_' + timeName;

            for (int i = 0; i < nSumFrame; i++)
            {
                ret = calcu1vec(framePathList[i], saveName);             //计算单眼眶的一维距离向量
                if (ret != 0) System.Windows.Forms.MessageBox.Show("error in calcu");

                postProcess();               //后处理函数，释放内存等
            }





            System.Windows.Forms.MessageBox.Show("end of calculation");
        }
    }
}
