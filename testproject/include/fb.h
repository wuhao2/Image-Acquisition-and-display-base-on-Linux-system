#include <config.h>


 // 宏定义
#define FBDEVICE	"/dev/fb0"
 
 // 旧开发板
 //#define WIDTH	 800 
 //#define HEIGHT	 480
 
 // 新开发板
#define WIDTH		1024	
#define HEIGHT		600
 
 
#define WHITE		0xffffffff			// test ok
#define BLACK		0x00000000
#define RED			0x00ff0000
#define GREEN		0xff00ff00			// test ok
#define BLUE		0xff0000ff			
 

 
 // 函数声明
 int fb_open(void);
 void fb_close(void);
 void fb_draw_back(unsigned int width, unsigned int height, unsigned int color);
 void fb_draw_line(unsigned int color);
 /*
 void fb_draw_picutre(void);
 void fb_draw_picutre2(void);
 void fb_draw_picutre3(void);
 void fb_draw_picutre4(void);
 void fb_draw_picutre5(void);
 void fb_draw_picutre6(unsigned int x0, unsigned int y0);
 void fb_draw_picutre7(unsigned int x0, unsigned int y0);
 void fb_draw_picutre8(unsigned int x0, unsigned int y0);
 void fb_draw_picutre9(unsigned int x0, unsigned int y0);
 */


// 这两个显示函数的差异是: 彼此的rgb顺序相反且转了180度
void fb_draw(const struct pic_info *pPic);
void fb_draw2(const struct pic_info *pPic);













