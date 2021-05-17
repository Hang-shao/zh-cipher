#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <STDLIB.H>
#include <time.h>
#include "hex2char.h"
#include "sm4.h"
int main()
{
	unsigned char key[16];
	unsigned char key1[8];
	unsigned char* input;
	unsigned char* input1;
	unsigned char* output;
	char* m;
	char* p;
	sm4_context ctx;
	time_t begin, end;
	unsigned long  int len, j, i, n, k;
	int len1;
	k = 12;
	FILE* fp = NULL, * fJ, * fq, * fa;
	FILE* secret = NULL;

	output = (unsigned char*)malloc(200000);
	input = (unsigned char*)malloc(200000);
	input1 = (unsigned char*)malloc(200000);
	m = (char*)malloc(200000);
	p = (char*)malloc(200000);
	memset(output, '\0', 200000);
	memset(input, '\0', 200000);
	memset(input1, '\0', 200000);
	char filename[256];

	printf("----------------------------SM4-txt加密----------------------------\n\n");
	printf("\t\t*************开始加密*************\n\n");
	begin = clock();
	secret = fopen("secret.txt", "rb");
	if (secret == NULL)
	{
		printf("file open failed\r\n");
		return -1;
	}
	for (i = 0; i < 8; i++)
	{
		key1[i] = fgetc(secret);
	}
	for (i = 0; i < 8;)
	{
		if (key1[i] > 90)
		{
			key1[i] = key1[i] - 87;
		}
		else
		{
			key1[i] = key1[i] - 48;
		}
		if (key1[i + 1] > 90)
		{
			key1[i + 1] = key1[i + 1] - 87;
		}
		else
		{
			key1[i + 1] = key1[i + 1] - 48;
		}
		key[k] = key1[i] * 16 + key1[i + 1];
		k = k + 1;
		i = i + 2;
	}
	rewind(secret);
	fclose(secret);

	//******************************进行加密**************************************//
	printf("选择要加密的文件:");
	scanf("%s", &filename);
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("读取文件失败 \n ");
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	rewind(fp);
	for (j = 0; j < len; j++)
	{
		m[j] = fgetc(fp);
	}
	for (n = 0; n < len; n++)
	{
		input[n] = m[n];
	}
	sm4_setkey_enc(&ctx, key);
	sm4_crypt_ecb(&ctx, 1, len, input, output);

	fJ = fopen("jiami.txt", "wb");
	for (i = 0; i < len + 16; i++)
		fprintf(fJ, "%02x", output[i]);
	fclose(fJ);
	printf("\n加密完成,密文写入到jiami.txt中\n");
	end = clock();
	printf("\n\t\t\t加密时间: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf("\n");

	//*******************************进行解密******************************//
	printf("\n\t\t*************开始解密*************\n");
	begin = clock();
	if ((fa = fopen("jiami.txt", "r")) == NULL)
	{
		printf("读取文件失败 \n ");
		exit(1);
	}
	fseek(fa, 0L, SEEK_END);
	len1 = ftell(fa);

	rewind(fa);
	for (j = 0; j < len1; j++)
	{
		p[j] = fgetc(fa);

	}
	charToByte(p, input1);
	for (j = 0; j < len1; j++)
	{
		input1[i] = p[j];
	}
	printf("\n");
	sm4_setkey_dec(&ctx, key);
	sm4_crypt_ecb(&ctx, 0, len1, input1, output);
	fq = fopen("jiemi.txt", "w");
	for (i = 0; i < len; i++)
		fprintf(fq, "%c", output[i]);
	fclose(fq);
	fclose(fa);
	printf("解密完成，明文写入到jiemi.txt中\n ");
	end = clock();
	printf("\n\t\t\t解密时间: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf("\n");
	system("pause");
	return 0;
}