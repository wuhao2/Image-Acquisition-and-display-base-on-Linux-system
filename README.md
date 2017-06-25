## Image-Acquisition-and-display-base-on-Linux-system

According to the need of  automatic production in the field of industrial control, an image acquisition and display system based on embedded Linux platform and USB Video Class type camera is designed. The system uses Samsung S5Pv210 as core controller, Calling V4L2 programming interface provided by the Linux kernel, using the UVC types of USB camera multi-channel images acquisition based on Epoll architecture. The image data collected YUVU format is encoded, saved as BMP and JPEG file formats for subsequent development and application. The system Callings libjpeg library function to decode JPEG images, and make image displayed in the TFT LCD screen.


## System structure

* System hardware platform

![ hardware platform](https://github.com/wuhao2/Image-Acquisition-and-display-base-on-Linux-system-/blob/master/picture_for_markdown/hardware.jpg)

* System software platform

![System software platform](https://github.com/wuhao2/Image-Acquisition-and-display-base-on-Linux-system-/blob/master/picture_for_markdown/software.jpg)


## build develop environment

*  Hardware: PC (host Win7 X64, virtual machine ubuntu14.04) + development board (X210)

* software: linux (based on linux API, framebuffer driver, V4l2)

* development board uboot (uboot can be in iNand, can also be in the external SD card)

* transplanted kernel (zImage in tftp server, or zImage direct fastboot way to burn to iNand)

* own rootfs (in fact, is the second part of the transplant course in the production of rootfs produced that)

* host ubuntu tftp server

* host ubuntu nfs server

## Other small details
* Code editor: SourceInsight
* code management: Makefile
* debugging process: Windows shared folder editing, virtual machine ubuntu in the compiler, make cp to nfs format rootfs run on the development board
* Development board standard: V3S kernel

## project function
* Based on v4l2 image acquisition
* Bmp, jpg, png and other pictures of the show
* Playlist organization, automatically identify and display a variety of formats of mixed library files
* Automatic timing switching sequence play


## The results show

![spot2](https://github.com/wuhao2/Image-Acquisition-and-display-base-on-Linux-system-/blob/master/picture_for_markdown/show2.jpg)

## About the author
   * who care！！！
