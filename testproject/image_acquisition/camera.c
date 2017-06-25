
/////////////////////////////////////////////////////////////////////
#if 0
struct v4l2_capability  
{  
	__u8 driver[16];     // ��������  
	__u8 card[32];       // �豸����  
	__u8 bus_info[32]; // �豸��ϵͳ�е�λ��  
	__u32 version;       // �����汾��  
	__u32 capabilities;  // �豸֧�ֵĲ���  
	__u32 reserved[4]; // �����ֶ�  
};


struct v4l2_format  
{  
	enum v4l2_buf_type type;// ֡���ͣ�Ӧ�ó�������  
	union fmt  
 	{  
		struct v4l2_pix_format pix;// ��Ƶ�豸ʹ��  
		struct v4l2_window win;  
		struct v4l2_vbi_format vbi;  
		struct v4l2_sliced_vbi_format sliced;  
		__u8raw_data[200];  
	};  
}; 

struct v4l2_pix_format  
{  
	__u32 width;  // ֡����λ����  
	__u32 height;  // ֡�ߣ���λ����  
	__u32 pixelformat; // ֡��ʽ  
	enum v4l2_fieldfield;  
	__u32 bytesperline;  
	__u32 sizeimage;  
	enum v4l2_colorspace colorspace;  
	__u32 priv;  
};  

struct v4l2_requestbuffers  
{  
	__u32 count;                 // �������ڻ���֡����Ŀ  
	enum v4l2_buf_type type;     // ����֡���ݸ�ʽ  
	enum v4l2_memorymemory;       // �������ڴ�ӳ�仹���û�ָ�뷽ʽ  
	__u32 reserved[2];  
}; 
enum v4l2_memoy{V4L2_MEMORY_MMAP,V4L2_MEMORY_USERPTR}; 

struct v4l2_buffer  
{  
	__u32 index;   //buffer ���  
	enum v4l2_buf_type type;     //buffer ����  
	__u32 byteused;     //buffer ����ʹ�õ��ֽ���  
	__u32 flags;    // ������MMAP ����USERPTR  
	enum v4l2_fieldfield;  
	struct timevaltimestamp;// ��ȡ��һ���ֽ�ʱ��ϵͳʱ��  
	struct v4l2_timecode timecode;  
 	__u32 sequence;// �����е����  
	enum v4l2_memorymemory;//IO ��ʽ����Ӧ�ó�������  
	union m  
	{  
		__u32 offset;// ����֡��ַ��ֻ��MMAP ��Ч  
		unsigned longuserptr;  
	};  
	__u32 length;// ����֡����  
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
     
     /*֡����*/
     buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     buf.memory = V4L2_MEMORY_MMAP;
     ioctl (fd, VIDIOC_DQBUF, &buf);

     write(file_fd,buffers[buf.index].start,buffers[buf.index].length);
	 
     printf("framebuffer comeout queue,you can process this image now.\n");
     /*buf����*/
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

     /*��ȡ������Ϣ*/
      ioctl (fd, VIDIOC_QUERYCAP, &cap);
      printf("Driver Name:%s\n Card Name:%s\n Bus info:%s\n\n",cap.driver,cap.card,cap.bus_info);
          
     /*����ͼ���ʽ*/
     fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     fmt.fmt.pix.width       = 1024;
     fmt.fmt.pix.height      = 600;
     fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
     fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;//V4L2_PIX_FMT_MJPEG

     ioctl(fd, VIDIOC_S_FMT, &fmt);
     printf("width:%d  height:%d \n", fmt.fmt.pix.width, fmt.fmt.pix.height );
	 
     /*����ͼ�񻺳���*/
     req.count               = 4;
     req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     req.memory              = V4L2_MEMORY_MMAP;
     ioctl (fd, VIDIOC_REQBUFS, &req);
   
     
     buffers = calloc (req.count, sizeof (*buffers));
    
  
     for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
     { 
           /*��ȡͼ�񻺳�������Ϣ*/
           buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory      = V4L2_MEMORY_MMAP;
           buf.index       = n_buffers;
 
           ioctl (fd, VIDIOC_QUERYBUF, &buf); 
             
           buffers[n_buffers].length = buf.length; 
           
           // ���ں˿ռ��е�ͼ�񻺳���ӳ�䵽�û��ռ�
          buffers[n_buffers].start = mmap (NULL ,    //ͨ��mmap����ӳ���ϵ
                                        buf.length,
                                        PROT_READ | PROT_WRITE ,
                                        MAP_SHARED ,
                                        fd,
                                        buf.m.offset);
     }

        
     /*ͼ�񻺳����*/ 
       
       for (i = 0; i < n_buffers; ++i)
       {
               buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
               buf.memory      = V4L2_MEMORY_MMAP;
               buf.index       = i; 
               ioctl (fd, VIDIOC_QBUF, &buf);
               
       }
    
    //��ʼ��׽ͼ������  
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMON, &type);


   fd_set fds;

   FD_ZERO (&fds);
   FD_SET (fd, &fds);

   select(fd + 1, &fds, NULL, NULL, NULL);
   printf("select finish,have captured a framedata \n");
   
   /*��ȡһ��ͼ��*/
   read_frame();

   for (i = 0; i < n_buffers; ++i)
      munmap (buffers[i].start, buffers[i].length);   

 

   close (fd);
   close (file_fd);
   printf("Camera Done.\n");


   return 0;
}