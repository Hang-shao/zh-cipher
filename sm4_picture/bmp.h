//bmp相关头文件
#include <stdio.h>
#include <stdlib.h>
int getBmpWidth(FILE *fpbmp)
{
    int width = 0;
    fseek(fpbmp, 18L, SEEK_SET);
    fread(&width, sizeof(char), 4, fpbmp);
    return width;
}
int getBmpHeight(FILE *fpbmp)
{
    int height = 0;
    fseek(fpbmp, 22L, SEEK_SET);
    fread(&height, sizeof(char), 4, fpbmp);
    return height;
}
