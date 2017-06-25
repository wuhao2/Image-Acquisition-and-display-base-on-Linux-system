
/////////////////////////////////////////////////////////////////////
#if 0
struct v4l2_capability  
{  
	__u8 driver[16];     // 驱动名字  
	__u8 card[32];       // 设备名字  
	__u8 bus_info[32]; // 设备在系统中的位置  
	__u32 version;       // 驱动版本号  
	__u32 capabilities;  // 设备支持的操作  
	__u32 reserved[4]; // 保留字段  
};


struct v4l2_format  
{  
	enum v4l2_buf_type type;// 帧类型，应用程序设置  
	union fmt  
 	{  
		struct v4l2_pix_format pix;// 视频设备使用  
		struct v4l2_window win;  
		struct v4l2_vbi_format vbi;  
		struct v4l2_sliced_vbi_format sliced;  
		__u8raw_data[200];  
	};  
}; 

struct v4l2_pix_format  
{  
	__u32 width;  // 帧宽，单位像素  
	__u32 height;  // 帧高，单位像素  
	__u32 pixelformat; // 帧格式  
	enum v4l2_fieldfield;  
	__u32 bytesperline;  
	__u32 sizeimage;  
	enum v4l2_colorspace colorspace;  
	__u32 priv;  
};  

struct v4l2_requestbuffers  
{  
	__u32 count;                 // 缓冲区内缓冲帧的数目  
	enum v4l2_buf_type type;     // 缓冲帧数据格式  
	enum v4l2_memorymemory;       // 区别是内存映射还是用户指针方式  
	__u32 reserved[2];  
}; 
enum v4l2_memoy{V4L2_MEMORY_MMAP,V4L2_MEMORY_USERPTR}; 

struct v4l2_buffer  
{  
	__u32 index;   //buffer 序号  
	enum v4l2_buf_type type;     //buffer 类型  
	__u32 byteused;     //buffer 中已使用的字节数  
	__u32 flags;    // 区分是MMAP 还是USERPTR  
	enum v4l2_fieldfield;  
	struct timevaltimestamp;// 获取第一个字节时的系统时间  
	struct v4l2_timecode timecode;  
 	__u32 sequence;// 队列中的序号  
	enum v4l2_memorymemory;//IO 方式，被应用程序设置  
	union m  
	{  
		__u32 offset;// 缓冲帧地址，只对MMAP 有效  
		unsigned longuserptr;  
	};  
	__u32 length;// 缓冲帧长度  
	__u32 input;  
	__u32 reserved;  
}; 
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>           
#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
 
#include <asm/types.h>        
#include <linux/videodev2.h>
  
struct buffer {
        void *                  start;
        size_t                  length;
};
 
struct buffer *buffers;
unsigned long  n_buffers;
unsigned long file_length;

int file_fd;
char *dev_name = "/dev/video3";
int fd;

static int read_frame (void)
{
     struct v4l2_buffer buf;
     
     /*帧出列*/
     buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     buf.memory = V4L2_MEMORY_MMAP;
     ioctl (fd, VIDIOC_DQBUF, &buf);

     write(file_fd,buffers[buf.index].start,buffers[buf.index].length);
	 
     printf("framebuffer comeout queue,you can process this image now.\n");
     /*buf入列*/
     ioctl(fd, VIDIOC_QBUF, &buf);

     return 1;
}
 
int main (int argc,char ** argv)
{
     struct v4l2_capability cap;
     struct v4l2_format fmt;
     struct v4l2_requestbuffers req;
     struct v4l2_buffer buf; 
     unsigned int i;
     enum v4l2_buf_type type;
     
     
     file_fd = open("test.jpg", O_RDWR | O_CREAT, 0777);
    
     fd = open (dev_name, O_RDWR | O_NONBLOCK, 0);

     /*获取驱动信息*/
      ioctl (fd, VIDIOC_QUERYCAP, &cap);
      printf("Driver Name:%s\n Card Name:%s\n Bus info:%s\n\n",cap.driver,cap.card,cap.bus_info);
          
     /*设置图像格式*/
     fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     fmt.fmt.pix.width       = 1024;
     fmt.fmt.pix.height      = 600;
     fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
     fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;//V4L2_PIX_FMT_MJPEG

     ioctl(fd, VIDIOC_S_FMT, &fmt);
     printf("width:%d  height:%d \n", fmt.fmt.pix.width, fmt.fmt.pix.height );
	 
     /*申请图像缓冲区*/
     req.count               = 4;
     req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     req.memory              = V4L2_MEMORY_MMAP;
     ioctl (fd, VIDIOC_REQBUFS, &req);
   
     
     buffers = calloc (req.count, sizeof (*buffers));
    
  
     for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
     { 
           /*获取图像缓冲区的信息*/
           buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory      = V4L2_MEMORY_MMAP;
           buf.index       = n_buffers;
 
           ioctl (fd, VIDIOC_QUERYBUF, &buf); 
             
           buffers[n_buffers].length = buf.length; 
           
           // 把内核空间中的图像缓冲区映射到用户空间
          buffers[n_buffers].start = mmap (NULL ,    //通过mmap建立映射关系
                                        buf.length,
                                        PROT_READ | PROT_WRITE ,
                                        MAP_SHARED ,
                                        fd,
                                        buf.m.offset);
     }

        
     /*图像缓冲入队*/ 
       
       for (i = 0; i < n_buffers; ++i)
       {
               buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
               buf.memory      = V4L2_MEMORY_MMAP;
               buf.index       = i; 
               ioctl (fd, VIDIOC_QBUF, &buf);
               
       }
    
    //开始捕捉图像数据  
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMON, &type);


   fd_set fds;

   FD_ZERO (&fds);
   FD_SET (fd, &fds);

   select(fd + 1, &fds, NULL, NULL, NULL);
   printf("select finish,have captured a framedata \n");
   
   /*读取一幅图像*/
   read_frame();

   for (i = 0; i < n_buffers; ++i)
      munmap (buffers[i].start, buffers[i].length);   

 

   close (fd);
   close (file_fd);
   printf("Camera Done.\n");


   return 0;
}