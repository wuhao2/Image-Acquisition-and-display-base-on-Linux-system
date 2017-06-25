#ifndef __BMP_H__
#define __BMP_H__




// BMP 文件头
typedef struct  
{  
//    unsigned short    bfType;  			// 2
    unsigned long    	bfSize; 			// 4 
    unsigned short    	bfReserved1;  
    unsigned short    	bfReserved2;  
    unsigned long    	bfOffBits;  
} ClBitMapFileHeader;

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
} ClBitMapInfoHeader;  






#endif













