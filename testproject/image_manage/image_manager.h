

#define PATHNAME_LEN		256

typedef enum image_type
{
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TPPE_UNKNOWN,
}image_type_e;


// �ṹ��������װһ��ͼƬ����Ϣ
typedef struct image_info
{
	char pathname[PATHNAME_LEN];			// ͼƬ�ļ���pathname
	image_type_e type;						// ͼƬ�ļ��ĸ�ʽ
}image_info_t;











