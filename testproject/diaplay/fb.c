/*
 * fb.c ����framebuffer�Ļ������룬����fb�Ĵ򿪡�ioctl��ȡ��Ϣ
 *      �����Ĳ���fb��ʾ����
 * *******************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>


#include <fb.h>

 
 // ȫ�ֱ���
 unsigned int *pfb = NULL;			// pfbָ��framebuffer�ڴ��ַ�׵�ַ
 int fbfd = -1;						// ��fb��õ���fd
 
 
 
int fb_open(void)
{
	 int ret = -1;
	 
	 struct fb_fix_screeninfo finfo;
	 struct fb_var_screeninfo vinfo;
	 
	 // ��1�������豸
	 fbfd = open(FBDEVICE, O_RDWR);
	 if (fbfd < 0)
	 {
		 perror("open error");
		 return -1;
	 }
	 debug("open %s success.\n", FBDEVICE);
	 
	 // ��2������ȡ�豸��Ӳ����Ϣ
	 ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	 if (ret < 0)
	 {
		 perror("ioctl");
		 return -1;
	 }
	 debug("smem_start = 0x%lx, smem_len = %u.\n", finfo.smem_start, finfo.smem_len);
	 
	 ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
	 if (ret < 0)
	 {
		 perror("ioctl");
		 return -1;
	 }
	 debug("xres = %u, yres = %u.\n", vinfo.xres, vinfo.yres);
	 debug("xres_virtual = %u, yres_virtual = %u.\n", vinfo.xres_virtual, vinfo.yres_virtual);
	 debug("bpp = %u.\n", vinfo.bits_per_pixel);
 
	 
	 // ��3��������mmap
	 unsigned long len = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
	 debug("len = %ld\n", len);
	 pfb = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	 if (NULL == pfb)
	 {
		 perror("mmap error");
		 return -1;
	 }
	 debug("pfb = %p.\n", pfb);

	 return 0;
	 
}


void fb_close(void)
{	
	close(fbfd);
}
 
 
// ������Ļ����ɫ
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color)
{
	 unsigned int x, y;
	 
	 for (y=0; y<height; y++)
	 {
		 for (x=0; x<width; x++)
		 {
			 *(pfb + y * WIDTH + x) = color;
		 }
	 }
}

// ���߲��Ժ���
void fb_draw_line(unsigned int color)
{
	 unsigned int x;
	 
	 for (x=50; x<600; x++)
	 {
		 *(pfb + 200 * WIDTH + x) = color;
	 }
}
 
#if 0

// ������ʾ1024*600�ֱ��ʵ�ͼƬ
void fb_draw_picutre(void)
{
	const unsigned char *pData = gImage_1024600;		// ָ��ָ��ͼ������
	unsigned int *p = pfb;
	unsigned int cnt;
	
	unsigned int i, j;

	for (i=0; i<HEIGHT; i++)
	{
		for (j=0; j<WIDTH; j++)
		{ 
			//cnt = WIDTH * i + j;		// ��ǰ���ص�ı��
			//cnt *= 3;					// ��ǰ���ص�������������е��±�
			cnt = 3 * (WIDTH * i + j);
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*p = ((pData[cnt+2]<<0) | (pData[cnt+1]<<8)| (pData[cnt+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			p++;
		}
	}
}


void fb_draw_picutre2(void)
{
	const unsigned char *pData = gImage_1024600;		// ָ��ָ��ͼ������
	unsigned int cnt;
	
	unsigned int x, y;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[3*cnt+2]<<0) | (pData[3*cnt+1]<<8)| (pData[3*cnt+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			
		}
	}
}


void fb_draw_picutre3(void)
{
	const unsigned char *pData = gImage_1024600;		// ָ��ָ��ͼ������
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// ������ʾ����ĻС��ͼƬ����ʾ����
void fb_draw_picutre4(void)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// ͼƬ��С�ֱ�����500*281
	for (y=0; y<281; y++)
	{
		for (x=0; x<500; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// ������ʾ����ĻС��ͼƬ����ʾ����2
void fb_draw_picutre5(void)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=0; y<281; y++)
	{
		for (x=0; x<500; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			
		}
	}
}


// ������ɹ���: ��ͼƬ��ʾ���������(x0, y0)��
void fb_draw_picutre6(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * (y - y0) + (x - x0);


			// ��ʽ����ֵ����ֵ�ֱ�Ҫȥ���ǣ�
			// ��ֵ���ǵ�ǰ���ص���fb�ڴ��е�ƫ����
			// ��ֵ���ǵ�ǰ���ص���ͼ�����������е��±�
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
		}
	}
}

// ����ͬ6����һ�����㷨
void fb_draw_picutre7(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// ͼƬ��С�ֱ�����500*281
	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// x��y�������򳬳���Ļ��Ĳ��ֶ�����ʾ
void fb_draw_picutre8(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// ͼƬ��С�ֱ�����500*281
	for (y=y0; y<y0+281; y++)
	{
		if (y >= HEIGHT)
		{
			// y���򳬳���
			break;
		}
		
		for (x=x0; x<x0+500; x++)
		{ 
			if (x >= WIDTH)
			{
				// x���򳬳���Ļ����
				a += 3;
				continue;
			}
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// ����ͬ8���㷨��һ��
void fb_draw_picutre9(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// ָ��ָ��ͼ������
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			if (x >= WIDTH)
			{
				// x���򳬳���Ļ����
				continue;
			}
			
			//cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * (y - y0) + (x - x0);


			// ��ʽ����ֵ����ֵ�ֱ�Ҫȥ���ǣ�
			// ��ֵ���ǵ�ǰ���ص���fb�ڴ��е�ƫ����
			// ��ֵ���ǵ�ǰ���ص���ͼ�����������е��±�
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
		}
	}
}

#endif




void fb_draw(const struct pic_info *pPic)
{
	const char *pData = (const char *)pPic->pData;		// ָ��ָ��ͼ������
	unsigned int cnt = 0, a = 0;
	unsigned int x, y;

	debug("image resolution: %d * %d, bpp=%d.\n", 
		pPic->width, pPic->height, pPic->bpp);

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	a = pPic->height * pPic->width * 3 - 3;
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+0]<<0) | (pData[a+1]<<8)| (pData[a+2]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a -= 3;
		}
	}
}


void fb_draw2(const struct pic_info *pPic)
{
	const char *pData = (const char *)pPic->pData;		// ָ��ָ��ͼ������
	unsigned int cnt = 0, a = 0;
	unsigned int x, y;

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	a = 0;
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{ 
			//cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// ��ǰ���ص������
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}





