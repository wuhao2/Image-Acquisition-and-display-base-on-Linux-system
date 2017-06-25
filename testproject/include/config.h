#ifndef __CONFIG_H__
#define __CONFIG_H__


/******** �궨�� ****************************************************************/

//#define DEBUG			// �򿪵�����Ϣ����Ŀ���

// debug��Ķ���
#ifdef DEBUG
#define debug(...)                                                      \
        {                                                               \
            fprintf(stderr, "[debug][%s:%s:%d] ",                     \
                    __FILE__, __FUNCTION__, __LINE__);                  \
            fprintf(stderr, __VA_ARGS__);                               \
        }
#else
#define debug(...)  
#endif

// ���ǹ涨���֧��1920*1080��ô���ͼƬ��BPP���24
#define BMP_MAX_RESOLUTION		(1920*1080)
#define BMP_BUF_SIZE			(BMP_MAX_RESOLUTION*3)

// ���ǹ涨���֧��100��ͼƬ
#define MAX_IMAGE_CNT			100

// ����ϵͳ�еĴ������豸���豸��
#define DEVICE_TOUCHSCREEN		"/dev/input/event2"
#define TOUCH_WIDTH				200				// ���崥����ҳ����Ŀ��


/***********  �ṹ�嶨��  ****************************************************/

// �ṹ��������װһ��ͼƬ�ĸ�����Ϣ
typedef struct pic_info
{
	char *pathname;			// ͼƬ���ļ�ϵͳ�е�·����+�ļ���
	unsigned int width;		// ͼƬ�ֱ���֮��
	unsigned int height;	// ͼƬ�ֱ���֮��
	unsigned int bpp;		// ͼƬbpp
	char *pData;			// ָ��ͼƬ��Ч���ݴ洢��buf����
}pic_info;



/***********  ȫ�ֱ��� ***************************************************/
extern char rgb_buf[BMP_BUF_SIZE];





/********* ����ԭ�� *******************************************************/
int is_bmp(const char *path);
int is_jpg(const char *path);
int is_png(const char *path);


int display_bmp(const char *pathname);
int display_jpg(const char *pathname);
int display_png(const char *pathname);


int scan_image(const char *path);
int scan_image2(const char *path);
void show_images(void);
int ts_updown(void);






#endif











