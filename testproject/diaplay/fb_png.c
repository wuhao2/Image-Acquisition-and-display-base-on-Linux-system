/*
 * ���ļ���������pngͼƬ��������fb.c�е���ʾ�ӿ�����ʾ��lcd��
 */
#include <stdio.h>
#include <string.h>
 
#include <config.h>			// for debug

#include <png.h>
#include <pngstruct.h>		// ���һ��Ҫ����png.h�ĺ������
#include <pnginfo.h>



#define PNG_BYTES_TO_CHECK 		8



// ��������: �ж�һ��ͼƬ�ļ��ǲ���pngͼƬ
// ��������: path��ͼƬ�ļ���pathname
// ����ֵ:   �����png�򷵻�0���������0
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
 * ��������: ����pngͼƬ������������������ݴ洢
 * ��������: pPIC����¼ԴpngͼƬ�����������ͼƬ��ߡ�ͼƬ���ݻ���������Ϣ
 * ����ֵ  : �ɹ������򷵻�0��ʧ���򷵻�-1
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

	// ��1��: ������ݽṹʵ����
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

	// ��2��: ���ô�������
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
		return -1;
	}

	// ��3��: ��Ҫ�����pngͼƬ���ļ�ָ���png������������
	png_init_io(png_ptr, fp);

	// ��4��: ��ȡpngͼƬ��Ϣ
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);

	// ��5��: ���ͼƬ��Ϣ��ӡ������һ��
	color_type = info_ptr->color_type;
	debug("color_type = %d\n", color_type);

	pPic->width = info_ptr->width;
	pPic->height = info_ptr->height;
	pPic->bpp = info_ptr->pixel_depth;
	len = info_ptr->width * info_ptr->height * info_ptr->pixel_depth / 8;
	debug("width = %u, height = %u, bpp = %u\n", pPic->width, pPic->height, pPic->bpp);

	// ��6��: ��ȡ������ͼ����Ϣ
	row_pointers = png_get_rows(png_ptr,info_ptr);

	// ֻ����RGB24λ���ɫͼƬ��������ʽ��ͼƬ����
	// ��7��: ͼ�������ƶ��������Լ���buf��
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

	// ��8��: ��β����
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	// close file
	fclose(fp);
	
	return 0;
}



// ��װ��һ������ʹ�õ�png��ʾ����
// ����������ֻ��Ҫһ��pngͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����jpg��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1
int display_png(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;

	// ��һ��: ������ͼƬ�ǲ���jpgͼƬ
	ret = is_png(pathname);
	if (ret != 0)
	{
		return -1;
	}
	debug("%s is a png\n", pathname);

	// �ڶ���: ����jpgͼƬ
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	png_analyze(&picture);

	// ������: ��ʾjpgͼƬ
	fb_draw2(&picture);

	return 0;
}







