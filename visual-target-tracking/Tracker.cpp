#include "Tracker.h"
#include <iostream>  

namespace mycv{

	Tracker::Tracker(void)
	{
	}
	Tracker::~Tracker(void) 
	{
	}

	//��ʼ��������
	bool Tracker::init(const Mat& initFrame,  Rect& initBoundingBox){
		return false;
	}
	//����Ŀ��
	bool Tracker::track(const Mat& currentFrame,  Rect& currentBoundingBox){
		return false;
	}
	//����ģ��
	bool Tracker::update(Rect& searchBox){
		return false;
	}
}



