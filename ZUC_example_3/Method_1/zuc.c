#include "zuc.h"

unsigned int AddMod(unsigned int a, unsigned int b)
{
    unsigned int c = a + b;
    if (c >> 31)
    {
        c = (c & 0x7fffffff) + 1;
    }
    return c;
}


unsigned int PowMod(unsigned int x, unsigned int k)
{
    return (((x << k) | (x >> (31 - k))) & 0x7fffffff);
}


unsigned int L1(unsigned int X)
{
    return X ^ ZUC_rotl32(X, 2) ^ ZUC_rotl32(X, 10) ^ ZUC_rotl32(X, 18) ^ ZUC_rotl32(X, 24);
}
unsigned int L2(unsigned int X)
{
    return X ^ ZUC_rotl32(X, 8) ^ ZUC_rotl32(X, 14) ^ ZUC_rotl32(X, 22) ^ ZUC_rotl32(X, 30);
}

unsigned char BitValue(unsigned int M[], unsigned int i)
{
    int j, k;
    j = i >> 5;
    k = i & 0x1f;
    if (M[j] & (0x1 << (31 - k)))
        return 1;
    else
        return 0;
}

unsigned int GetWord(unsigned int k[], unsigned int i)             //获取字串中的从第i个比特值开始的字
{
    int j, m;
    unsigned int word;
    j = i >> 5;
    m = i & 0x1f;
    if (m == 0)
        word = k[j];
    else
        word = (k[j] << m) | (k[j + 1] >> (32 - m));
    return word;
}
void LFSRWithInitMode(unsigned int LFSR_S[], unsigned int u)
{
    unsigned int v = LFSR_S[0], i;
    v = AddMod(v, PowMod(LFSR_S[15], 15));
    v = AddMod(v, PowMod(LFSR_S[13], 17));
    v = AddMod(v, PowMod(LFSR_S[10], 21));
    v = AddMod(v, PowMod(LFSR_S[4], 20));
    v = AddMod(v, PowMod(LFSR_S[0], 8));

    for (i = 0; i < 15; i++)
    {
        LFSR_S[i] = LFSR_S[i + 1];
    }
    LFSR_S[15] = AddMod(v, u);

    if (!LFSR_S[15])
    {
        LFSR_S[15] = 0x7fffffff;
    }
}

void LFSRWithWorkMode(unsigned int LFSR_S[])
{
    unsigned int v = LFSR_S[0], i;
    v = AddMod(v, PowMod(LFSR_S[15], 15));
    v = AddMod(v, PowMod(LFSR_S[13], 17));
    v = AddMod(v, PowMod(LFSR_S[10], 21));
    v = AddMod(v, PowMod(LFSR_S[4], 20));
    v = AddMod(v, PowMod(LFSR_S[0], 8));

    for (i = 0; i < 15; i++)
    {
        LFSR_S[i] = LFSR_S[i + 1];
    }
    LFSR_S[15] = v;

    if (!LFSR_S[15])
    {
        LFSR_S[15] = 0x7fffffff;
    }
}

void BR(unsigned int LFSR_S[], unsigned int BR_X[])
{
    BR_X[0] = ((LFSR_S[15] & 0x7fff8000) << 1) | (LFSR_S[14] & 0x0000ffff);
    BR_X[1] = ((LFSR_S[11] & 0x0000ffff) << 16) | ((LFSR_S[9] & 0x7fff8000) >> 15);
    BR_X[2] = ((LFSR_S[7] & 0x0000ffff) << 16) | ((LFSR_S[5] & 0x7fff8000) >> 15);
    BR_X[3] = ((LFSR_S[2] & 0x0000ffff) << 16) | ((LFSR_S[0] & 0x7fff8000) >> 15);
}

unsigned int F(unsigned int BR_X[], unsigned int F_R[])
{
    unsigned int W, W1, W2;

    W = (BR_X[0] ^ F_R[0]) + F_R[1];
    W1 = F_R[0] + BR_X[1];
    W2 = F_R[1] ^ BR_X[2];
    F_R[0] = L1((W1 << 16) | (W2 >> 16));
    F_R[0] = (ZUC_S0[(F_R[0] >> 24) & 0xFF]) << 24 | (ZUC_S1[(F_R[0] >> 16) & 0xFF]) << 16 | (ZUC_S0[(F_R[0] >> 8) & 0xFF]) << 8 | (ZUC_S1[F_R[0] & 0xFF]);
    F_R[1] = L2((W2 << 16) | (W1 >> 16));
    F_R[1] = (ZUC_S0[(F_R[1] >> 24) & 0xFF]) << 24 | (ZUC_S1[(F_R[1] >> 16) & 0xFF]) << 16  | (ZUC_S0[(F_R[1] >> 8) & 0xFF]) << 8 | (ZUC_S1[F_R[1] & 0xFF]);

    return W;
}

