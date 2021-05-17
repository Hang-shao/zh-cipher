#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <STDLIB.H>
#include <time.h>
#define ENCRYPT  0     //������ܱ�־
#define DECRYPT  1     //������ܱ�־
int i;
typedef unsigned char muint8;
typedef unsigned int muint32;

//muint32* md;
const muint8 Sbox[256] = {
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};

const muint32 CK[32] = {
	0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
	0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
	0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
	0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279 };

#define Rotl(_x, _y) (((_x) << (_y)) | ((_x) >> (32 - (_y))))

#define ByteSub(_A) (Sbox[(_A) >> 24 & 0xFF] << 24 ^ \
                     Sbox[(_A) >> 16 & 0xFF] << 16 ^ \
                     Sbox[(_A) >>  8 & 0xFF] <<  8 ^ \
                     Sbox[(_A) & 0xFF])

#define L1(_B) ((_B) ^ Rotl(_B, 2) ^ Rotl(_B, 10) ^ Rotl(_B, 18) ^ Rotl(_B, 24))
#define L2(_B) ((_B) ^ Rotl(_B, 13) ^ Rotl(_B, 23))

// SMS4�ļӽ��ܺ���
// ����˵����InputΪ������Ϣ���飬OutputΪ������飬rkΪ����Կ
muint8* ISD_SMS4Crypt(muint8* Input, muint8* Output, muint32* rk)
{
	muint32 r, mid, x0, x1, x2, x3, * p;
	p = (muint32*)Input;
	x0 = p[0];
	x1 = p[1];
	x2 = p[2];
	x3 = p[3];
	for (r = 0; r < 32; r += 4)
	{
		mid = x1 ^ x2 ^ x3 ^ rk[r + 0]; //X1^X2^X3^CK[i]
		mid = ByteSub(mid);  //S��
		x0 ^= L1(mid);   //X0 = X0^L�任
		mid = x2 ^ x3 ^ x0 ^ rk[r + 1];
		mid = ByteSub(mid);
		x1 ^= L1(mid);
		mid = x3 ^ x0 ^ x1 ^ rk[r + 2];
		mid = ByteSub(mid);
		x2 ^= L1(mid);
		mid = x0 ^ x1 ^ x2 ^ rk[r + 3];
		mid = ByteSub(mid);
		x3 ^= L1(mid);
	}
	p = (muint32*)Output; 
	//����û�
	p[0] = x3;
	p[1] = x2;
	p[2] = x1;
	p[3] = x0;
	return Output;
}

// SMS4����Կ��չ�㷨
// ����˵����KeyΪ������Կ(128bit)��rkΪ����Կ��CryptFlagΪ�ӽ��ܱ�־
void ISD_SMS4KeyExt(muint8* Key, muint32* rk, muint32 CryptFlag)
{
	muint32 r, mid, x0, x1, x2, x3, * p;
	p = (muint32*)Key;
	x0 = p[0]; //MK0 (32bit)
	x1 = p[1]; //MK1
	x2 = p[2]; //MK2
	x3 = p[3]; //MK3

	x0 ^= 0xa3b1bac6; //MK0^FK0 = K0
	x1 ^= 0x56aa3350; //MK1^FK1 = K1
	x2 ^= 0x677d9197; //MK2^FK2 = K2
	x3 ^= 0xb27022dc; //MK3^FK3 = K3
	for (r = 0; r < 32; r += 4)
	{
		mid = x1 ^ x2 ^ x3 ^ CK[r + 0]; //K1^K2^K3^CK[i]
		mid = ByteSub(mid);  //S�б任
		rk[r + 0] = x0 ^= L2(mid); // K0^L�任 ������Կ����
		mid = x2 ^ x3 ^ x0 ^ CK[r + 1];
		mid = ByteSub(mid);
		rk[r + 1] = x1 ^= L2(mid);
		mid = x3 ^ x0 ^ x1 ^ CK[r + 2];
		mid = ByteSub(mid);
		rk[r + 2] = x2 ^= L2(mid);
		mid = x0 ^ x1 ^ x2 ^ CK[r + 3];
		mid = ByteSub(mid);
		rk[r + 3] = x3 ^= L2(mid);
	}
	if (CryptFlag == DECRYPT)
	{
		for (r = 0; r < 16; r++)
			mid = rk[r], rk[r] = rk[31 - r], rk[31 - r] = mid;
	}
}

int main()
{
	time_t begin,end;
	muint8 key[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98 ,0x76,0x54,0x32,0x10 }; //128bit
	muint8 m[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98 ,0x76,0x54,0x32,0x10 }; //128bit
	muint8 c[16];
	muint32 rk[32];
	printf("================SMS4�����㷨��ʾ================\n\n");
	int route;
	printf("��������ܵĴ���:");
	scanf("%d", &route);

	begin = clock();
	ISD_SMS4KeyExt((muint8*)key, rk, ENCRYPT);
	muint8*md = ISD_SMS4Crypt(m, c, rk);
	for (i = 0; i < route - 1; i++)
	{
		md=ISD_SMS4Crypt(md,c, rk);
	}
	printf("\n����������Ϊ��");
	for (int i = 0; i < 16; i++) {
		printf("%x ", m[i]);
	}
	printf("\n���ܺ�����Ϊ��");
	for (i = 0; i < 16; i++) {
		printf("%x ", c[i]);
	}
	end = clock();
	printf("\n--------����ʱ��: %f seconds\n", double(end - begin) / CLOCKS_PER_SEC);
	printf("\n");
	begin = clock();
	ISD_SMS4KeyExt((muint8*)key, rk, DECRYPT);

	muint8* mn = ISD_SMS4Crypt(c, m, rk);
	for (i = 0; i < route - 1; i++)
	{
		mn = ISD_SMS4Crypt(mn, m, rk);
	}
	printf("���ܺ�����Ϊ��");
	for (i = 0; i < 16; i++) {
		printf("%x ", m[i]);
	}
	end = clock();
	printf("\n--------����ʱ��: %f seconds\n", double(end - begin) / CLOCKS_PER_SEC);
	printf("\n");
	system("pause");
	return 0;
}