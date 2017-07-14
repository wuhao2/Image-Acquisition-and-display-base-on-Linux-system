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

		//����̳еķ��������Ҫ�����ü���virtual�ؼ��ʣ�����virtual����õ�ʱ�����init��track��update
		virtual bool init(const Mat& initFrame,  Rect& currentBoundingBox);//��ʼ��������
		virtual bool track(const Mat& currentFrame, Rect& currentBoundingBox );//����Ŀ��
		virtual bool update(Rect& searchBox);//����Ŀ��ģ��

		//ģ��ƥ��
		float SingleTemplateTracker:: MatchTemplate(const Mat& src, const Mat& templ, Point2i& matcth_location,
					int match_method, Vec2i& xy_step, Vec2i& xy_stride);

		//���ƾֲ�������Χ
		void SingleTemplateTracker:: EstimateSearchArea(const Rect& target_location, 
					Rect& search_area, int expand_x, int expand_y);

	public:
		//Ŀ��ģ��
		Mat targetTemplate;

		//��ǰ֡�ϵ�Ŀ���
		Rect CurrentBoundingBox;

		//��ǰ֡�ϵ�ͼ���
		Mat CurrentTargetPatch;    //T(k+1) = aT(k) + bT(k-1)

		//��һ֡��������Χ
		Rect NextSearchArea;

		//������Χ����չ
		int expandWidth; int expandHeight;

		//��Ƶ֡�ľ�������
		Rect FrameArea;
		

	};


}

