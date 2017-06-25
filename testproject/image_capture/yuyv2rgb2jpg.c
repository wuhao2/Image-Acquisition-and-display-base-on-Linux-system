#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <stdlib.h>  
#include <sys/mman.h>  
#include <linux/types.h>  
#include <linux/videodev2.h>  
#include <setjmp.h>  
#include "jpeglib.h"  
   
//#include "v4l2grab.h"  
  
typedef int BOOL;  
  
#define  TRUE    1  
#define  FALSE    0  
  
#define FILE_VIDEO     "/dev/video0"  
#define BMP          "image_bmp.bmp"  
#define YUV        "image_yuv.yuv"  
#define JPEG        "image_jpeg.jpg"  
  
#define  IMAGEWIDTH    320
#define  IMAGEHEIGHT   240
  
static   int      fd;  
static   struct   v4l2_capability   cap;  
struct v4l2_fmtdesc  fmtdesc;  
struct v4l2_format  fmt,fmtack;  
struct v4l2_streamparm  setfps;    
struct v4l2_requestbuffers  req;  
struct v4l2_buffer  buf;  
enum v4l2_buf_type  type;  
unsigned char frame_buffer[IMAGEWIDTH*IMAGEHEIGHT*3];  
  
  
struct buffer  
{  
    void * start;  
    unsigned int length;  
} * buffers;  
   
// BMP 文件头
typedef struct  
{  
    unsigned short      bfType;  			// 2
    unsigned long    	bfSize; 			// 4 
    unsigned short    	bfReserved1;  
    unsigned short    	bfReserved2;  
    unsigned long    	bfOffBits;  
}BMPFILEHEADER_T ;

// BMP 信息头
typedef struct  
{  
    unsigned long  		biSize;   
    long   				biWidth;   
    long   				biHeight;   
    unsigned short   	biPlanes;   
    unsigned short   	biBitCount;  
    unsigned long 	 	biCompression;   
    unsigned long  		biSizeImage;   
    long   				biXPelsPerMeter;   
    long   				biYPelsPerMeter;   
    unsigned long  	 	biClrUsed;   
    unsigned long   	biClrImportant;   
}BMPINFOHEADER_T;  
   
   
   
   
   
int init_v4l2(void)  
{  
    int i;  
    int ret = 0;  
      
    //opendev  
    if ((fd = open(FILE_VIDEO, O_RDWR)) == -1)   
    {  
        printf("Error opening V4L interface\n");  
        return (FALSE);  
    }  
  
    //query cap  
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)   
    {  
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);  
        return (FALSE);  
    }  
    else  
    {  
         printf("driver:\t\t%s\n",cap.driver);  
         printf("card:\t\t%s\n",cap.card);  
         printf("bus_info:\t%s\n",cap.bus_info);  
         printf("version:\t%d\n",cap.version);  
         printf("capabilities:\t%x\n",cap.capabilities);  
           
         if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)   
         {  
            printf("Device %s: supports capture.\n",FILE_VIDEO);  
        }  
  
        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)   
        {  
            printf("Device %s: supports streaming.\n",FILE_VIDEO);  
        }  
    }   
      
    //emu all support fmt  
    fmtdesc.index=0;  
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    printf("Support format:\n");  
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)  
    {  
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);  
        fmtdesc.index++;  
    }  
      
    //set fmt  
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  
    fmt.fmt.pix.height = IMAGEHEIGHT;  
    fmt.fmt.pix.width = IMAGEWIDTH;  
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;  
      
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)  
    {  
        printf("Unable to set format\n");  
        return FALSE;  
    }       
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)  
    {  
        printf("Unable to get format\n");  
        return FALSE;  
    }   
    {  
         printf("fmt.type:\t\t%d\n",fmt.type);  
         printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);  
         printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);  
         printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);  
         printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);  
    }  
    //set fps  
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    setfps.parm.capture.timeperframe.numerator = 10;  
    setfps.parm.capture.timeperframe.denominator = 10;  
      
    printf("init %s \t[OK]\n",FILE_VIDEO);  
          
    return TRUE;  
}  
  
