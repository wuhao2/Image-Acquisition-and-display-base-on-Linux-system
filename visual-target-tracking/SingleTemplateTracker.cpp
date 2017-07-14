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

	//初始化跟踪器init
	bool SingleTemplateTracker::init(const Mat& initFrame,  Rect& initBoundingBox){
		//初始帧的目标模板
		this -> FrameArea = Rect(0, 0, initFrame.cols, initFrame.rows);
		//提取初始帧上的目标模板
		this ->targetTemplate = initFrame(initBoundingBox).clone(); //在初始帧上将目标狂克隆下来，作为目标模板
		//估计下一帧的搜索范围
		this -> EstimateSearchArea(initBoundingBox, this->NextSearchArea, this->expandWidth ,this-> expandHeight);//得到下一帧的所搜区域
		return false;
	}

	//跟踪目标track
	bool SingleTemplateTracker::track(const Mat& currentFrame,  Rect& currentBoundingBox){

		Point2i match_location(-1, -1); int match_method = 1;
		Vec2i xy_step(1 ,1); Vec2i xy_stride(1 , 1);
		//传入下一帧的搜索区域，只在ROI区域内搜索
		this -> MatchTemplate(currentFrame(this->NextSearchArea), this->targetTemplate, match_location, match_method, xy_step, xy_stride);
		//重新调整mathch_location匹配点坐标
		match_location.x = this->NextSearchArea.x;
		match_location.y = this->NextSearchArea.y;
		//计算当前帧上的目标位置
		this -> CurrentBoundingBox = Rect(match_location.x, match_location.y, this->targetTemplate.cols, this->targetTemplate.rows);
		//抓取当前帧上的目标图像块
		this-> CurrentTargetPatch = currentFrame(this->CurrentBoundingBox).clone(); //克隆当前帧的ROI区域


		currentBoundingBox = this->CurrentBoundingBox;
		return false;
	}


	//更新目标模型
	bool SingleTemplateTracker::update(Rect& searchBox){
		//更新目标表面特征模型
		double alpha = 0.7;  //opencv提供的加权函数   T（k+1） = alpha * T（k） + （1-alpha）* T(k-1)
		                    //alpha = 1.0 则表示永远保持初心，即完全记住原始模板， alpha=0.0表示完全与时俱进，即完全记住后面的模板
		cv::addWeighted(this->targetTemplate, alpha, this->CurrentTargetPatch, 1.0-alpha, 0.0, this->targetTemplate);

		//更新下一帧上的局部搜索范围
		this-> EstimateSearchArea(this->CurrentBoundingBox, this->NextSearchArea, this->expandWidth ,this-> expandHeight);
		searchBox = this->NextSearchArea;
		return false;
	}

	/*模板匹配函数
	-----单模板全帧匹配搜索：
	-----缺点：太慢了，太浪费资源， 尽管设置了步长，但是还是慢 
	-----优点：目标跑出视野后，能够进行二次跟踪*/
	float MatchTemplate(const Mat& src, const Mat& templ, Point2i& matcth_location, 
		int match_method, Vec2i& xy_step, Vec2i& xy_stride){

		CV_Assert((src.type() == CV_8UC1) && (templ.type() == CV_8UC1));
		//原始图像和模板的尺寸
		int src_width = src.cols;
		int src_height = src.rows;
		int templ_cols = templ.cols;
		int templ_rows = templ.rows;
		int x_end = src_height - templ_rows + 1;
		int y_end = src_width - templ_cols + 1;

		float match_degree = FLT_MAX;
		int y_match = -1, x_match = -1;

		//从上到下扫面
		for (int y=0; y<y_end; y +=xy_stride[1]){
			//从左到右扫描
			for (int x=0; x<x_end; x+=xy_stride[0]){
				//src(y,x)位置上的模板匹配度
				float match_yx = 0.0f;
				//将模板左上角templ(0,0)对其到src(y,x)位置，在模板内积累家每个采样点上的误差
				for(int r=0; r<templ_rows; r+=xy_step[1]){
					for (int c=0; c<templ_cols; r+=xy_step[0]){
						uchar src_val = src.ptr<uchar>(y+r)[x+c];
						uchar templ_val = templ.ptr<uchar>(r)[c];
						if(match_method == 0)
							match_yx += float (std::abs(src_val - templ_val)*std::abs(src_val - templ_val)); //平方误差和
						if(match_method == 1)
							match_yx += float (std::abs(src_val - templ_val)); //绝对误差和
					}
				
				}
				//与历史最好的差异度比较，找出误差最小的点
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

