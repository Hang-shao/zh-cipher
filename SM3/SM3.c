/************************************************************************
  File name:    SM3.c
  Version:      SM3_V1.1
  Date:         Sep 18,2016
  Description:  to calculate a hash message from a given message
  Function List:
    1.SM3_256        //calls SM3_init, SM3_process and SM3_done to calculate hash value
    2.SM3_init       //init the SM3 state
    3.SM3_process    //compress the the first len/64 blocks of the message
    4.SM3_done       //compress the rest message and output the hash value
    5.SM3_compress   //called by SM3_process and SM3_done, compress a single block of message
    6.BiToW          //called by SM3_compress,to calculate W from Bi
    7.WToW1          //called by SM3_compress, calculate W1 from W
    8.CF             //called by SM3_compress, to calculate CF function.
    9.BigEndian      //called by SM3_compress and SM3_done.GM/T 0004-2012 requires to use
big-endian.
                     //if CPU uses little-endian, BigEndian function is a necessary call to
change the
                     //little-endian format into big-endian format.
    10.SM3_SelfTest  //test whether the SM3 calculation is correct by comparing the hash result
with the standard result
  History:
    1. Date:         Sep 18,2016
       Author:       Mao Yingying, Huo Lili
       Modification: 1)add notes to all the functions
                     2)add SM3_SelfTest function
**************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include "SM3.h" 

/******************************************************************************
  Function:       inputChar
  Description:    输出
*******************************************************************************/
void inputChar(unsigned char* Msg, int len)
{
    int i,count=0;
    for (i = 0; i < len; i++)
    {
        printf("%02X\t", Msg[i]);
        count++;
        if ( count%8 == 0)
        {
            printf("\n");
        }
    }
}


/******************************************************************************
  Function:       inputInt
  Description:    输出
*******************************************************************************/
void inputInt(unsigned int* Msg, int len)
{
    int i, count = 0;
    for (i = 0; i < len; i++)
    {
        printf("%08X\t", Msg[i]);
        count++;
        if (count % 8 == 0)
        {
            printf("\n");
        }
    }
}

/****************************************************************
  Function:       BiToW
  Description:    calculate W from Bi【消息扩展】
  Calls:
  Called By:      SM3_compress
  Input:          Bi[16]    //a block of a message
  Output:         W[64]
  Return:         null
  Others:         将消息分组B[i]扩展生成132个消息字W和W1，这里生成的是W
****************************************************************/
void BiToW(unsigned int Bi[], unsigned int W[])
{
    int i,count=0;
    unsigned int tmp;
    //消息扩展-A
    //将消息分组B[i]划分为16个字W[i]
    for (i = 0; i <= 15; i++)
    {
        W[i] = Bi[i];
    }
    //消息扩展-B
    for (i = 16; i <= 67; i++)
    {
        tmp = W[i - 16]^ W[i - 9]^ SM3_rotl32(W[i - 3], 15);
        W[i] = SM3_p1(tmp)^ (SM3_rotl32(W[i - 13], 7))^ W[i - 6];
    }
    inputInt(W, 68);
}


/*****************************************************************
  Function:       WToW1
  Description:    calculate W1 from W
  Calls:
  Called By:      SM3_compress
  Input:          W[64]
  Output:         W1[64]
  Return:         null
  Others:         将消息分组B[i]扩展生成132个消息字W和W1，这里生成的是W1
*****************************************************************/
void WToW1(unsigned int W[], unsigned int W1[])
{
    int i;
    //消息扩展-C，生成W1
    for (i = 0; i <= 63; i++)
    {
        W1[i] = W[i] ^ W[i + 4];
    }
    inputInt(W1, 64);
}


