#include <stdio.h>

int main()
{
	SM9_SelfCheck();
	system("pause");
	return 0;
}




































/*调试说明：
1.原代码包中SM9_enc_dec.c文件中的SM9_SelfCheck()承担主函数功能，包含了完整的加解密流程，其它函数为功能函数。
2.此代码需要miracl库的支持，除加入miracl.h和mirdef.h头文件，miracl.lib文件外，需要新添加mrmuldv.c和mrzzn4.c两个源文件。否则会提示未定义的引用。
3.在原代码的基础上，新增了一些说明性内容，将原来的一些英文改为中文显示。
*/