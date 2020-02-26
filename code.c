#include "opencv2/core/core.hpp"  
#include"opencv2/highgui/highgui.hpp"  
#include"opencv2/imgproc/imgproc.hpp"  
#include<iostream>
#include<opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"  
 
#define TRACKBARNAME "魔法棒大小"
#define RESULT "美颜后"
#define ORIGIN "美颜前"
 
using namespace cv;
 
//***************************************************************//
//全局变量定义
//***************************************************************//
bool g_bDrawing = false; //鼠标点击flag
Point g_CurrPoint, g_OrgPoint;//鼠标当前位置
int g_nThick , g_nBlue = 140, g_nGreen = 140, g_nRed = 140;//线型
RNG &rng = theRNG();
Mat dst_step1;//高美颜后的图
Mat dst;//高斯模糊图
Mat mengban1;//蒙版
Mat image;   //自拍后的图像名
Mat frame;   //视频显示桢
 
//****************************************************************//
//调用摄像头实现自拍函数
//***************************************************************//
void cameraCapture()
{
	char key;
	bool flag=1;	
	int i = 8;
	//打开摄像头
	VideoCapture capture(0);
	//大循环，为了截图后继续显示摄像头实时图片
	while (flag)
	{
		//给key一个初始值，便于循环一开始进行
		key = 'c';
		//判断key是否为空格，不为空格则循环，即显示摄像头
		while (key != ' ')
		{
			key = cvWaitKey(10);
			capture >> frame;			
			imshow("摄像头", frame);		
			waitKey(3);
			flag = 0;
		}
		//便于观察key是否有效
		imwrite("image.jpg", frame);
		key = 'c';
		destroyAllWindows();
	}
	
}
 
//****************************************************************//
//滑动条改变笔触大小的回调函数
//***************************************************************//
void changePenSize(int i,void *)
{
	g_nThick = i;
}
 
//****************************************************************//
//磨皮（高斯模糊）
//***************************************************************//
int GaussianBlur(){
	image = imread("image.jpg");
	int value1 = 3, value2 = 1; //磨皮程度与细节程度的确定
	int dx = value1 * 5;    //双边滤波参数之一  
	double fc = value1*12.5; //双边滤波参数之一  
	int p = 50; //透明度  
	Mat temp1, temp2, temp3, temp4;
	//双边滤波  
	bilateralFilter(image, temp1, dx, fc, fc);
	temp2 = (temp1 - image + 128);
	//高斯模糊  
	GaussianBlur(temp2, temp3, Size(2 * value2 - 1, 2 * value2 - 1), 0, 0);
	temp4 = image + 2 * temp3 - 255;
	dst = (image*(100 - p) + temp4*p) / 100;
	dst.copyTo(dst_step1);	
	return 0;
}
 
//***************************************************************//
//inpaint函数 手动选择杂色点
//***************************************************************//
void onMouse(int event, int x, int y, int flag, void *param)
{
	Mat &img = *(cv::Mat*)param;
	switch (event)
	{
		//移动鼠标的时候
	case CV_EVENT_MOUSEMOVE:
	{							   							
		g_OrgPoint = g_CurrPoint;
		g_CurrPoint = Point(x, y);
		if (g_bDrawing == 1)
         {				
			line(dst_step1, g_CurrPoint, g_OrgPoint, Scalar(255), g_nThick);
			imshow(RESULT, dst_step1);
			line(mengban1, g_CurrPoint, g_OrgPoint, Scalar(255), g_nThick);
		 }
	}
		break;
		//点击鼠标左键时
	case CV_EVENT_LBUTTONDOWN:
	{
			g_bDrawing = true;
			g_OrgPoint = Point(x, y);
			g_CurrPoint = g_OrgPoint;
	}
		break;
		//松开鼠标左键时
	case CV_EVENT_LBUTTONUP:
	{
			g_bDrawing = false;
			inpaint(dst_step1, mengban1, dst_step1, 3, INPAINT_NS);
			imshow(RESULT, dst_step1);
			imwrite("美颜结果图.jpg", dst_step1);
	}   
		break;
	}
}
 
//***************************************************************//
//程序说明
//***************************************************************//
void Introduction()
{
	printf("\n\\*****************************************************************************\\\n");
	printf("\\*****************************************************************************\\");
	printf("\n\t\t\t\t自拍美颜程序!\n");
	printf("\n\n功能介绍：\n\n1、自动调用摄像头\n2、按下空格键实现自拍\n3、自动美颜\n4、魔法笔点击图片杂色进行自动修复");
	printf("\n\n\t\t\t\t谢谢您的使用!\n");
	printf("\\*****************************************************************************\\\n");
	printf("\\*****************************************************************************\\\n");
}
 
//***************************************************************//
//主函数
//***************************************************************//
int main()
{
	//***************************************************************//
	//程序说明
	//***************************************************************//
	Introduction();
 
	//***************************************************************//
	//调用摄像头获取需要美颜的自拍
	//***************************************************************//
	cameraCapture();
	image = imread("image.jpg");
	imshow(ORIGIN, image);
 
	//***************************************************************//
	//图像磨皮功能
	//***************************************************************//
	GaussianBlur();
	
	//***************************************************************//
	//inpaint函数变量准备
	//***************************************************************//
	//用一个变量来存储原图像 为MASK 分配空间
	mengban1.create(dst_step1.size(), CV_8UC1);
	mengban1 = Scalar::all(0);
	namedWindow(RESULT);
	setMouseCallback(RESULT, onMouse, 0);
 
	//***************************************************************//
	//创建滑条改变笔触大小
	//***************************************************************//
	int originValue = 2;
	createTrackbar(TRACKBARNAME, RESULT, &originValue, 30, changePenSize);
	changePenSize(originValue,0);
 
	//***************************************************************//
	//美颜后的照片
	//***************************************************************//
	imshow(RESULT, dst_step1);
	waitKey();		
 
	return 0;
}