/******************************************************************
  Function:       CF
  Description:    calculate the CF compress function and update V【压缩函数】
  Calls:
  Called By:      SM3_compress
  Input:          W[64]
                  W1[64]
                  V[8]
  Output:         V[8]
  Return:         null
  Others:
********************************************************************/
void CF(unsigned int W[], unsigned int W1[], unsigned int V[])
{
    unsigned int SS1;
    unsigned int SS2;
    unsigned int TT1;
    unsigned int TT2;
    unsigned int A, B, C, D, E, F, G, H;
    unsigned int T = SM3_T1;
    unsigned int FF;
    unsigned int GG;
    int j;
    printf("\n\n迭代压缩后的中间值：");

    //ABCDEFGH=V0 
    A = V[0];
    B = V[1];
    C = V[2];
    D = V[3];
    E = V[4];
    F = V[5];
    G = V[6];
    H = V[7];
    printf("\nj\tA\t B\t  C\t   D\t    E\t     F\t      G\t       H\n");
    printf("\n\t%08X %08X %08X %08X %08X %08X %08X %08X\n", A, B, C, D, E, F, G, H);
    for (j = 0; j <= 63; j++)
    {
        //SS1 
        if (j == 0)
        {
            T = SM3_T1;
        }
        else if (j == 16)
        {
            T = SM3_rotl32(SM3_T2, 16);
        }
        else
        {
            T = SM3_rotl32(T, 1);
        }
        SS1 = SM3_rotl32((SM3_rotl32(A, 12) + E + T), 7);

        //SS2 
        SS2 = SS1 ^ SM3_rotl32(A, 12);

        //TT1 
        if (j <= 15)
        {
            FF = SM3_ff0(A, B, C);
        }

        else
        {
            FF = SM3_ff1(A, B, C);
        }
        TT1 = FF + D + SS2 + *W1;
        W1++;

        //TT2 
        if (j <= 15)
        {
            GG = SM3_gg0(E, F, G);
        }
        else
        {
            GG = SM3_gg1(E, F, G);
        }
        TT2 = GG + H + SS1 + *W;
        W++;

        //D 
        D = C;

        //C 
        C = SM3_rotl32(B, 9);

        //B 
        B = A;

        //A 
        A = TT1;

        //H 
        H = G;

        //G 
        G = SM3_rotl32(F, 19);

        //F 
        F = E;

        //E 
        E = SM3_p0(TT2);

        printf("\n%d\t%08X %08X %08X %08X %08X %08X %08X %08X\n", j,A, B, C, D, E, F, G, H);
    }

    //update V 
    V[0] = A ^ V[0];
    V[1] = B ^ V[1];
    V[2] = C ^ V[2];
    V[3] = D ^ V[3];
    V[4] = E ^ V[4];
    V[5] = F ^ V[5];
    V[6] = G ^ V[6];
    V[7] = H ^ V[7];
}


/******************************************************************************
  Function:       BigEndian
  Description:    大端，高字节存放在低地址，低字节存放在高地址
  Calls:
  Called By:      SM3_compress, SM3_done
  Input:          src[bytelen]
                  bytelen
  Output:         des[bytelen]
  Return:         null
  Others:         src and des could implies the same address
*******************************************************************************/
void BigEndian(unsigned char src[], unsigned int bytelen, unsigned char des[])
{
    unsigned char tmp = 0;
    unsigned int i = 0;

    for (i = 0; i < bytelen / 4; i++)
    {
        tmp = des[4 * i];
        des[4 * i] = src[4 * i + 3];
        src[4 * i + 3] = tmp;

        tmp = des[4 * i + 1];
        des[4 * i + 1] = src[4 * i + 2];
        des[4 * i + 2] = tmp;
    }
}


/******************************************************************************
  Function:       SM3_init
  Description:    initiate SM3 state
  Calls:
  Called By:      SM3_256
  Input:          SM3_STATE *md
  Output:         SM3_STATE *md
  Return:         null
  Others:
*******************************************************************************/
void SM3_init(SM3_STATE* md)
{
    //将IV存于md->state中，即V中
    md->curlen = md->length = 0;
    md->state[0] = SM3_IVA;
    md->state[1] = SM3_IVB;
    md->state[2] = SM3_IVC;
    md->state[3] = SM3_IVD;
    md->state[4] = SM3_IVE;
    md->state[5] = SM3_IVF;
    md->state[6] = SM3_IVG;
    md->state[7] = SM3_IVH;
}


