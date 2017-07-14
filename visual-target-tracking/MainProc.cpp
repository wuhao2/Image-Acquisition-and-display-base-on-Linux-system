
//#include <opencv2/core/core.hpp>  
//#include<opencv2/highgui/highgui.hpp>  
//#include<opencv2/imgproc/imgproc.hpp>  

#include <opencv2\opencv.hpp>
#include <iostream>  
#include <string>
#include <vector>

#include "Tracker.h"
#include "SingleTemplateTracker.h"
	 
using namespace std;  
using namespace cv;  

namespace global{ //鼠标操作
	bool paused = true; //暂停标志
	Mat displayImg;
	bool selectObject = false; //鼠标左键是否按下标志
	bool isRoiReady = 0;  //是否已经选择好了
	Point origin;    //ROI区域左上角起始位置
	Rect selectedRoi; //选择的鼠标区域
	
	static void onMouse(int event, int x, int y, int, void*){
		//左键按下，触发此函数，不断的计算目标矩形的窗口
		if (selectObject){
			selectedRoi.x = MIN(x, origin.x);
			selectedRoi.y = MIN(x, origin.y);
			selectedRoi.width = std::abs(x - origin.x);
			selectedRoi.height = std::abs(y - origin.y);
			//x，y不能越界
			selectedRoi &= Rect(0, 0, displayImg.cols, displayImg.rows);
			//画出鼠标的选择框
			rectangle(displayImg, selectedRoi, Scalar(0, 0, 255), 1);
		}

		switch (event)
		{
		//鼠标左键按下
		case CV_EVENT_MBUTTONDOWN :
			origin = Point(x,y);
			selectedRoi = Rect(x, y, 0, 0);
			selectObject = true;
			isRoiReady = false; 
			break;
		//鼠标左键松开
		case CV_EVENT_LBUTTONUP :
			selectObject = false;
			if (selectedRoi.width > 0 && selectedRoi.height > 0){
				isRoiReady = true;
			cout << "目标区域已经选择完毕" << endl;
			cout << "选中举行区域为： " << selectedRoi << endl;
			break;
			}
		//鼠标右键，暂停，开始
		case CV_EVENT_RBUTTONDOWN :
			paused =! paused;
			break;		
		}
	}

}

namespace datasets{
	const string dataset_dir = "C:\\Users\\wuhao\\Documents\\";
	const string video1 = dataset_dir + "VID20170707144802.mp4";
	const string video2 = dataset_dir + "VID20170707144802.mp4";
	int video_start_frame = 0;

	const string video = video1;
	int start_frame = video_start_frame;


}

int main(int argc, char* argv[]){
	//实例化一个视频读取器
	VideoCapture capture;
	capture.open(datasets::video);
	CV_Assert(capture.isOpened());
	const int FrameCount = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);
	const int FrameWidth = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
	const int FrameHeight = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	/*const int FrameCount = (int)capture.get(VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
	const int FrameWidth = (int)capture.get(VideoCaptureProperties::CAP_PROP_FRAME_WIDTH);
	const int FrameHeight = (int)capture.get(VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT);*/
	const Rect FrameArea(0,0,FrameWidth, FrameHeight);

	//设置从第几帧开始读取
	int frameIndex = datasets::start_frame;
	//capture.set(VideoCaptureProperties::CAP_PROP_POS_FRAMES, double(frameIndex));
	capture.set(CV_CAP_PROP_POS_FRAMES, double(frameIndex));

	//创建窗口
	const string winName = "Tracking Window";
	namedWindow(winName, 1);
	setMouseCallback(winName, global::onMouse, 0);//鼠标回调函数，消息循环

	//读取指定起始帧
	Mat CurrentFrame, WorkFrame;
	capture >> CurrentFrame;
	CV_Assert(!CurrentFrame.empty());
	cout << "当前帧索引：" << frameIndex << endl;
	frameIndex++;//播放下一帧


	//在起始帧上选择目标区域
	while (!global::isRoiReady){
		CurrentFrame.copyTo(global::displayImg);
		//鼠标左键按下与松开期间，selectObject为真
		//selectedRoi会随着鼠标的移动不断的变化，知道抬起鼠标左键
		//selectObject为假，selectedRoi就是选中的目标矩形框
		if (global:: selectObject && global::selectedRoi.width > 0 && global::selectedRoi.height > 0){
			Mat roi_img(global::displayImg, global::selectedRoi);
			cv::bitwise_not(roi_img, roi_img);//把选中的区域图像像素值取反，用于显示给人看
		}
		imshow(winName, global::displayImg);//显示鼠标选择的过程
		waitKey(10);
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//实例化Tracker， 初始化目标跟踪器init()
	//Ptr<mycv::Tracker> tracker = new mycv::Tracker();
	Ptr<mycv::Tracker> tracker = new mycv::SingleTemplateTracker(30,30);  //子类转父类, 30,30,表示expandWidth，expandWidth
	cvtColor(CurrentFrame, WorkFrame, CV_BGR2GRAY);                      //----当目标机动性很快，则把这两个值放大一点，避免目标走出搜索框
	tracker->init(WorkFrame, global::selectedRoi);


	//进入for循环，处理视频图像序列，跟踪目标
	for(; frameIndex < FrameCount;){
		//没有暂停
		if (!global::paused){
			capture >> CurrentFrame;
			CV_Assert(!CurrentFrame.empty());
			cout << "当前帧索引:" << frameIndex << endl;
			frameIndex++;
			//将当前帧拷贝到displayImg中
			CurrentFrame.copyTo(global::displayImg);
			cvtColor(CurrentFrame, WorkFrame, CV_BGR2GRAY);

			//开始跟踪track
			Rect CurrentBoundingBox;//目标框
			tracker->track(WorkFrame,CurrentBoundingBox); //不运行的时候，会自动转到父类Tracker定义处，运行时才会调到子类

			//更新目标模型update
			Rect NextSearchBox;
			tracker->update(NextSearchBox);

			//如果暂停，则显示当前帧的跟踪结果
			rectangle(global::displayImg, NextSearchBox, Scalar(255, 0, 0), 2); //显示蓝色搜索框  ------局部搜索优点：极大提高搜索效率，避免二次搜索，更换了搜索目标
			rectangle(global::displayImg, CurrentBoundingBox, Scalar(0, 0, 255), 2); //显示红色目标框
			imshow(winName, global::displayImg);	
			waitKey(300);
		}else{
			//如果暂停，则显示当前帧的跟踪结果
			imshow(winName, global::displayImg);	
			waitKey(300);
		}		
	}
	return 0;
}


