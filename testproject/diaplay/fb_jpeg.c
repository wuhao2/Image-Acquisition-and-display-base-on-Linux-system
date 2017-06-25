/*
 * ���ļ���������jpgͼƬ��������fb.c�е���ʾ�ӿ�����ʾ��lcd��
 */
#include <stdio.h>
 
#include <config.h>			// for debug

#include <jpeglib.h>
#include <jerror.h>
#include <string.h>


// ��������: �ж�һ��ͼƬ�ļ��ǲ���jpgͼƬ
// ��������: path��ͼƬ�ļ���pathname
// ����ֵ:   �����jpg�򷵻�0�������򷵻�1�����󷵻�-1
int is_jpg(const char *path)
{
	FILE *file = NULL;
	char buf[2] = {0};
	// ���ļ�
	file = fopen(path, "rb");
	if (NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n", path);
		fclose(file);
		return -1;
	}
	// ����ǰ2���ֽ�
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// �ж��ǲ���0xffd8
	if (!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		// ����jpgͼƬ
	}
	// ��0xffd8��ͷ���ͼ���
	// �ļ�ָ���ƶ�������2���ַ���λ��
	fseek(file, -2, SEEK_END);
	// ����2���ֽ�
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// �ж��ǲ���0xffd9
	if (!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
		fclose(file);
		return 1;		// ����jpgͼƬ
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


// �Լ�����Ĵ�������
METHODDEF(void)my_error_exit (j_common_ptr cinfo)
{  
	//my_error_ptr myerr = (my_error_ptr) cinfo->err;  
	//(*cinfo->err->output_message) (cinfo);    
	fprintf(stderr, "my_error_exit\n");
	//longjmp(myerr->setjmp_buffer, 1);
}

/*
 * ��������: ����jpgͼƬ������������������ݴ洢
 * ��������: pPIC����¼ԴjpgͼƬ�����������ͼƬ��ߡ�ͼƬ���ݻ���������Ϣ
 * ����ֵ  : �ɹ������򷵻�0��ʧ���򷵻�-1
 */
 
static int jpg_analyze(struct pic_info *pPic)
{
	struct jpeg_decompress_struct cinfo;		// cinfo�ᴩ����������̵���Ϣ��¼�ʹ��ݵ����ݽṹ
	struct my_error_mgr jerr;					// �������	
	//JSAMPARRAY buffer = NULL;					
	char * buffer = NULL;// ָ����������ݵ�ָ��
	FILE * infile;								// ָ��fopen��ԴjpgͼƬ�ļ���ָ��
	int row_stride;								// ���������һ��ͼƬ��Ϣ���ֽ���

	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{    
		fprintf(stderr, "can't open %s\n", pPic->pathname);    
		return -1;  
	}

	// ��1��: �����������ֵİ�
	cinfo.err = jpeg_std_error(&jerr.pub);	
	jerr.pub.error_exit = my_error_exit;
	// ������������Ҫ���ڴ��������ݽṹ�ĳ�ʼ��
	jpeg_create_decompress(&cinfo);

	// ��2��: ��fopen�򿪵�ԴjpgͼƬ�ͽ����������
	jpeg_stdio_src(&cinfo, infile);

	// ��3��: ��jpg�ļ�ͷ
	jpeg_read_header(&cinfo, TRUE);

	// ��4��: ����������
	jpeg_start_decompress(&cinfo);
	

	debug("image resolution: %d * %d, bpp/8=%d.\n", 
		cinfo.output_width, cinfo.output_height, cinfo.output_components);
	// �������������һ�е��ֽ���
	row_stride = cinfo.output_width * cinfo.output_components;
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if (NULL == buffer)
	{
		fprintf(stderr, "cinfo.mem->alloc_sarray error.\n");
		return -1;
	}

	// ��5��: ���н��룬��������������ݶ�������׼���õĻ�����ȥ
	while (cinfo.output_scanline < cinfo.output_height) 
	{	 
		// ����һ����Ϣ�����Ҷ���buffer��
		//jpeg_read_scanlines(&cinfo, buffer, 1);
		jpeg_read_scanlines(&cinfo, &buffer, 1);	 

		// ��buffer����һ���������ߵ���ĵط�ȥ�ݴ����ʹ�ã���֮��Ҫ�ڳ�buffer�ռ�
		// ����ѭ������һ�ν���һ����ʹ��
		memcpy(pPic->pData + (cinfo.output_scanline-1) * row_stride, buffer, row_stride);
	}

	// ��6��: �������ˣ�������������
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);


	// jpgͼƬ�����ݼ�¼����
	pPic->width = cinfo.output_width;
	pPic->height = cinfo.output_height;
	pPic->bpp = cinfo.output_components * 8;


	return 0;
}


// ��װ��һ������ʹ�õ�jpg��ʾ����
// ����������ֻ��Ҫһ��jpgͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����jpg��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1
int display_jpg(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;
	
	// ��һ��: ������ͼƬ�ǲ���jpgͼƬ
	ret = is_jpg(pathname);
	if (ret != 0)
	{
		return -1;
	}
	// �ڶ���: ������jpgͼƬ
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	// ������: ��ʾ��jpgͼƬ
	fb_draw2(&picture);
}










