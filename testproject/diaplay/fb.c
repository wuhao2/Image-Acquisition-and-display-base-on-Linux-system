/*
 * fb.c 操作framebuffer的基础代码，包括fb的打开、ioctl获取信息
 *      基本的测试fb显示代码
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

 
 // 全局变量
 unsigned int *pfb = NULL;			// pfb指向framebuffer内存地址首地址
 int fbfd = -1;						// 打开fb后得到的fd
 
 
 
int fb_open(void)
{
	 int ret = -1;
	 
	 struct fb_fix_screeninfo finfo;
	 struct fb_var_screeninfo vinfo;
	 
	 // 第1步：打开设备
	 fbfd = open(FBDEVICE, O_RDWR);
	 if (fbfd < 0)
	 {
		 perror("open error");
		 return -1;
	 }
	 debug("open %s success.\n", FBDEVICE);
	 
	 // 第2步：获取设备的硬件信息
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
 
	 
	 // 第3步：进行mmap
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
 
 
// 绘制屏幕背景色
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

// 画线测试函数
void fb_draw_line(unsigned int color)
{
	 unsigned int x;
	 
	 for (x=50; x<600; x++)
	 {
		 *(pfb + 200 * WIDTH + x) = color;
	 }
}
 
#if 0

// 测试显示1024*600分辨率的图片
void fb_draw_picutre(void)
{
	const unsigned char *pData = gImage_1024600;		// 指针指向图像数组
	unsigned int *p = pfb;
	unsigned int cnt;
	
	unsigned int i, j;

	for (i=0; i<HEIGHT; i++)
	{
		for (j=0; j<WIDTH; j++)
		{ 
			//cnt = WIDTH * i + j;		// 当前像素点的编号
			//cnt *= 3;					// 当前像素点的数据在数组中的下标
			cnt = 3 * (WIDTH * i + j);
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*p = ((pData[cnt+2]<<0) | (pData[cnt+1]<<8)| (pData[cnt+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			p++;
		}
	}
}


void fb_draw_picutre2(void)
{
	const unsigned char *pData = gImage_1024600;		// 指针指向图像数组
	unsigned int cnt;
	
	unsigned int x, y;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[3*cnt+2]<<0) | (pData[3*cnt+1]<<8)| (pData[3*cnt+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			
		}
	}
}


void fb_draw_picutre3(void)
{
	const unsigned char *pData = gImage_1024600;		// 指针指向图像数组
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// 测试显示比屏幕小的图片的显示函数
void fb_draw_picutre4(void)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// 图片大小分辨率是500*281
	for (y=0; y<281; y++)
	{
		for (x=0; x<500; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// 测试显示比屏幕小的图片的显示函数2
void fb_draw_picutre5(void)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=0; y<281; y++)
	{
		for (x=0; x<500; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			
		}
	}
}


// 函数完成功能: 将图片显示到起点坐标(x0, y0)处
void fb_draw_picutre6(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * (y - y0) + (x - x0);


			// 算式的左值和右值分别要去考虑，
			// 左值考虑当前像素点在fb内存中的偏移量
			// 右值考虑当前像素点在图像数据数组中的下标
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
		}
	}
}

// 功能同6，不一样的算法
void fb_draw_picutre7(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// 图片大小分辨率是500*281
	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// x和y两个方向超出屏幕外的部分都不显示
void fb_draw_picutre8(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt;
	unsigned int a = 0;
	
	unsigned int x, y;

	// 图片大小分辨率是500*281
	for (y=y0; y<y0+281; y++)
	{
		if (y >= HEIGHT)
		{
			// y方向超出了
			break;
		}
		
		for (x=x0; x<x0+500; x++)
		{ 
			if (x >= WIDTH)
			{
				// x方向超出屏幕外了
				a += 3;
				continue;
			}
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}

// 功能同8，算法不一样
void fb_draw_picutre9(unsigned int x0, unsigned int y0)
{
	const unsigned char *pData = gImage_picture;		// 指针指向图像数组
	unsigned int cnt1, cnt2;
	
	unsigned int x, y;

	for (y=y0; y<y0+281; y++)
	{
		for (x=x0; x<x0+500; x++)
		{ 
			if (x >= WIDTH)
			{
				// x方向超出屏幕外了
				continue;
			}
			
			//cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 500 * (y - y0) + (x - x0);


			// 算式的左值和右值分别要去考虑，
			// 左值考虑当前像素点在fb内存中的偏移量
			// 右值考虑当前像素点在图像数据数组中的下标
			*(pfb + cnt1) = ((pData[3*cnt2+2]<<0) | (pData[3*cnt2+1]<<8)| (pData[3*cnt2+0]<<16)); 
		}
	}
}

#endif




void fb_draw(const struct pic_info *pPic)
{
	const char *pData = (const char *)pPic->pData;		// 指针指向图像数组
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
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+0]<<0) | (pData[a+1]<<8)| (pData[a+2]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a -= 3;
		}
	}
}


void fb_draw2(const struct pic_info *pPic)
{
	const char *pData = (const char *)pPic->pData;		// 指针指向图像数组
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
			//cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0]  pData[cnt+1]  pData[cnt+2]  
			// 当前像素点的数据
			*(pfb + cnt) = ((pData[a+2]<<0) | (pData[a+1]<<8)| (pData[a+0]<<16)); 
			//*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			a += 3;
		}
	}
}





