#include "SingleTemplateTracker.h"
using namespace std;  
using namespace cv;  
namespace mycv {
	

	SingleTemplateTracker::SingleTemplateTracker(int expandWidth,int expandHeight)
	{
		this->expandHeight = expandHeight;
		this->expandWidth = expandWidth;
	}
	SingleTemplateTracker::~SingleTemplateTracker(void)
	{
	}

	//��ʼ��������init
	bool SingleTemplateTracker::init(const Mat& initFrame,  Rect& initBoundingBox){
		//��ʼ֡��Ŀ��ģ��
		this -> FrameArea = Rect(0, 0, initFrame.cols, initFrame.rows);
		//��ȡ��ʼ֡�ϵ�Ŀ��ģ��
		this ->targetTemplate = initFrame(initBoundingBox).clone(); //�ڳ�ʼ֡�Ͻ�Ŀ����¡��������ΪĿ��ģ��
		//������һ֡��������Χ
		this -> EstimateSearchArea(initBoundingBox, this->NextSearchArea, this->expandWidth ,this-> expandHeight);//�õ���һ֡����������
		return false;
	}

	//����Ŀ��track
	bool SingleTemplateTracker::track(const Mat& currentFrame,  Rect& currentBoundingBox){

		Point2i match_location(-1, -1); int match_method = 1;
		Vec2i xy_step(1 ,1); Vec2i xy_stride(1 , 1);
		//������һ֡����������ֻ��ROI����������
		this -> MatchTemplate(currentFrame(this->NextSearchArea), this->targetTemplate, match_location, match_method, xy_step, xy_stride);
		//���µ���mathch_locationƥ�������
		match_location.x = this->NextSearchArea.x;
		match_location.y = this->NextSearchArea.y;
		//���㵱ǰ֡�ϵ�Ŀ��λ��
		this -> CurrentBoundingBox = Rect(match_location.x, match_location.y, this->targetTemplate.cols, this->targetTemplate.rows);
		//ץȡ��ǰ֡�ϵ�Ŀ��ͼ���
		this-> CurrentTargetPatch = currentFrame(this->CurrentBoundingBox).clone(); //��¡��ǰ֡��ROI����


		currentBoundingBox = this->CurrentBoundingBox;
		return false;
	}


	//����Ŀ��ģ��
	bool SingleTemplateTracker::update(Rect& searchBox){
		//����Ŀ���������ģ��
		double alpha = 0.7;  //opencv�ṩ�ļ�Ȩ����   T��k+1�� = alpha * T��k�� + ��1-alpha��* T(k-1)
		                    //alpha = 1.0 ���ʾ��Զ���ֳ��ģ�����ȫ��סԭʼģ�壬 alpha=0.0��ʾ��ȫ��ʱ���������ȫ��ס�����ģ��
		cv::addWeighted(this->targetTemplate, alpha, this->CurrentTargetPatch, 1.0-alpha, 0.0, this->targetTemplate);

		//������һ֡�ϵľֲ�������Χ
		this-> EstimateSearchArea(this->CurrentBoundingBox, this->NextSearchArea, this->expandWidth ,this-> expandHeight);
		searchBox = this->NextSearchArea;
		return false;
	}

	/*ģ��ƥ�亯��
	-----��ģ��ȫ֡ƥ��������
	-----ȱ�㣺̫���ˣ�̫�˷���Դ�� ���������˲��������ǻ����� 
	-----�ŵ㣺Ŀ���ܳ���Ұ���ܹ����ж��θ���*/
	float MatchTemplate(const Mat& src, const Mat& templ, Point2i& matcth_location, 
		int match_method, Vec2i& xy_step, Vec2i& xy_stride){

		CV_Assert((src.type() == CV_8UC1) && (templ.type() == CV_8UC1));
		//ԭʼͼ���ģ��ĳߴ�
		int src_width = src.cols;
		int src_height = src.rows;
		int templ_cols = templ.cols;
		int templ_rows = templ.rows;
		int x_end = src_height - templ_rows + 1;
		int y_end = src_width - templ_cols + 1;

		float match_degree = FLT_MAX;
		int y_match = -1, x_match = -1;

		//���ϵ���ɨ��
		for (int y=0; y<y_end; y +=xy_stride[1]){
			//������ɨ��
			for (int x=0; x<x_end; x+=xy_stride[0]){
				//src(y,x)λ���ϵ�ģ��ƥ���
				float match_yx = 0.0f;
				//��ģ�����Ͻ�templ(0,0)���䵽src(y,x)λ�ã���ģ���ڻ��ۼ�ÿ���������ϵ����
				for(int r=0; r<templ_rows; r+=xy_step[1]){
					for (int c=0; c<templ_cols; r+=xy_step[0]){
						uchar src_val = src.ptr<uchar>(y+r)[x+c];
						uchar templ_val = templ.ptr<uchar>(r)[c];
						if(match_method == 0)
							match_yx += float (std::abs(src_val - templ_val)*std::abs(src_val - templ_val)); //ƽ������
						if(match_method == 1)
							match_yx += float (std::abs(src_val - templ_val)); //��������
					}
				
				}
				//����ʷ��õĲ���ȱȽϣ��ҳ������С�ĵ�
				if (match_degree > match_yx){
					match_degree = match_yx;
					x_match = x;
					y_match = y;

				}


			}
		
		}
			



	}


	void SingleTemplateTracker:: EstimateSearchArea(const Rect& target_location, 
		Rect& search_area, int expand_x, int expand_y){
		
		float center_x = target_location.x + 0.5f*target_location.width;
		float center_y = target_location.y + 0.5f*target_location.height;
		search_area.width = target_location.width + expand_x;
		search_area.height = target_location.height + expand_y;
		search_area.x = int(center_x - 0.5f*search_area.width);
		search_area.y = int(center_y - 0.5f*search_area.height);
		search_area &= this -> FrameArea;
	}


}

