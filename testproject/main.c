#include <stdio.h>
#include <unistd.h>

#include <fb.h>
#include <config.h>



char rgb_buf[BMP_BUF_SIZE];






int main(void)
{
	int ret = -1;

	printf("image decode player.....\n");

	ret = fb_open();
	if (ret < 0)
	{
		printf("fb_open error.\n");
		return -1;
	}

	//fb_draw_back(1024, 600, RED);
	//fb_draw_picutre6(100, 100);
	//fb_draw_picutre7((1024-500)/2, (600-281)/2);
	//fb_draw_picutre9(800, 100);

// ²âÊÔbmpÍ¼Æ¬ÏÔÊ¾£¬ok	
//	picture.pathname = "meinv.bmp";			// Ö¸ÏòÒªÏÔÊ¾µÄÍ¼Æ¬
//	bmp_analyze(&picture);

// ²âÊÔjpgÍ¼Æ¬ÏÔÊ¾
//	display_bmp("123.bmp");
//	sleep(3);
//	display_jpg("meinv.jpg");

//	display_png("meinv.png");
//	debug("-------------\n");


	scan_image2("./image");
	print_images();
//	while(1)
//		show_images();

	ts_updown();

	
	
	fb_close();

	

	return 0;
}



























