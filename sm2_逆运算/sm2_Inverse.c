#include <stdio.h>

/*
有限域上的逆运算
输入：域Fq，Fq中的非零元素g
输出：逆元素 g ^ -1
*/
int bin[16];
int r=-1;
int Inverse(int g,int q,int a)
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
    int c,g,q;
    printf("----------------------------有限域中的逆运算----------------------------\n");
    printf("\n\t\t\t\t求Fq下的g^-1\n\n");
    printf("-------------------------------------------------------------------------\n");
    
    printf("请输入，g,q：");
    scanf("%d,%d,",&g,&q);
    //1、计算c = g ^ q-2
    c = Inverse(g,q,q-2);
    //2、输出c
    printf("\n%d^-1=%d\n\n",g,c);
    
    system("pause");
    return 0;
}