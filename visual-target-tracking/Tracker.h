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
		bool init(const Mat& initFrame, Rect& initBoundingBox);//初始化跟踪器
		bool track(const Mat& currentFrame, Rect& currentBoundingBox);//跟踪目标
		bool update(Rect& searchBox);//更新目标模型
	};



}
