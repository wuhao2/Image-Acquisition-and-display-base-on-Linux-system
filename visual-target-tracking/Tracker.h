#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>  
#include <string>
#include <vector>

using namespace std;  
using namespace cv;  

namespace mycv {

	class Tracker
	{
	public:
		Tracker(void);
		virtual ~Tracker(void);
		bool init(const Mat& initFrame, Rect& initBoundingBox);//��ʼ��������
		bool track(const Mat& currentFrame, Rect& currentBoundingBox);//����Ŀ��
		bool update(Rect& searchBox);//����Ŀ��ģ��
	};



}
