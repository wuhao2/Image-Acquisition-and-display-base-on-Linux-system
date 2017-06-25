/*
 * 本文件用来解码jpg图片，并调用fb.c中的显示接口来显示到lcd上
 */
#include <stdio.h>
 
#include <config.h>			// for debug

#include <jpeglib.h>
#include <jerror.h>
#include <string.h>


// 函数功能: 判断一个图片文件是不是jpg图片
// 函数参数: path是图片文件的pathname
// 返回值:   如果是jpg则返回0，不是则返回1，错误返回-1
int is_jpg(const char *path)
{
	FILE *file = NULL;
	char buf[2] = {0};
	// 打开文件
	file = fopen(path, "rb");
	if (NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n", path);
		fclose(file);
		return -1;
	}
	// 读出前2个字节
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// 判断是不是0xffd8
	if (!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		// 不是jpg图片
	}
	// 是0xffd8开头，就继续
	// 文件指针移动到倒数2个字符的位置
	fseek(file, -2, SEEK_END);
	// 读出2个字节
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// 判断是不是0xffd9
	if (!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
		fclose(file);
		return 1;		// 不是jpg图片
	}

	fclose(file);
	
	return 0;
}



struct my_error_mgr 
{  
	struct jpeg_error_mgr pub;	/* "public" fields */  
//	jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;


// 自己定义的错误处理函数
METHODDEF(void)my_error_exit (j_common_ptr cinfo)
{  
	//my_error_ptr myerr = (my_error_ptr) cinfo->err;  
	//(*cinfo->err->output_message) (cinfo);    
	fprintf(stderr, "my_error_exit\n");
	//longjmp(myerr->setjmp_buffer, 1);
}

/*
 * 函数功能: 解码jpg图片，并将解码出来的数据存储
 * 函数参数: pPIC，记录源jpg图片，解码出来的图片宽高、图片数据缓冲区等信息
 * 返回值  : 成功解码则返回0，失败则返回-1
 */
 
static int jpg_analyze(struct pic_info *pPic)
{
	struct jpeg_decompress_struct cinfo;		// cinfo贯穿整个解码过程的信息记录和传递的数据结构
	struct my_error_mgr jerr;					// 错误处理的	
	//JSAMPARRAY buffer = NULL;					
	char * buffer = NULL;// 指向解码行数据的指针
	FILE * infile;								// 指向fopen打开源jpg图片文件的指针
	int row_stride;								// 解码出来的一行图片信息的字节数

	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{    
		fprintf(stderr, "can't open %s\n", pPic->pathname);    
		return -1;  
	}

	// 第1步: 错误处理函数部分的绑定
	cinfo.err = jpeg_std_error(&jerr.pub);	
	jerr.pub.error_exit = my_error_exit;
	// 给解码器做必要的内存分配和数据结构的初始化
	jpeg_create_decompress(&cinfo);

	// 第2步: 将fopen打开的源jpg图片和解码器相关联
	jpeg_stdio_src(&cinfo, infile);

	// 第3步: 读jpg文件头
	jpeg_read_header(&cinfo, TRUE);

	// 第4步: 启动解码器
	jpeg_start_decompress(&cinfo);
	

	debug("image resolution: %d * %d, bpp/8=%d.\n", 
		cinfo.output_width, cinfo.output_height, cinfo.output_components);
	// 解码出来的数据一行的字节数
	row_stride = cinfo.output_width * cinfo.output_components;
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if (NULL == buffer)
	{
		fprintf(stderr, "cinfo.mem->alloc_sarray error.\n");
		return -1;
	}

	// 第5步: 逐行解码，并将解码出的数据丢到事先准备好的缓冲区去
	while (cinfo.output_scanline < cinfo.output_height) 
	{	 
		// 解码一行信息，并且丢到buffer中
		//jpeg_read_scanlines(&cinfo, buffer, 1);
		jpeg_read_scanlines(&cinfo, &buffer, 1);	 

		// 将buffer中这一行数据移走到别的地方去暂存或者使用，总之是要腾出buffer空间
		// 来给循环的下一次解码一行来使用
		memcpy(pPic->pData + (cinfo.output_scanline-1) * row_stride, buffer, row_stride);
	}

	// 第6步: 解码完了，做各种清理工作
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);


	// jpg图片的数据记录下来
	pPic->width = cinfo.output_width;
	pPic->height = cinfo.output_height;
	pPic->bpp = cinfo.output_components * 8;


	return 0;
}


// 封装的一个对外使用的jpg显示函数
// 本函数对外只需要一个jpg图片的pathname即可，那些复杂的数据结构都是jpg显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1
int display_jpg(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;
	
	// 第一步: 检测给的图片是不是jpg图片
	ret = is_jpg(pathname);
	if (ret != 0)
	{
		return -1;
	}
	// 第二步: 解析该jpg图片
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	// 第三步: 显示该jpg图片
	fb_draw2(&picture);
}










