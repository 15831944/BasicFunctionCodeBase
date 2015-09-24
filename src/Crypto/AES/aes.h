
#ifndef _AES_H
#define _AES_H

#define IN 
#define OUT 

//#include <openssl/opensslconf.h>

#ifdef OPENSSL_NO_AES
#error AES is disabled.
#endif

#define AES_ENCRYPT	1
#define AES_DECRYPT	0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

#ifdef OPENSSL_FIPS
#define FIPS_AES_SIZE_T	int
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* This should be a hidden type, but EVP requires that the size be known */
typedef struct aes_key_st {
#ifdef AES_LONG
    unsigned long rd_key[4 *(AES_MAXNR + 1)];
#else
    unsigned int rd_key[4 *(AES_MAXNR + 1)];
#endif
    int rounds;
} AES_KEY;

const char *AES_options(void);

int AES_set_encrypt_key(IN const unsigned char *userKey, IN const int bits,
	OUT AES_KEY *key);
int AES_set_decrypt_key(IN const unsigned char *userKey, IN const int bits,
	OUT AES_KEY *key);

void AES_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const AES_KEY *key);
void AES_decrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const AES_KEY *key);

	
	
void AES_ecb_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const AES_KEY *key, const int enc);
void AES_cbc_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	OUT unsigned char *ivec, IN const int enc);
	
	
	
void AES_cfb128_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	unsigned char *ivec, int *num, const int enc);
void AES_cfb1_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	unsigned char *ivec, int *num, IN const int enc);
void AES_cfb8_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	unsigned char *ivec, int *num, IN const int enc);
void AES_cfbr_encrypt_block(IN const unsigned char *in, OUT unsigned char *out,
	IN const int nbits, IN const AES_KEY *key, 
	OUT unsigned char *ivec, IN const int enc);
void AES_ofb128_encrypt(IN const unsigned char *in, OUT unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	OUT unsigned char *ivec, int *num);
void AES_ctr128_encrypt(IN const unsigned char *in, unsigned char *out,
	IN const unsigned long length, IN const AES_KEY *key,
	OUT unsigned char ivec[AES_BLOCK_SIZE],
	OUT unsigned char ecount_buf[AES_BLOCK_SIZE],
	OUT unsigned int *num);

/* For IGE, see also http://www.links.org/files/openssl-ige.pdf */
/* NB: the IV is _two_ blocks long */
void AES_ige_encrypt(const unsigned char *in, unsigned char *out,
		     const unsigned long length, const AES_KEY *key,
		     unsigned char *ivec, const int enc);
/* NB: the IV is _four_ blocks long */
void AES_bi_ige_encrypt(const unsigned char *in, unsigned char *out,
			const unsigned long length, const AES_KEY *key,
			const AES_KEY *key2, const unsigned char *ivec,
			const int enc);

int AES_wrap_key(AES_KEY *key, const unsigned char *iv,
		unsigned char *out,
		const unsigned char *in, unsigned int inlen);
int AES_unwrap_key(AES_KEY *key, const unsigned char *iv,
		unsigned char *out,
		const unsigned char *in, unsigned int inlen);



static void bcd_get(unsigned char data, unsigned char bcd_data[2]);		
static unsigned char hex_get(unsigned char data);
static unsigned char hex_get_from_bcd(unsigned char bcd_data[2]);

static int char_get_from_hex(unsigned char *hex, int hex_len, unsigned char *out_buf, int out_buf_len);
static int hex_get_from_char(unsigned char *in, int in_len, unsigned char *hex_out_buf, int hex_out_buf_len);

static int pkcs5_padding(const char *plain_data, int plain_data_len, char *padded_data, int padded_data_len, int block_size);
static int pkcs5_padding_remove(char *plain_data, int plain_data_len);//这里要保证plain_data有结束位存在，即plain_data得buf长度=plain_data_len + 1;否则会越界。

int yx_aes_ecb_encrypt(char *plain_txt, int plain_txt_len, char *key, char *encrypted_txt, int * encrypted_txt_len);
int yx_aes_ecb_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *plain_txt, int *plain_txt_len);

int yx_aes_cbc_encrypt(char *plain_txt, int plain_txt_len, char *key, char *vector, char *encrypted_txt, int * encrypted_txt_len);
int yx_aes_cbc_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *vector, char *plain_txt, int *plain_txt_len);

int app_aes_ecb_encrypt(char *plain_txt, int plain_txt_len, char *key, char *encrypted_txt, int * encrypted_txt_len);
int app_aes_ecb_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *plain_txt, int *plain_txt_len );




#ifdef  __cplusplus
}
#endif

#endif /* !HEADER_AES_H */
