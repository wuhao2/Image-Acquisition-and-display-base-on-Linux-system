
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

namespace global{ //������
	bool paused = true; //��ͣ��־
	Mat displayImg;
	bool selectObject = false; //�������Ƿ��±�־
	bool isRoiReady = 0;  //�Ƿ��Ѿ�ѡ�����
	Point origin;    //ROI�������Ͻ���ʼλ��
	Rect selectedRoi; //ѡ����������
	
	static void onMouse(int event, int x, int y, int, void*){
		//������£������˺��������ϵļ���Ŀ����εĴ���
		if (selectObject){
			selectedRoi.x = MIN(x, origin.x);
			selectedRoi.y = MIN(x, origin.y);
			selectedRoi.width = std::abs(x - origin.x);
			selectedRoi.height = std::abs(y - origin.y);
			//x��y����Խ��
			selectedRoi &= Rect(0, 0, displayImg.cols, displayImg.rows);
			//��������ѡ���
			rectangle(displayImg, selectedRoi, Scalar(0, 0, 255), 1);
		}

		switch (event)
		{
		//����������
		case CV_EVENT_MBUTTONDOWN :
			origin = Point(x,y);
			selectedRoi = Rect(x, y, 0, 0);
			selectObject = true;
			isRoiReady = false; 
			break;
		//�������ɿ�
		case CV_EVENT_LBUTTONUP :
			selectObject = false;
			if (selectedRoi.width > 0 && selectedRoi.height > 0){
				isRoiReady = true;
			cout << "Ŀ�������Ѿ�ѡ�����" << endl;
			cout << "ѡ�о�������Ϊ�� " << selectedRoi << endl;
			break;
			}
		//����Ҽ�����ͣ����ʼ
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
	//ʵ����һ����Ƶ��ȡ��
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

	//���ôӵڼ�֡��ʼ��ȡ
	int frameIndex = datasets::start_frame;
	//capture.set(VideoCaptureProperties::CAP_PROP_POS_FRAMES, double(frameIndex));
	capture.set(CV_CAP_PROP_POS_FRAMES, double(frameIndex));

	//��������
	const string winName = "Tracking Window";
	namedWindow(winName, 1);
	setMouseCallback(winName, global::onMouse, 0);//���ص���������Ϣѭ��

	//��ȡָ����ʼ֡
	Mat CurrentFrame, WorkFrame;
	capture >> CurrentFrame;
	CV_Assert(!CurrentFrame.empty());
	cout << "��ǰ֡������" << frameIndex << endl;
	frameIndex++;//������һ֡


	//����ʼ֡��ѡ��Ŀ������
	while (!global::isRoiReady){
		CurrentFrame.copyTo(global::displayImg);
		//�������������ɿ��ڼ䣬selectObjectΪ��
		//selectedRoi�����������ƶ����ϵı仯��֪��̧��������
		//selectObjectΪ�٣�selectedRoi����ѡ�е�Ŀ����ο�
		if (global:: selectObject && global::selectedRoi.width > 0 && global::selectedRoi.height > 0){
			Mat roi_img(global::displayImg, global::selectedRoi);
			cv::bitwise_not(roi_img, roi_img);//��ѡ�е�����ͼ������ֵȡ����������ʾ���˿�
		}
		imshow(winName, global::displayImg);//��ʾ���ѡ��Ĺ���
		waitKey(10);
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ʵ����Tracker�� ��ʼ��Ŀ�������init()
	//Ptr<mycv::Tracker> tracker = new mycv::Tracker();
	Ptr<mycv::Tracker> tracker = new mycv::SingleTemplateTracker(30,30);  //����ת����, 30,30,��ʾexpandWidth��expandWidth
	cvtColor(CurrentFrame, WorkFrame, CV_BGR2GRAY);                      //----��Ŀ������Ժܿ죬���������ֵ�Ŵ�һ�㣬����Ŀ���߳�������
	tracker->init(WorkFrame, global::selectedRoi);


	//����forѭ����������Ƶͼ�����У�����Ŀ��
	for(; frameIndex < FrameCount;){
		//û����ͣ
		if (!global::paused){
			capture >> CurrentFrame;
			CV_Assert(!CurrentFrame.empty());
			cout << "��ǰ֡����:" << frameIndex << endl;
			frameIndex++;
			//����ǰ֡������displayImg��
			CurrentFrame.copyTo(global::displayImg);
			cvtColor(CurrentFrame, WorkFrame, CV_BGR2GRAY);

			//��ʼ����track
			Rect CurrentBoundingBox;//Ŀ���
			tracker->track(WorkFrame,CurrentBoundingBox); //�����е�ʱ�򣬻��Զ�ת������Tracker���崦������ʱ�Ż��������

			//����Ŀ��ģ��update
			Rect NextSearchBox;
			tracker->update(NextSearchBox);

			//�����ͣ������ʾ��ǰ֡�ĸ��ٽ��
			rectangle(global::displayImg, NextSearchBox, Scalar(255, 0, 0), 2); //��ʾ��ɫ������  ------�ֲ������ŵ㣺�����������Ч�ʣ������������������������Ŀ��
			rectangle(global::displayImg, CurrentBoundingBox, Scalar(0, 0, 255), 2); //��ʾ��ɫĿ���
			imshow(winName, global::displayImg);	
			waitKey(300);
		}else{
			//�����ͣ������ʾ��ǰ֡�ĸ��ٽ��
			imshow(winName, global::displayImg);	
			waitKey(300);
		}		
	}
	return 0;
}


