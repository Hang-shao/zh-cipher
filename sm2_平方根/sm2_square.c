#include <stdio.h>

/*
    模素数平方根求解
        y^2=g(mod p)
    输入：奇素数p，整数g，0<g<p
    输出：若存在g的平方根，则输出一个平方根mod p，否则输出“不存在平方根”
*/

int bin[16];
int r=-1;
int index(int g,int q,int a)
{
    int x,e,i,tmp;
    //1、置e = a % (q-1)，若e = 0，则输出1
    e = a % (q-1);
    if(e == 0)
    {
        return 1;
    }
    //2、设e的二进制表示是e=Er Er-1 ...... E0，最高位Er=1
    while(e != 0)
    {
        x = e % 2;
        r++;
        bin[r] = x;
        e /= 2;
    }

    //3、置x=g
    x=g;
    //4、对i从r-1下降到0执行：
    //  4.1 置x=x^2
    //  4.2 若Ei=1，则置x=g.x
    for(i=r-1;i>=0;i--)
    {
        x = (x * x) % q;
        if(bin[i] == 1)
        {
            x = (g * x) % q;
        }
    }
    return x;
}

int main()
{
    int g,p,y,u,z;
    printf("----------------------------模素数平方根求解----------------------------\n");
    printf("\n\t\t\ty^2=g(mod p)，求y\n\n");
    printf("------------------------------------------------------------------------\n");
    
    printf("请输入，g,p：");
    scanf("%d,%d",&g,&p);
   /*
    若g=0，则只有一个平方根，即y=0
     若g≠0，则有0个或者2个平方根，若y是一个平方根，那么另外一个就是 p-y
    */
    if(g == 0)
    {
        y=0;
        printf("y=%d\n",y);
    }else
    {
        /*
        若p=3(mod4)，则存在正整数u，使得p=4u+3
        1、y=g^(u+1) mod p
        2、z=y^2 mod p
        3、若z=g，则输出y，否则不存在平方根
        */
        if(p % 4 == 3)
        {
            u = (p-3)/4;
            y = index(g,p,u+1);
            z = index(y,p,2);
            if(z == g)
            {
                printf("y1=%d,y2=%d",y,p-y);
            }else
            {
                printf("不存在平方根！\n");
            }
        }
         /*
        若p=5(mod8)，则存在正整数u，使得p=8u+5
        1、z=g^(2u+1) mod p
        2、若z=1(mod p)，计算y=g^(u+1) mod p，输出y，终止算法
        3、若z=-1(mod p)，计算y=(2g*(4g)^u) mod p，输出y，终止算法
        4、输出不存在平方根
        */
        else if(p % 8 == 5)
        {
            u = (p-5)/8;
            z = index(g,p,2*u+1);
            if(z == 1)
            {
                y = index(g,p,u+1);
                printf("y1=%d,y2=%d",y,p-y);
            }
            if(z == -1)
            {
                y = (2*g*index(4*g,p,u)) % p;
                printf("y1=%d,y2=%d",y,p-y);
            }
            printf("不存在平方根！\n");
        }
        else
        {
           printf("请输入合适的参数！\n");
        }  
    }
    
    system("pause");
    return 0;
}