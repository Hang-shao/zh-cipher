#include <stdio.h>
/*
有限域上的指数运算
输入：正整数a，域Fq，域元素g
输出：g^a
*/
int bin[16];
int r=-1;
//输出e的二进制表示：将e的二进制存于数组bin中，最低位存在数组的第一个元素
void print_bin(int e)
{
    int x;
    while(e != 0)
    {
        x = e % 2;
        r++;
        bin[r] = x;
        e /= 2;
    }
}
int main()
{
    printf("----------------------------有限域中的指数运算----------------------------\n");
    printf("\n\t\t\t\t求Fq下的g^a\n\n");
    printf("--------------------------------------------------------------------------\n");
    int a,q,g,x,e,i,tmp,result;
    
    printf("请输入a,q,g：");
    scanf("%d,%d,%d",&g,&q,&a);
    
    //1、置e = a % (q-1)，若e = 0，则输出1
    e = a % (q-1);
    if(e == 0)
    {
        return 1;
    }
    //2、设e的二进制表示是e=Er Er-1 ...... E0，最高位Er=1
    print_bin(e);

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
    //5、输出x
    printf("\n%d^%d mod %d=%d\n",g,a,q,x);
    system("pause");
    return 0;
}