#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <android/log.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

// TAG for logcat
#define TAG "fb_log"

int main(int argc, char **argv)
{
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "The start of framebuffer\n");

	int fp=0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int screensize=0;
    char *fbp = 0;
    unsigned int x = 0;
	unsigned int y = 0;
    int location = 0;
    int bytes_per_pixel;

    int a = 3;
    int b = 4;
    int r = 0;

    fp = open("/dev/graphics/fb0", O_RDWR);

    if(fp < 0)
	{
        printf("Error : Can not open framebuffer device\n");
        exit(1);
    }

    if(ioctl(fp, FBIOGET_FSCREENINFO, &finfo))
	{
        printf("Error reading fixed information\n");
        exit(2);
    }

    if(ioctl(fp, FBIOGET_VSCREENINFO, &vinfo))
	{
        printf("Error reading variable information\n");
        exit(3);
    }

    bytes_per_pixel = vinfo.bits_per_pixel / 8;
    screensize = vinfo.xres * vinfo.yres * bytes_per_pixel;
    printf("x=%d  y=%d  bytes_per_pixel=%d\n", vinfo.xres, vinfo.yres, bytes_per_pixel);
    printf("screensize=%d\n", screensize);

    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if((int)fbp == -1)
    {
         printf("Error: failed to map framebuffer device to memory.\n");
         exit(4);
    }
    
	while(1) 
	{
        x = 0;
        y = 0;
        
		for(x = 0; x < vinfo.xres; x++) 
		{
            y = a * x + b;
            if(y > vinfo.yres)
				break;

			location = x * bytes_per_pixel + y  *  finfo.line_length;

            *(fbp + location)     = 255;  
            *(fbp + location + 1) = 255; 
            *(fbp + location + 2) = 255; 
            *(fbp + location + 3) = 0;  
        }
       
        usleep(40);
    }

    munmap(fbp, screensize);
    close(fp);

    return 0;
}