/******************************************************************************
  Function:       SM3_compress
  Description:    compress a single block of message
  Calls:          BigEndian
                  BiToW
                  WToW1
                  CF
  Called By:      SM3_256
  Input:          SM3_STATE *md
  Output:         SM3_STATE *md
  Return:         null
  Others:
*******************************************************************************/
void SM3_compress(SM3_STATE* md)
{
    //将消息分组B[i]扩展生成132个消息字W0,W1,...,W67,W'0,W'1,...,W'63
    unsigned int W[68];  //存储W
    unsigned int W1[64]; //存储W'

    //字存储为大端格式，坐标为高有效位，右边为低有效位 
    BigEndian(md->buf, 64, md->buf);

    //消息扩展
    printf("\n\n扩展后的消息:");
    printf("\n\n*******W0,W1,...,W67*******:\n");
    BiToW((unsigned int*)md->buf, W);
    printf("\n\n*******W'0,W'1,...,W'63*******:\n");
    WToW1(W, W1);

    //压缩函数(W,W1,V[i])
    CF(W, W1, md->state);
}

/******************************************************************************
  Function:       SM3_process
  Description:    compress the first (len/64) blocks of message【迭代过程】
  Calls:          SM3_compress
  Called By:      SM3_256
  Input:          SM3_STATE *md
                  unsigned char buf[len]  //the input message
                  int len                 //bytelen of message
  Output:         SM3_STATE *md
  Return:         null
  Others:
*******************************************************************************/
void SM3_process(SM3_STATE* md, unsigned char* buf, int len)
{
    //将填充后的消息m'按512比特进行分组为B0,B1,...,Bn-1
    int n = 1;//消息分组的个数
    while (len--)
    {
        /* copy byte */
        md->buf[md->curlen] = *buf++;
        md->curlen++;
        
        /* is 64 bytes full?  即是否是512比特为一组*/
        if (md->curlen == 64)
        {
            printf("\n\n第%d个消息分组:\n\n",n);
            SM3_compress(md);
            md->length += 512;
            n++;
            md->curlen = 0;
        }
    }
}


/******************************************************************************
  Function:       SM3_done
  Description:    compress the rest message that the SM3_process has left behind
  Calls:          SM3_compress
  Called By:      SM3_256
  Input:          SM3_STATE *md
  Output:         unsigned char *hash
  Return:         null
  Others:
*******************************************************************************/
void SM3_done(SM3_STATE* md, unsigned char hash[])
{
    int i,count=1;
    unsigned char tmp = 0;

    /* increase the bit length of the message 增长1字节的长度*/
    md->length += md->curlen << 3;

    /* append the '1' bit 首先将比特'1'添加到消息的末尾*/
    md->buf[md->curlen] = 0x80;
    md->curlen++;

    /* if the length is currently above 56 bytes, appends zeros till
       it reaches 64 bytes, compress the current block, creat a new
       block by appending zeros and length,and then compress it
       若当前长度超过56字节（512比特），则添加0直到64字节，然后迭代压缩
     */
    if (md->curlen > 56)
    {
        for (; md->curlen < 64;)
        {
            md->buf[md->curlen] = 0;
            md->curlen++;
        }
        //64字节为一组进行迭代压缩
        SM3_compress(md);
        //长度置0，重新创建新块
        md->curlen = 0;
    }

    /* if the length is less than 56 bytes, pad upto 56 bytes of zeroes
       若长度小于56字节，则填充0至56个字节
    */
    for (; md->curlen < 56;)
    {
        md->buf[md->curlen] = 0;
        md->curlen++;
    }

    /* since all messages are under 2^32 bits we mark the top bits zero
       由于所有消息都在2^32位以下，故将最高位标记为0
    */
    for (i = 56; i < 60; i++)
    {
        md->buf[i] = 0;
    }

    /* append length 添加一个64比特串，该比特串是长度为l的二进制表示*/
    md->buf[63] = md->length & 0xff;
    md->buf[62] = (md->length >> 8) & 0xff;
    md->buf[61] = (md->length >> 16) & 0xff;
    md->buf[60] = (md->length >> 24) & 0xff;

    printf("\n*******填充后的消息*******：\n");
    inputChar(md->buf, 64);

    SM3_compress(md);

    /* copy output */
    memcpy(hash, md->state, SM3_len / 8);
    //if CPU uses little-endian, BigEndian function is a necessary call
    BigEndian(hash, SM3_len / 8, hash);   
}