int v4l2_grab(void)  
{  
    unsigned int n_buffers;  
      
    //request for 4 buffers   
    req.count=4;  
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    req.memory=V4L2_MEMORY_MMAP;  
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)  
    {  
        printf("request for buffers error\n");  
    }  
  
    //mmap for buffers  
    buffers = malloc(req.count*sizeof (*buffers));  
    if (!buffers)   
    {  
        printf ("Out of memory\n");  
        return(FALSE);  
    }  
      
    for (n_buffers = 0; n_buffers < req.count; n_buffers++)   
    {  
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory = V4L2_MEMORY_MMAP;  
        buf.index = n_buffers;  
        //query buffers  
        if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1)  
        {  
            printf("query buffer error\n");  
            return(FALSE);  
        }  
  
        buffers[n_buffers].length = buf.length;  
        //map  
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ |PROT_WRITE, MAP_SHARED, fd, buf.m.offset);  
        if (buffers[n_buffers].start == MAP_FAILED)  
        {  
            printf("buffer map error\n");  
            return(FALSE);  
        }  
    }  
          
    //queue  
    for (n_buffers = 0; n_buffers < req.count; n_buffers++)  
    {  
        buf.index = n_buffers;  
        ioctl(fd, VIDIOC_QBUF, &buf);  
    }   
      
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    ioctl (fd, VIDIOC_STREAMON, &type);  
      
    ioctl(fd, VIDIOC_DQBUF, &buf);  
  
    printf("grab yuyv OK\n");  
    return(TRUE);  
}  
  
  
int yuyv_2_rgb888(void)  
{  
    int           i,j;  
    unsigned char y1,y2,u,v;  
    int r1,g1,b1,r2,g2,b2;  
    char * pointer;  
  
    pointer = buffers[0].start;  
  
    for(i=0;i<IMAGEHEIGHT;i++)  
    {  
    for(j=0;j<IMAGEWIDTH/2;j++)//每次取4个字节，也就是两个像素点，转换rgb，6个字节，还是两个像素点  
    {  
    y1 = *( pointer + (i*IMAGEWIDTH/2+j)*4);       
    u  = *( pointer + (i*IMAGEWIDTH/2+j)*4 + 1);  
    y2 = *( pointer + (i*IMAGEWIDTH/2+j)*4 + 2);  
    v  = *( pointer + (i*IMAGEWIDTH/2+j)*4 + 3);  
  
    r1 = y1 + 1.042*(v-128);  
    g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);  
    b1 = y1 + 1.772*(u-128);  
  
    r2 = y2 + 1.042*(v-128);  
    g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);  
    b2 = y2 + 1.772*(u-128);  
  
    if(r1>255)  
    r1 = 255;  
    else if(r1<0)  
    r1 = 0;  
  
    if(b1>255)  
    b1 = 255;  
    else if(b1<0)  
    b1 = 0;      
  
    if(g1>255)  
    g1 = 255;  
    else if(g1<0)  
    g1 = 0;      
  
    if(r2>255)  
    r2 = 255;  
    else if(r2<0)  
    r2 = 0;  
  
    if(b2>255)  
    b2 = 255;  
    else if(b2<0)  
    b2 = 0;      
  
    if(g2>255)  
    g2 = 255;  
    else if(g2<0)  
    g2 = 0;          
  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6    ) = (unsigned char)b1;  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6 + 1) = (unsigned char)g1;  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6 + 2) = (unsigned char)r1;  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6 + 3) = (unsigned char)b2;  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6 + 4) = (unsigned char)g2;  
    *(frame_buffer + (i*IMAGEWIDTH/2+j)*6 + 5) = (unsigned char)r2;  
    }  
    }  
    printf("change to RGB OK \n");  
}  

