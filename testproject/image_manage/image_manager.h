

#define PATHNAME_LEN		256

typedef enum image_type
{
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TPPE_UNKNOWN,
}image_type_e;


// 结构体用来封装一个图片的信息
typedef struct image_info
{
	char pathname[PATHNAME_LEN];			// 图片文件的pathname
	image_type_e type;						// 图片文件的格式
}image_info_t;











