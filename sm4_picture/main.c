#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bmp.h"
#include "SM4.H""

char filename[50]; //输入文件名
/*图像加密*/
void bmpEnc(char *filename)
{
    //文件指针
    FILE* fp = NULL;
    //用于存储imread的结果
    size_t result;
    //SM4加密体系
    sm4_context ctx;
    //真实数据量的大小
    int num;
    //是否需要生成真实图像矩阵  默认为0 即不用生成图像矩阵
    int flag = 0;
    //加密图像时是否二次加密  默认为0 即不用二次加密
    int state = 0;
    //完整分组密码的长度
    int length_whole;
    //需要补全的分组密码的长度
    int length_extra;
    //bmp图像的宽度 高度
    int bmpWidth, bmpHeight;
    //真实数据矩阵的高度 宽度
    int materixHeight, materixWidth;
    //真实数据矩阵
    int* matrix = NULL;
    //图像头指针
    unsigned char* imageBmpHead;
    //图像读取指针
    unsigned char* imageBin;
    //存储真实图像数据的指针
    unsigned char* realImageData = NULL;
    //加密后的十六进制数组
    unsigned char* afterEnc;
    //16长度的十六进制数组
    unsigned char tem[16];
    //加密得到的十六进制数组
    unsigned char Enc[16];
    //密钥
    unsigned char key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
    //不知道为什么最后存在的两个零
    unsigned char end[2] = { 0x00, 0x00 };

    //读图片并生成真实矩阵
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("file open failed\r\n");
        return -1;
    }
    bmpWidth = getBmpWidth(fp);
    bmpHeight = getBmpHeight(fp);
    materixHeight = bmpHeight;
    materixWidth = bmpWidth * 3;
    while (materixWidth % 4 != 0)
    {
        //能进来说明需要生成真实矩阵
        flag = 1;
        materixWidth++;
    }
    //生成真实数据对应矩阵
    if (flag == 1)
    {
        matrix = (int*)malloc(sizeof(int) * materixHeight * materixWidth);
        for (int i = 0; i < materixHeight; i++)
        {
            for (int j = 0; j < materixWidth; j++)
            {
                if (j < bmpWidth * 3)
                {
                    matrix[i * materixWidth + j] = 1;
                }
                else
                {
                    matrix[i * materixWidth + j] = 0;
                }
            }
        }
    }

    //给文件头分配空间
    fseek(fp, 0, SEEK_SET);
    imageBmpHead = (unsigned char*)malloc(sizeof(unsigned char) * 54);
    if (imageBmpHead == NULL)
    {
        printf("imageBmpHead malloc failed");
        return -1;
    }
    //读取文件头
    result = fread(imageBmpHead, 1, 54, fp);
    if (result != 54)
    {
        printf("file read failed");
        return -1;
    }
    else
    {
        printf("\t\t\tbmpHeaderSize:%d\n", result);
    }

    //给图像数据分配空间
    imageBin = (unsigned char*)malloc(sizeof(unsigned char) * materixHeight * materixWidth);
    if (imageBin == NULL)
    {
        printf("imageBin malloc failed");
        return -1;
    }
    //存储图像数据
    fseek(fp, 54, SEEK_SET);
    result = fread(imageBin, 1, materixHeight * materixWidth, fp);
    if (result != materixHeight * materixWidth)
    {
        printf("file read failed");
        return -1;
    }
    else
    {
        printf("\t\t\tImageSize:%d\n", result);
    }

    //为真实数据分配空间
    realImageData = (unsigned char*)malloc(sizeof(unsigned char) * bmpHeight * bmpWidth * 3);
    //将真实数据存储到空间

    if (flag == 1) //生成图像矩阵
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            if (matrix[i] == 1)
            {

                realImageData[num] = imageBin[i];
                num++;
            }
        }
    }
    else //读入的图像数据就是真实数据
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            realImageData[num] = imageBin[i];
            num++;
        }
    }

    //对数据进行加密
    //为加密后的数据分配空间
    afterEnc = (unsigned char*)malloc(sizeof(unsigned char) * bmpHeight * bmpWidth * 3);
    sm4_setkey_enc(&ctx, key); //设置密钥
    //判断是否需要二次加密
    if ((bmpHeight * bmpWidth * 3) % 16 != 0) //4px*4px图像共有48个 unsigned char 对应state=0
    {
        state = 1;
    }
    if (state == 1) //需要二次加密
    {
        for (int i = 0;; i += 16)
        {
            if (i + 16 >= bmpHeight * bmpWidth * 3)
            {
                length_whole = i; //以下标为准进行计算
                break;
            }
            for (int j = 0; j < 16; j++)
            {
                tem[j] = realImageData[i + j];
            }
            sm4_crypt_ecb(&ctx, 1, 16, tem, Enc); //以16为一组进行加密
            for (int k = 0; k < 16; k++)
            {
                afterEnc[i + k] = Enc[k];
            }
        }
        //进行额外部分的加密
        printf("extraMingwen:\n");
        for (int i = bmpHeight * bmpWidth * 3 - 16; i < length_whole; i++)
        {
            tem[i - bmpHeight * bmpWidth * 3 + 16] = afterEnc[i];
        }
        for (int j = length_whole; j < bmpHeight * bmpWidth * 3; j++)
        {
            tem[j - bmpHeight * bmpWidth * 3 + 16] = realImageData[j];
        }
        sm4_crypt_ecb(&ctx, 1, 16, tem, Enc); //以16为一组进行加密
        printf("AfterEnc extraMingwen:\n");
        for (int i = bmpHeight * bmpWidth * 3 - 16; i < bmpHeight * bmpWidth * 3; i++)
        {
            afterEnc[i] = Enc[i - bmpHeight * bmpWidth * 3 + 16];
        }
    }
    else //直接进行加密
    {
        for (int i = 0; i < bmpHeight * bmpWidth * 3; i += 16)
        {
            for (int j = 0; j < 16; j++)
            {
                tem[j] = realImageData[i + j];
            }
            sm4_crypt_ecb(&ctx, 1, 16, tem, Enc); //以16为一组进行加密
            for (int k = 0; k < 16; k++)
            {
                afterEnc[i + k] = Enc[k];
            }
        }
    }
    //将加密后的数据返回到图片中
    //判断是否生成了真实数据矩阵
    if (flag == 1) //生成了真实矩阵
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            if (matrix[i] == 1)
            {
                imageBin[i] = afterEnc[num];
                //    printf("%02X  ", afterEnc[num]);
                num++;
            }
        }
    }
    else //无缝衔接
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            imageBin[num] = afterEnc[i];
            num++;
        }
    }

    fp = fopen("afterEncImage.bmp", "wb");
    if (fp == 0)
    {
        return -1;
    }
    fwrite(imageBmpHead, 1, 54, fp);
    fwrite(imageBin, 1, materixHeight * materixWidth, fp);
    fwrite(end, 1, 2, fp);
    fclose(fp);
    free(imageBin);
    free(imageBmpHead);
    free(realImageData);
    free(afterEnc);
}

