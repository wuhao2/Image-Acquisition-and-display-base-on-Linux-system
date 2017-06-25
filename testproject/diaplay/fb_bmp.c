// ���ļ���������BMPͼƬ��������ʾ��fb��
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "bmp.h"
#include <fb.h>
#include <config.h>


// �ж�һ��ͼƬ�ļ��ǲ���һ���Ϸ���bmp�ļ�
// ����ֵ: ������򷵻�0�������򷵻�-1
int is_bmp(const char *path)
{
	int fd = -1;
	unsigned char buf[2] = {0};
	ssize_t ret = 0;
	
	// ��һ��: ��bmpͼƬ
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", path);
		return -1;
	}
	
	// �ڶ���: ��ȡ�ļ�ͷ��Ϣ
	ret = read(fd, buf, 2);
	if (ret !=	2)
	{
		fprintf(stderr, "read file header error.\n");
		ret = -1;
		goto close;
	}
	
	// ����ͷ
	// ������: �ж��ǲ���BMPͼƬ
	if ((buf[0] != 'B') || (buf[1] != 'M'))
	{
		//fprintf(stderr, "file %s is not a bmp picture.\n", path);
		ret = -1;
		goto close;
	}
	else
	{
		ret = 0;
	}

close:
	close(fd);
	return ret;
}




// �����б�: path: Ҫ������bmpͼƬ��pathname
// ��������: �ú�������path���bmpͼƬ�����ҽ�����������ͼƬ���ݶ�����rgb_buf��ȥ
// ����ֵ  : ����ʱ����-1��������ȷ����0
static int bmp_analyze(struct pic_info *pPic)
{
	int fd = -1;
	ClBitMapFileHeader fHeader;
	ClBitMapInfoHeader info;
	unsigned short tmp;
	unsigned long len;

	// ��һ��: ��bmpͼƬ
	fd = open(pPic->pathname, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", pPic->pathname);
		return -1;
	}

	// �ڶ���: ��ȡ�ļ�ͷ��Ϣ
	read(fd, &tmp, 2);
	read(fd, &fHeader, sizeof(fHeader));
	debug("bfSize = %ld.\n", fHeader.bfSize);
	debug("bfOffBits = %ld.\n", fHeader.bfOffBits);

	read(fd, &info, sizeof(info));
	debug("picutre resolution: %ld x %ld.\n", info.biWidth, info.biHeight);
	debug("picture bpp: %d.\n", info.biBitCount);
	pPic->width = info.biWidth;
	pPic->height = info.biHeight;
	pPic->bpp = info.biBitCount;			// ��������Ͳ���

	// ������: ��ȡͼƬ��Ч��Ϣ
	// �Ȱ��ļ�ָ���ƶ�����Ч��Ϣ��ƫ������
	lseek(fd, fHeader.bfOffBits, SEEK_SET);
	// Ȼ�����info.biWidth * info.biHeight * info.biBitCount / 3 ��ô���ֽڼ���
	len = info.biWidth * info.biHeight * info.biBitCount / 3;
	read(fd, rgb_buf, len);
	//pPic->pData = rgb_buf;

	// ���Ĳ�: �����ݶ���fb��ȥ��ʾ
	fb_draw(pPic);
	

	// ���رմ򿪵��ļ�
	close(fd);
	
	return 0;
}


// ��װ��һ������ʹ�õ�bmp��ʾ����
// ����������ֻ��Ҫһ��bmpͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����bmp��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1
int display_bmp(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;
	
	// ��һ��: ������ͼƬ�ǲ���bmpͼƬ
	ret = is_bmp(pathname);
	if (ret != 0)
	{
		return -1;
	}
	// �ڶ���: ��ʾ��jpgͼƬ
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	bmp_analyze(&picture);
}



