void rgb_to_bmp(unsigned char* pdata,int width,int height )     
{  
    //分别为rgb数据，要保存的bmp文件名   
    int size = IMAGEWIDTH * IMAGEHEIGHT * 33 * sizeof(char); // 每个像素点3个字节    
    // 位图第一部分，文件信息    
    BMPFILEHEADER_T bfh;    
    bfh.bfType = (unsigned short)0x4d42;  //bm    
    bfh.bfSize = size  // data size    
        + sizeof( BMPFILEHEADER_T ) // first section size    
        + sizeof( BMPINFOHEADER_T ) // second section size    
        ;    
    bfh.bfReserved1 = 0; // reserved    
    bfh.bfReserved2 = 0; // reserved    
    bfh.bfOffBits = sizeof( BMPFILEHEADER_T )+ sizeof( BMPINFOHEADER_T );//真正的数据的位置   
    printf("bmp_head== %ld\n", bfh.bfOffBits);   
	
    // 位图第二部分，数据信息    
    BMPINFOHEADER_T bih;    
    bih.biSize = sizeof(BMPINFOHEADER_T);    
    bih.biWidth = IMAGEWIDTH;    
    bih.biHeight = -IMAGEHEIGHT;//BMP图片从最后一个点开始扫描，显示时图片是倒着的，所以用-height，这样图片就正了    
    bih.biPlanes = 1;//为1，不用改    
    bih.biBitCount = 24;    
    bih.biCompression = 0;//不压缩    
    bih.biSizeImage = size;    
  
    bih.biXPelsPerMeter = 0;//像素每米    
    
    bih.biYPelsPerMeter = 0;    
    bih.biClrUsed = 0;//已用过的颜色，为0,与bitcount相同    
    bih.biClrImportant = 0;//每个像素都重要     

	FILE *bmp_fd = fopen (BMP,"wb");//注意这里为什么用fopen而不用open  
    if(bmp_fd==NULL)  
    {  
    printf("bmp_fd:open file failed!/n") ;  
     return FALSE;  
    }
	
    fwrite( &bfh, 8, 1, bmp_fd);   
    fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, bmp_fd);    
    fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, bmp_fd);    
    fwrite(&bih, sizeof(BMPINFOHEADER_T), 1, bmp_fd);    
    fwrite(pdata, size, 1, bmp_fd);
	fclose(bmp_fd);
	return TRUE;
	
}


  
BOOL encode_jpeg(char *lpbuf, int width, int height)  
{  
    struct jpeg_compress_struct cinfo ;  
    struct jpeg_error_mgr jerr ;  
    JSAMPROW  row_pointer[1] ;  
    int row_stride ;  
    char *buf=NULL ;  
    int x ;  
  
    FILE *fptr_jpg = fopen (JPEG,"wb");//注意这里为什么用fopen而不用open  
    if(fptr_jpg==NULL)  
    {  
    printf("Encoder:open file failed!/n") ;  
     return FALSE;  
    }  
  
    cinfo.err = jpeg_std_error(&jerr);  
    jpeg_create_compress(&cinfo);  
    jpeg_stdio_dest(&cinfo, fptr_jpg);  
  
    cinfo.image_width = width;  
    cinfo.image_height = height;  
    cinfo.input_components = 3;  
    cinfo.in_color_space = JCS_RGB;  
  
    jpeg_set_defaults(&cinfo);  
  
  
    jpeg_set_quality(&cinfo, 80,TRUE);  
  
  
    jpeg_start_compress(&cinfo, TRUE);  
  
    row_stride = width * 3;  
    buf=malloc(row_stride) ;  
    row_pointer[0] = buf;  
    while (cinfo.next_scanline < height)  
    {  
		for (x = 0; x < row_stride; x+=3)  
		{  
  
			buf[x]   = lpbuf[x];  
			buf[x+1] = lpbuf[x+1];  
			buf[x+2] = lpbuf[x+2];  
  
		}  
    jpeg_write_scanlines (&cinfo, row_pointer, 1);//critical  
    lpbuf += row_stride;  
    }  
  
    jpeg_finish_compress(&cinfo);  
    fclose(fptr_jpg);  
    jpeg_destroy_compress(&cinfo);  
    free(buf) ;  
    return TRUE ;  
  
}    
  
int close_v4l2(void)  
{  
     if(fd != -1)   
     {  
         close(fd);  
         return (TRUE);  
     }  
     return (FALSE);  
}  
 

 
  
int main(void)  
{  
    FILE * fp2;  
    
    if(init_v4l2() == FALSE)   
    {  
    return(FALSE);  
    }  
    v4l2_grab();  
  
    yuyv_2_rgb888();                  //yuyv to RGB24  
	
    rgb_to_bmp(frame_buffer,IMAGEWIDTH,IMAGEHEIGHT);//RGB24 to bmp
    
  	printf("save "BMP"OK\n");
	
 	encode_jpeg(frame_buffer,IMAGEWIDTH,IMAGEHEIGHT); //RGB24 to Jpeg  
    printf("save "JPEG"OK\n"); 
	
    close_v4l2();  
  
    return(TRUE);  
} 
