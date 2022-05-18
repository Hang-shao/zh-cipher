#include <iostream>
#include "sm3/sm3.h"
int main()
{
    SM3_CTX SMC;
    ossl_sm3_init(&SMC);

    const unsigned char Data[1024] = "Hello World";
    unsigned char md[SM3_DIGEST_LENGTH] = { 0 };

    printf("input message:%s\n",Data);
    ossl_sm3_update(&SMC, Data, strlen((const char *) Data));
    ossl_sm3_final(md, &SMC);
    printf("output hash value:");
    for (int i = 0; i < SM3_DIGEST_LENGTH; i++) {
        printf("%02x ", *(md + i));
    }
}
