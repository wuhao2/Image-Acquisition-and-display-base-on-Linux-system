#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>  
#include <string>
#include <vector>

#include "Tracker.h"

namespace mycv {

	class SingleTemplateTracker : public mycv::Tracker
	{

	public:
		SingleTemplateTracker(int expandWidth,int expandHeight);
		virtual ~SingleTemplateTracker(void);  

		//子类继承的方法，如果要被调用加上virtual关键词，不加virtual则调用的时父类的init，track，update
		virtual bool init(const Mat& initFrame,  Rect& currentBoundingBox);//初始化跟踪器
		virtual bool track(const Mat& currentFrame, Rect& currentBoundingBox );//跟踪目标
		virtual bool update(Rect& searchBox);//更新目标模型

		//模板匹配
		float SingleTemplateTracker:: MatchTemplate(const Mat& src, const Mat& templ, Point2i& matcth_location,
					int match_method, Vec2i& xy_step, Vec2i& xy_stride);

		//估计局部搜索范围
		void SingleTemplateTracker:: EstimateSearchArea(const Rect& target_location, 
					Rect& search_area, int expand_x, int expand_y);

	public:
		//目标模板
		Mat targetTemplate;

		//当前帧上的目标框
		Rect CurrentBoundingBox;

		//当前帧上的图像块
		Mat CurrentTargetPatch;    //T(k+1) = aT(k) + bT(k-1)

		//下一帧的搜索范围
		Rect NextSearchArea;

		//搜索范围的扩展
		int expandWidth; int expandHeight;

		//视频帧的矩形区域
		Rect FrameArea;
		

	};


}