void ZUC_Init(unsigned int k[], unsigned int iv[], unsigned int LFSR_S[], unsigned int BR_X[], unsigned int F_R[])
{
    unsigned char count = 32;
    int i;

    //loading key to the LFSR s0,s1,s2....s15 
    printf("\ninitial state of LFSR: S[0]-S[15]\n");
    for (i = 0; i < 16; i++)
    {
        LFSR_S[i] = ZUC_LinkToS(k[i], ZUC_d[i], iv[i]);
        printf("%08x  ", LFSR_S[i]);
    }

    F_R[0] = 0x00;    //R1 
    F_R[1] = 0x00;    //R2 

    // 初始变量和秘钥状态完毕后，开始32轮初始化
    while (count)         //32 times 
    {
        unsigned int W;
        BR(LFSR_S, BR_X); //BitReconstruction 
        W = F(BR_X, F_R);   //nonlinear function 
        LFSRWithInitMode(LFSR_S, W >> 1);
        count--;
    }
}

void ZUC_Work(unsigned int LFSR_S[], unsigned int BR_X[], unsigned int F_R[], unsigned int pKeyStream[], int KeyStreamLen)
{
    int i = 0;
    BR(LFSR_S, BR_X);
    F(BR_X, F_R);
    LFSRWithWorkMode(LFSR_S);
    printf("------------------------------------------------------------\n输出密钥流：\n");
    while (i < KeyStreamLen)
    {
        BR(LFSR_S, BR_X);
        pKeyStream[i] = F(BR_X, F_R) ^ BR_X[3];
        printf("pKeyStream[%d]=%08x\n", i,pKeyStream[i]);
        LFSRWithWorkMode(LFSR_S);
        i++;
    }
}

void ZUC_GenKeyStream(unsigned int k[], unsigned int iv[], unsigned int KeyStream[], int KeyStreamLen)
{

    unsigned int LFSR_S[16]; //LFSR state s0,s1,s2,...s15 
    unsigned int BR_X[4];    //Bit Reconstruction X0,X1,X2,X3 
    unsigned int F_R[2];     //R1,R2,variables of nonlinear function F 
    int i;

    //Initialisation 
    ZUC_Init(k, iv, LFSR_S, BR_X, F_R);
    printf("\n------------------------------------------------------------\nstate of LFSR after executing initialization: S[0]-S[15]\n");
    for (i = 0; i < 16; i++)
    {
        printf("%08x  ", LFSR_S[i]);
    }
    printf("\n------------------------------------------------------------\ninternal state of Finite State Machine:\n");
    printf("R1=%08x\n", F_R[0]);
    printf("R2=%08x\n", F_R[1]);

    //Working 
    ZUC_Work(LFSR_S, BR_X, F_R, KeyStream, KeyStreamLen);
}

int main()
{
    int KeystreamLen, i;
    unsigned int key[16] = {0};
    unsigned int iv[16] = {0};

    char k[50] = { 0 };
    char v[50] = { 0 };

    printf("key: ");
    scanf("%s", k);
    printf("IV : ");
    scanf("%s", v);
    printf("KeystreamLen : ");
    scanf("%d", &KeystreamLen);
    for (i = 0; i < 16; i++)
    {
        key[i] = (((k[2 * i] <= '9') ? (k[2 * i] - '0') : (k[2 * i] - 'a' + 10)) << 4) +
            ((k[2 * i + 1] <= '9') ? (k[2 * i + 1] - '0') : (k[2 * i + 1] - 'a' + 10));
        iv[i] = (((v[2 * i] <= '9') ? (v[2 * i] - '0') : (v[2 * i] - 'a' + 10)) << 4) +
            ((v[2 * i + 1] <= '9') ? (v[2 * i + 1] - '0') : (v[2 * i + 1] - 'a' + 10));
    }
   
    printf("\n开始生成秘钥流：\n------------------------------------------------------------");
    ZUC_GenKeyStream(key, iv,Keystream, KeystreamLen);
}
