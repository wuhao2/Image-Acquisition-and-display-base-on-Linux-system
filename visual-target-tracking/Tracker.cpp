#include "Tracker.h"
#include <iostream>  

namespace mycv{

	Tracker::Tracker(void)
	{
	}
	Tracker::~Tracker(void) 
	{
	}

	//初始化跟踪器
	bool Tracker::init(const Mat& initFrame,  Rect& initBoundingBox){
		return false;
	}
	//跟踪目标
	bool Tracker::track(const Mat& currentFrame,  Rect& currentBoundingBox){
		return false;
	}
	//更新模型
	bool Tracker::update(Rect& searchBox){
		return false;
	}
}