/******************************************************************************
  Function:       SM3_256
  Description:    calculate a hash value from a given message
  Calls:          SM3_init
                  SM3_process
                  SM3_done
  Called By:
  Input:          unsigned char buf[len]  //the input message
                  int len                 //bytelen of the message
  Output:         unsigned char hash[32]
  Return:         null
  Others:
*******************************************************************************/
void SM3_256(unsigned char buf[], int len, unsigned char hash[])
{
    SM3_STATE md;
    SM3_init(&md);
    SM3_process(&md, buf, len);
    SM3_done(&md, hash);
}

/******************************************************************************
  Function:       SM3_SelfTest
  Description:    test whether the SM3 calculation is correct by comparing
                  the hash result with the standard result
  Calls:          SM3_256
  Called By:
  Input:          null
  Output:         null
  Return:         0      //the SM3 operation is correct
                  1      //the sm3 operation is wrong
  Others:
*******************************************************************************/
void SM3_SelfTest()
{
    unsigned int i = 0, a = 1, b = 1;
    int choose;
    printf("请选择输入示例：1.示例1\t\t2.示例2\n");
    scanf("%d", &choose);

    if (choose == 1)
    {
        //示例1
        unsigned char Msg1[3] = { 0x61,0x62,0x63 };// ASCII码
        int MsgLen1 = 3;
        printf("\n*******输入的字符串消息*******：\n");
        inputChar(Msg1, MsgLen1);
        unsigned char MsgHash1[32] = { 0 }; //生成的Hash值
        /*
        unsigned char StdHash1[32] = { 0x66,0xC7,0xF0,0xF4,0x62,0xEE,0xED,0xD9,0xD1,0xF2,0xD4,0x6B,0xDC,0x10,0xE4,0xE2,
            0x41,0x67,0xC4,0x87,0x5C,0xF2,0xF7,0xA2,0x29,0x7D,0xA0,0x2B,0x8F,0x4B,0xA8,0xE0 };//已经计算好的Hash值
        */
        SM3_256(Msg1, MsgLen1, MsgHash1);
        printf("\n*******输出杂凑值*******：\n");
        inputChar(MsgHash1, 32);
    }
    else if (choose == 2)
    {
        //示例2
        unsigned char Msg2[64] = { 0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
            0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
            0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
            0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64 };// ASCII码
        int MsgLen2 = 64;
        printf("\n*******输入的字符串消息*******：\n");
        inputChar(Msg2, MsgLen2);
        unsigned char MsgHash2[32] = { 0 };//生成的Hash值
        /*
        unsigned char StdHash2[32] = { 0xde,0xbe,0x9f,0xf9,0x22,0x75,0xb8,0xa1,0x38,0x60,0x48,0x89,0xc1,0x8e,0x5a,0x4d,
            0x6f,0xdb,0x70,0xe5,0x38,0x7e,0x57,0x65,0x29,0x3d,0xcb,0xa3,0x9c,0x0c,0x57,0x32 };//已经计算好的Hash值
        */
        SM3_256(Msg2, MsgLen2, MsgHash2);
        printf("\n*******输出杂凑值*******：\n");
        inputChar(MsgHash2, 32);
    }
    else
        printf("输入错误，请重新输入！\n");

    //比较是否相同
    /*
    a = memcmp(MsgHash1, StdHash1, SM3_len / 8); 
    b = memcmp(MsgHash2, StdHash2, SM3_len / 8);
  
    if ((a == 0) && (b == 0))
    {
        return 0;
    }
    else
    {
        return 1;
    }
    */
}


int main()
{
    printf("*********************************************************\n");
    printf("*\t\t\tSM3密码杂凑算法\t\t\t*\n");
    printf("*********************************************************\n\n");
    SM3_SelfTest();
    printf("\n");
    system("pause");
    return 0;
}
