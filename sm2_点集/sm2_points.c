#include <stdio.h>

/*
    求Fp上的椭圆曲线的点集
    椭圆曲线：y^2=x^3+ax+b
    输入：素数p，参数a，b
    输出：(x,y)
*/

int flag; //flag=0，表示不存在平方根；flag=1，表示y=0；flag=2,表示有两个平方根
int bin[16];
int r = -1;
//输出e的二进制表示：将e的二进制存于数组bin中，最低位存在数组的第一个元素
void print_bin(int e)
{
    int x;
    while (e != 0)
    {
        x = e % 2;
        r++;
        bin[r] = x;
        e /= 2;
    }
}
int index(int g, int q, int a)
{
    int x, e, i, tmp;
    //1、置e = a % (q-1)，若e = 0，则输出1
    e = a % (q - 1);
    if (e == 0)
    {
        return 1;
    }
    //2、设e的二进制表示是e=Er Er-1 ...... E0，最高位Er=1
    print_bin(e);

    //3、置x=g
    x = g;
    //4、对i从r-1下降到0执行：
    //  4.1 置x=x^2
    //  4.2 若Ei=1，则置x=g.x
    for (i = r - 1; i >= 0; i--)
    {
        x = (x * x) % q;
        if (bin[i] == 1)
        {
            x = (g * x) % q;
        }
    }
    return x;
}

int square(int g, int p)
{
    /*
     若g=0，则只有一个平方根，即y=0
      若g≠0，则有0个或者2个平方根，若y是一个平方根，那么另外一个就是 p-y
     */
    int u, z, y;
    if (g == 0)
    {
        y = 0;
        flag = 1;
        return y;
    }
    else
    {
        /*
        若p=3(mod4)，则存在正整数u，使得p=4u+3
        1、y=g^(u+1) mod p
        2、z=y^2 mod p
        3、若z=g，则输出y，否则不存在平方根
        */
        if (p % 4 == 3)
        {
            u = (p - 3) / 4;
            y = index(g, p, u + 1);
            r = -1;
            z = index(y, p, 2);
            r = -1;
            if (z == g)
            {
                flag = 2;
                return y;
            }
            else
            {
                flag = 0;
                //printf("不存在平方根！\n");
            }
        }
        /*
       若p=5(mod8)，则存在正整数u，使得p=8u+5
       1、z=g^(2u+1) mod p
       2、若z=1(mod p)，计算y=g^(u+1) mod p，输出y，终止算法
       3、若z=-1(mod p)，计算y=(2g*(4g)^u) mod p，输出y，终止算法
       4、输出不存在平方根
       */
        else if (p % 8 == 5)
        {
            u = (p - 5) / 8;
            z = index(g, p, 2 * u + 1);
            r = -1;
            if (z == 1)
            {
                flag = 2;
                y = index(g, p, u + 1);
                return y;
            }
            if (z == -1)
            {
                y = (2 * g * index(4 * g, p, u)) % p;
                return y;
                flag = 2;
            }
            flag = 0;
            //printf("不存在平方根！\n");
        }
        else
        {
            printf("请输入合适的参数！\n");
        }
    }
    return -1;
}

int main()
{
    int a, b, p, y, x, A, count = 0; //count记录点的数量
    printf("----------------------------求Fp上的椭圆曲线的点集----------------------------\n");
    printf("\n\t\t\ty^2=x^3+ax+b，求(x,y)\n\n");
    printf("------------------------------------------------------------------------\n");

    printf("请依次输入，a b p：");
    scanf("%d %d %d", &a, &b, &p);

    //1、选取x，0≤x<p
    for (x = 0; x < p; x++)
    {
        A = (x * x * x + a * x + b) % p;
        if (A == 0)
        {
            printf("(%d,0)\t", x);
            count++;
            continue;
        }
        y = square(A, p);
        //只有一个根，y=0
        if (flag == 1)
        {
            printf("(%d,%d)\t", x, y);
            count++;
        }
        else if (flag == 2)
        {
            printf("(%d,%d)\t", x, y);
            count++;
            printf("(%d,%d)\t", x, p - y);
            count++;
        }
        else
        {
            continue;
        }
    }
    printf("\n该椭圆曲线的点数为(包括无穷远点)：%d\n",count+1);

    system("pause");
    return 0;
}