/*图像解密*/
void bmpDec()
{
    //文件指针
    FILE* fp = NULL;
    //用于存储imread的结果
    size_t result;
    //SM4加密体系
    sm4_context ctx;
    //真实数据量的大小
    int num;
    //是否需要生成真实图像矩阵  默认为0 即不用生成图像矩阵
    int flag = 0;
    //加密图像时是否二次加密  默认为0 即不用二次加密
    int state = 0;
    //完整分组密码的长度
    int length_whole;
    //需要补全的分组密码的长度
    int length_extra;
    //bmp图像的宽度 高度
    int bmpWidth, bmpHeight;
    //真实数据矩阵的高度 宽度
    int materixHeight, materixWidth;
    //真实数据矩阵
    int* matrix = NULL;
    //图像头指针
    unsigned char* imageBmpHead;
    //图像读取指针
    unsigned char* imageBin;
    //存储真实图像数据的指针
    unsigned char* realImageData = NULL;
    //加密后的十六进制数组
    unsigned char* afterDec;
    //16长度的十六进制数组
    unsigned char tem[16];
    //加密得到的十六进制数组
    unsigned char dec[16];
    //密钥
    unsigned char key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
    //不知道为什么最后存在的两个零
    unsigned char end[2] = { 0x00, 0x00 };

    //读图片并生成真实矩阵
    fp = fopen("afterEncImage.bmp", "rb");
    if (fp == NULL)
    {
        printf("file open failed.\n");
        return -1;
    }
    bmpWidth = getBmpWidth(fp);
    bmpHeight = getBmpHeight(fp);
    materixHeight = bmpHeight;
    materixWidth = bmpWidth * 3;
    length_whole = ((int)((bmpHeight * bmpWidth * 3) / 16)) * 16;
    length_extra = (bmpHeight * bmpWidth * 3) % 16;
    while (materixWidth % 4 != 0)
    {
        //能进来说明需要生成真实矩阵
        flag = 1;
        materixWidth++;
    }
    //生成真实数据对应矩阵
    if (flag == 1)
    {
        matrix = (int*)malloc(sizeof(int) * materixHeight * materixWidth);
        for (int i = 0; i < materixHeight; i++)
        {
            for (int j = 0; j < materixWidth; j++)
            {
                if (j < bmpWidth * 3)
                {
                    matrix[i * materixWidth + j] = 1;
                }
                else
                {
                    matrix[i * materixWidth + j] = 0;
                }
            }
        }
    }

    //给文件头分配空间
    fseek(fp, 0, SEEK_SET);
    imageBmpHead = (unsigned char*)malloc(sizeof(unsigned char) * 54);
    if (imageBmpHead == NULL)
    {
        printf("imageBmpHead malloc failed");
        return -1;
    }
    //读取文件头
    result = fread(imageBmpHead, 1, 54, fp);
    if (result != 54)
    {
        printf("file read failed");
        return -1;
    }
    else
    {
        printf("\t\t\tbmpHeaderSize:%d\n", result);
    }

    //给图像数据分配空间
    imageBin = (unsigned char*)malloc(sizeof(unsigned char) * materixHeight * materixWidth);
    if (imageBin == NULL)
    {
        printf("imageBin malloc failed");
        return -1;
    }
    //存储图像数据
    fseek(fp, 54, SEEK_SET);
    result = fread(imageBin, 1, materixHeight * materixWidth, fp);
    if (result != materixHeight * materixWidth)
    {
        printf("file read failed");
        return -1;
    }
    else
    {
        printf("\t\t\tImageSize:%d\n", result);
    }

    //为真实数据分配空间
    realImageData = (unsigned char*)malloc(sizeof(unsigned char) * bmpHeight * bmpWidth * 3);
    //将真实数据存储到空间

    if (flag == 1) //生成图像矩阵
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            if (matrix[i] == 1)
            {

                realImageData[num] = imageBin[i];
                num++;
            }
        }
    }
    else //读入的图像数据就是真实数据
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            realImageData[num] = imageBin[i];
            num++;
        }
    }
 //对数据进行解密
 //为解密后的数据分配空间
    afterDec = (unsigned char*)malloc(sizeof(unsigned char) * bmpHeight * bmpWidth * 3);
    sm4_setkey_dec(&ctx, key); //设置密钥
    //判断是否需要二次解密
    if ((bmpHeight * bmpWidth * 3) % 16 != 0) //4px*4px图像共有48个 unsigned char 对应state=0
    {
        state = 1;
    }
    if (state == 1) //需要二次解密
    {
        //先对额外的部分进行解密
        printf("extraMiwen:\n");
        for (int i = bmpHeight * bmpWidth * 3 - 16; i < bmpHeight * bmpWidth * 3; i++)
        {
            tem[i - bmpHeight * bmpWidth * 3 + 16] = realImageData[i];
        }
        for (int i = 0; i < 16; i++)
        {
            //    printf("%02X  ", tem[i]);
        }
        printf("\n");
        sm4_crypt_ecb(&ctx, 0, 16, tem, dec); //以16为一组进行加密
        printf("The first Dec:\n");
        /* for (int i = 0; i < 16; i++)
        {
            printf("%02X  ", dec[i]);
        }
        printf("\n"); */
        for (int j = bmpHeight * bmpWidth * 3 - 16; j < length_whole; j++)
        {
            realImageData[j] = dec[j - bmpHeight * bmpWidth * 3 + 16];
        }
        for (int j = length_whole; j < bmpHeight * bmpWidth * 3; j++)
        {
            afterDec[j] = dec[j - bmpHeight * bmpWidth * 3 + 16];
        }
        //再进行分组解密
        for (int i = 0; i + 16 < bmpHeight * bmpWidth * 3; i += 16)
        {
            for (int j = 0; j < 16; j++)
            {
                tem[j] = realImageData[i + j];
                //    printf("%02X  ", tem[j]);
            }
            printf("\n");
            sm4_crypt_ecb(&ctx, 0, 16, tem, dec); //以16为一组进行加密
            for (int k = 0; k < 16; k++)
            {
                afterDec[i + k] = dec[k];
                //    printf("%02X  ", afterDec[i + k]);
            }
            printf("\n");
        }
        //将额外解密出来的明文部分放入结果矩阵
    }
    else //不需要二次解密
    {
        for (int i = 0; i < bmpHeight * bmpWidth * 3; i += 16)
        {
            for (int j = 0; j < 16; j++)
            {
                tem[j] = realImageData[i + j];
            }
            sm4_crypt_ecb(&ctx, 0, 16, tem, dec); //以16为一组进行加密
            for (int k = 0; k < 16; k++)
            {
                afterDec[i + k] = dec[k];
            }
        }
    }
    //将加密后的数据返回到图片中
    //判断是否生成了真实数据矩阵
    if (flag == 1) //生成了真实矩阵
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            if (matrix[i] == 1)
            {
                imageBin[i] = afterDec[num];
                num++;
            }
        }
    }
    else //无缝衔接
    {
        num = 0; //
        for (int i = 0; i < materixHeight * materixWidth; i++)
        {
            imageBin[i] = afterDec[num];
            num++;
        }
    }

    fp = fopen("afterDecImage.bmp", "wb");
    if (fp == 0)
    {
        return -1;
    }
    fwrite(imageBmpHead, 1, 54, fp);
    fwrite(imageBin, 1, materixHeight * materixWidth, fp);
    fwrite(end, 1, 2, fp);
    fclose(fp);
    free(imageBin);
    free(imageBmpHead);
    free(realImageData);
    free(afterDec);
}
int main()
{
    time_t begin, end;
    printf("----------------------------SM4图像加密----------------------------\n\n");
    printf("请输入要加密的BMP文件：");
    scanf("%s", filename);
    printf("\n\t\t*************开始加密*************\n");
    begin = clock();
    bmpEnc(filename);
    end = clock();
    printf("\n\t\t\t加密时间: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
    printf("\n");

    printf("\n\t\t*************开始解密*************\n");
    begin = clock();
    bmpDec();
    end = clock();
    printf("\n\t\t\t解密时间: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
    printf("\n");
    system("pause");
	return 0;
}