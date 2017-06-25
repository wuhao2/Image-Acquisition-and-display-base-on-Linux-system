/*
 * 本文件用来解码png图片，并调用fb.c中的显示接口来显示到lcd上
 */
#include <stdio.h>
#include <string.h>
 
#include <config.h>			// for debug

#include <png.h>
#include <pngstruct.h>		// 这个一定要放在png.h的后面包含
#include <pnginfo.h>



#define PNG_BYTES_TO_CHECK 		8



// 函数功能: 判断一个图片文件是不是png图片
// 函数参数: path是图片文件的pathname
// 返回值:   如果是png则返回0，不是则非0
int is_png(const char *path)
{
	FILE *fp = NULL;
	char buf[PNG_BYTES_TO_CHECK] = {0};	

	/* Open the prospective PNG file. */   
	if ((fp = fopen(path, "rb")) == NULL)		
		return -1;	

	/* Read in some of the signature bytes */	
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)	   
		return -1; 

//	printf("%d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.		
	Return nonzero (true) if they match */	 
	return(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}


/*
 * 函数功能: 解码png图片，并将解码出来的数据存储
 * 函数参数: pPIC，记录源png图片，解码出来的图片宽高、图片数据缓冲区等信息
 * 返回值  : 成功解码则返回0，失败则返回-1
 */
 
static int png_analyze(struct pic_info *pPic)
{
	FILE *fp = NULL;
	png_structp png_ptr;   
	png_infop info_ptr;
	int color_type;
	png_bytep* row_pointers;
	unsigned long len = 0;
	int pos = 0;
	int i = 0, j = 0;

	if ((fp = fopen(pPic->pathname, "rb")) == NULL) 
	{	 
		fprintf(stderr, "can't open %s\n", pPic->pathname);    
		return -1;	
	}

	// 第1步: 相关数据结构实例化
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (png_ptr == 0)
	{
		fclose(fp);
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
  	if (info_ptr == 0)
  	{
   		png_destroy_read_struct(&png_ptr, 0, 0);
   		fclose(fp);
   		return -1;
  	}

	// 第2步: 设置错误处理函数
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
		return -1;
	}

	// 第3步: 将要解码的png图片的文件指针和png解码器绑定起来
	png_init_io(png_ptr, fp);

	// 第4步: 读取png图片信息
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);

	// 第5步: 相关图片信息打印出来看一看
	color_type = info_ptr->color_type;
	debug("color_type = %d\n", color_type);

	pPic->width = info_ptr->width;
	pPic->height = info_ptr->height;
	pPic->bpp = info_ptr->pixel_depth;
	len = info_ptr->width * info_ptr->height * info_ptr->pixel_depth / 8;
	debug("width = %u, height = %u, bpp = %u\n", pPic->width, pPic->height, pPic->bpp);

	// 第6步: 读取真正的图像信息
	row_pointers = png_get_rows(png_ptr,info_ptr);

	// 只处理RGB24位真彩色图片，其他格式的图片不管
	// 第7步: 图像数据移动到我们自己的buf中
	if(color_type == PNG_COLOR_TYPE_RGB)
  	{
   		//memcpy(pPic->pData, row_pointers, len);
		for(i=0; i<pPic->height; i++)
		{
			for(j=0; j<3*pPic->width; j+=3)
			{
				pPic->pData[pos++] = row_pointers[i][j+0];		
				pPic->pData[pos++] = row_pointers[i][j+1];		
				pPic->pData[pos++] = row_pointers[i][j+2];		
			}
		}
  	}

	// 第8步: 收尾处理
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	// close file
	fclose(fp);
	
	return 0;
}



// 封装的一个对外使用的png显示函数
// 本函数对外只需要一个png图片的pathname即可，那些复杂的数据结构都是jpg显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1
int display_png(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;

	// 第一步: 检测给的图片是不是jpg图片
	ret = is_png(pathname);
	if (ret != 0)
	{
		return -1;
	}
	debug("%s is a png\n", pathname);

	// 第二步: 解析jpg图片
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	png_analyze(&picture);

	// 第三步: 显示jpg图片
	fb_draw2(&picture);

	return 0;
}







