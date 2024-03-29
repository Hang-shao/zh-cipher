//加密用头文件
#ifndef XYSSL_SM4_H
#define XYSSL_SM4_H
 
#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
 

typedef struct
{
    int mode;                   /*!<  encrypt/decrypt   */
    unsigned long sk[32];       /*!<  SM4 subkeys       */
}
sm4_context;
 
 
#ifdef __cplusplus
extern "C" {
#endif
 

void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] );
 

void sm4_setkey_dec( sm4_context *ctx, unsigned char key[16] );
 

void sm4_crypt_ecb( sm4_context *ctx,
				     int mode,
					 int length,
                     unsigned char *input,
                     unsigned char *output);
 

void sm4_crypt_cbc( sm4_context *ctx,
                     int mode,
                     int length,
                     unsigned char iv[16],
                     unsigned char *input,
                     unsigned char *output );
/// <summary>
/// SGIM4 中使用的替换AEGISR函数的东西
/// </summary>
/// <param name="input">输入</param>
/// <param name="output">输出</param>
void sm4_round_without_rk(unsigned char input[16], unsigned char output[16]);

/// <summary>
///  sgim4 中使用的R函数	
/// </summary>
/// <param name="input">输入</param>
/// <param name="key">经过R函数与输入做异或运算</param>
/// <param name="output">输出</param>
void sgim4_round(unsigned char* input, unsigned char* key, unsigned char* output);
#ifdef __cplusplus
}
#endif
 
#endif 
