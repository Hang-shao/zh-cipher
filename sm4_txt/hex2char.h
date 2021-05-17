#include <string.h>

unsigned char getByte(char a, char b){
	if(a >= '0' && a <= '9'){
		a-='0';
	}else if(a >= 'A' && a <= 'Z'){
		a= a-'A'+10;
	}else if(a >= 'a' && a <= 'z'){
		a= a-'a'+10;
	}

	if(b >= '0' && b <= '9'){
		b-='0';
	}else if(b >= 'A' && b <= 'Z'){
		b= b-'A'+10;
	}else if(b >= 'a' && b <= 'z'){
		b= b-'a'+10;
	}
	return a*16+b;
}

void charToByte(char* src, unsigned char* byte){
	int length = strlen(src);
	int i;
	char a,b;
	for(i = 0; i < length; i++){
		a = src[i];
		i = i+1;
		b = src[i];
		byte[i/2] = getByte(a,b);
	}
}