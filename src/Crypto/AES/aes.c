#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "aes.h"

#define MAX_LEN 4096

/*
int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
    AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
    AES_KEY *key);

void AES_encrypt(const unsigned char *in, unsigned char *out,
    const AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out,
    const AES_KEY *key);

void AES_ecb_encrypt(const unsigned char *in, unsigned char *out,
    const AES_KEY *key, const int enc);

*/
#if 0
unsigned char bcd_char(unsigned char data)
{
    switch (data) {
    case 0x0:
        return '0';
    case 0x1:
        return '1';
    case 0x2:
        return '2';
    case 0x3:
        return '3';
    case 0x4:
        return '4';
    case 0x5:
        return '5';
    case 0x6:
        return '6';
    case 0x7:
        return '7';
    case 0x8:
        return '8';
    case 0x9:
        return '9';
    case 0xa:
        return 'A';
    case 0xb:
        return 'B';
    case 0xc:
        return 'C';
    case 0xd:
        return 'D';
    case 0xe:
        return 'E';
    case 0xf:
        return 'F';
    }
}

static void bcd_get(unsigned char data, unsigned char bcd_data[2])
{
    unsigned char split_data = 0;

    split_data = (data & 0xf0) >> 4;
    bcd_data[0] = bcd_char(split_data);
    split_data = data & 0xf;
    bcd_data[1] = bcd_char(split_data);
}
#endif

static unsigned char hex_get(unsigned char data)
{
    if (data >= '0' && data <= '9')
        return data - '0';
    if (data >= 'A' && data <= 'F')
        return data - 'A' + 10;
    return 0;
}

static unsigned char hex_get_from_bcd(unsigned char bcd_data[2])
{
    unsigned char ret;

    ret = hex_get(bcd_data[0]);
    ret = ret << 4;
    ret += hex_get(bcd_data[1]);
    return ret;
}

static int char_get_from_hex(unsigned char *hex, int hex_len, unsigned char *out_buf, int out_buf_len)
{
    int i;

    if ((hex_len * 2 + 1) > out_buf_len) {
        //PRINTF("out len is too small:hex_len:%d, out_buf_len is %d\n", hex_len, out_buf_len);
        return  -1;
    }
    for (i = 0; i < hex_len; i++) {
        //bcd_get(hex[i], &out_buf[i * 2]);
        sprintf(&out_buf[2 * i], "%02X", hex[i]);
    }
    out_buf[hex_len * 2 + 1] = 0;
    //PRINTF("out buf is %s\n ", out_buf);
    return 0;
}

static int hex_get_from_char(unsigned char *in, int in_len, unsigned char *hex_out_buf, int hex_out_buf_len)
{
    int i;

    if (in_len / 2 > hex_out_buf_len) {
        //PRINTF("hex_out_buf_len is small; in_len :%d, hex_out_buf_len :%d\n", in_len, hex_out_buf_len);
        return -1;
    }
    for (i = 0; i < in_len / 2; i ++) {
        hex_out_buf[i] = hex_get_from_bcd(&in[i * 2]);
    }
    return 0;
}

static int pkcs5_padding(const char *plain_data, int plain_data_len, char *padded_data, int padded_data_len, int block_size)
{
    int pad_len = plain_data_len % block_size;

    if (plain_data_len > padded_data_len) {
        //PRINTF("error to padding:%d:%d\n", plain_data_len, padded_data_len);
        return -1;
    }
    memcpy(padded_data, plain_data, plain_data_len);
    if (pad_len) {
        pad_len = block_size - pad_len;
        int i;
        for (i = 0; i < pad_len; i ++) {
            padded_data[plain_data_len + i] = pad_len;
        }
    }
    return pad_len + plain_data_len;
}

static int pkcs5_padding_remove(char *plain_data, int plain_data_len)//这里要保证plain_data有结束位存在，即plain_data得buf长度=plain_data_len + 1;否则会越界。
{
    if (plain_data[plain_data_len - 1] <= 8 && plain_data[plain_data_len - 1] >= 0) {
        int padding_value = plain_data[plain_data_len - 1];
        while (padding_value > 0) {
            if (plain_data[plain_data_len - padding_value] != plain_data[plain_data_len - 1])
                return -1;
            padding_value --;
        }
        plain_data[plain_data_len - plain_data[plain_data_len - 1]] = 0;
        return 0;
    } else
        plain_data[plain_data_len] = 0;
    return -1;
}

int yx_aes_ecb_encrypt(char *plain_txt, int plain_txt_len, char *key, char *encrypted_txt, int *encrypted_txt_len)
{
    int encrypt_len = 0;
    int inc_len = 0;
    char input[MAX_LEN] = {0};
    char output[MAX_LEN] = {0};
    AES_KEY aes_ks;
    int mem_len = plain_txt_len + 9; //padding最多会补8位

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    AES_set_encrypt_key(key, 128, &aes_ks);
    if (plain_txt_len > MAX_LEN) {
        //PRINTF("error :plain_txt_len :%d is  too longer for buf:%d\n", plain_txt_len, MAX_LEN);
        return -1;
    }
    encrypt_len = pkcs5_padding(plain_txt, plain_txt_len, input, mem_len, 16);
    if (*encrypted_txt_len < (encrypt_len * 2 + 1)) {
        //PRINTF("encrypted_txt_len:%d is less than  plain_txt_len * 2 + 1 :%d\n", *encrypted_txt_len, (encrypt_len * 2 + 1));
        return -1;
    }
    inc_len = 0;
    while (inc_len < encrypt_len) {
        AES_encrypt(&input[inc_len], &output[inc_len], &aes_ks);
        inc_len += 16;
    }
    char_get_from_hex(output, encrypt_len, encrypted_txt, *encrypted_txt_len);
    *encrypted_txt_len = encrypt_len * 2;
    return 0;
}

int yx_aes_ecb_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *plain_txt, int *plain_txt_len)
{
    int encrypt_len = 0;
    int inc_len = 0;
    AES_KEY aes_ks;
    char hex[MAX_LEN] = {0};

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    if (*plain_txt_len < encrypted_txt_len / 2) {
        //PRINTF("plain_txt_len :%d is less than encrypted_txt_len / 2:%d\n", *plain_txt_len, encrypted_txt_len / 2);
        return -1;
    }
    if (encrypted_txt_len / 2 > MAX_LEN) {
        //PRINTF("error :encrypted_txt_len :%d is  too longer for buf:%d\n", encrypted_txt_len, MAX_LEN);
        return -1;
    }
    hex_get_from_char(encrypted_txt, encrypted_txt_len, hex, encrypted_txt_len / 2);
    //PRINTF("pass is %s\n", output_bcd);
    AES_set_decrypt_key(key, 128, &aes_ks);
    inc_len = 0;
    encrypt_len = encrypted_txt_len / 2;
    while (inc_len < encrypt_len) {
        AES_decrypt(&hex[inc_len], &plain_txt[inc_len], &aes_ks);
        inc_len += 16;
    }
    pkcs5_padding_remove(plain_txt, encrypt_len);
    return 0;
}


int yx_aes_cbc_encrypt(char *plain_txt, int plain_txt_len, char *key, char *vector, char *encrypted_txt, int *encrypted_txt_len)
{
    AES_KEY aes_ks;
    int encrypt_len = 0;
    char input[MAX_LEN + 1] = {0};
    int tmp_len;
    char output[MAX_LEN + 1] = {0};

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    assert(vector != NULL);
    tmp_len = plain_txt_len + 9;
    AES_set_encrypt_key(key, 128, &aes_ks);
    if (plain_txt_len > MAX_LEN) {
        //PRINTF("error :plain_txt_len :%d is  too longer for buf:%d\n", plain_txt_len, MAX_LEN);
        return -1;
    }
    encrypt_len = pkcs5_padding(plain_txt, plain_txt_len, input, tmp_len, 16);
    if (*encrypted_txt_len < (encrypt_len * 2  + 1)) {
        //PRINTF("encrypted_txt_len:%d is less than  plain_txt_len * 2 + 1 :%d\n", *encrypted_txt_len, (encrypt_len * 2 + 1));
        return -1;
    }
    AES_cbc_encrypt(input, output, encrypt_len, &aes_ks, vector , 1);
    char_get_from_hex(output, encrypt_len, encrypted_txt, *encrypted_txt_len);
    *encrypted_txt_len = encrypt_len * 2;
    return 0;
}

int yx_aes_cbc_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *vector, char *plain_txt, int *plain_txt_len)
{
    AES_KEY aes_ks;
    char hex[MAX_LEN] = {0};

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    if (*plain_txt_len < encrypted_txt_len / 2) {
        //PRINTF("plain_txt_len :%d is less than encrypted_txt_len / 2:%d\n", *plain_txt_len, encrypted_txt_len / 2);
        return -1;
    }
    if (encrypted_txt_len / 2 > MAX_LEN) {
        //PRINTF("error :encrypted_txt_len :%d is  too longer for buf:%d\n", encrypted_txt_len, MAX_LEN);
        return -1;
    }
    hex_get_from_char(encrypted_txt, encrypted_txt_len, hex, encrypted_txt_len / 2);
    AES_set_decrypt_key(key, 128, &aes_ks);
    AES_cbc_encrypt(hex, plain_txt, *plain_txt_len, &aes_ks, vector , 0);
    pkcs5_padding_remove(plain_txt, encrypted_txt_len / 2);
    return 0;
}


/*AES ECB128 PKCS5Padding zm add*/
int app_aes_ecb_encrypt(char *plain_txt, int plain_txt_len, char *key, char *encrypted_txt, int *encrypted_txt_len)
{
    int encrypt_len = 0;
    int inc_len = 0;
    char input[MAX_LEN] = {0};
    char output[MAX_LEN] = {0};
    AES_KEY aes_ks;
    int mem_len = plain_txt_len + 9; //padding最多会补8位

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    AES_set_encrypt_key(key, 128, &aes_ks);
    if (plain_txt_len > MAX_LEN) {
        printf("app_aes_ecb_encrypt error :plain_txt_len :%d is  too longer for buf:%d\n", plain_txt_len, MAX_LEN);
        return -1;
    }
    encrypt_len = pkcs5_padding(plain_txt, plain_txt_len, input, mem_len, 16);
    if (*encrypted_txt_len < (encrypt_len  + 1) || encrypt_len < 0) {
        printf("error!encrypt_len < 0 or app_aes_ecb_encrypt encrypted_txt_len:%d is less than  plain_txt_len * 2 + 1 :%d\n", *encrypted_txt_len, (encrypt_len  + 1));
        return -1;
    }
    inc_len = 0;
    while (inc_len < encrypt_len) {
        AES_encrypt(&input[inc_len], &output[inc_len], &aes_ks);
        inc_len += 16;
    }

    memcpy(encrypted_txt, output, encrypt_len);
    *encrypted_txt_len = encrypt_len ;

    return 0;
}

/*AES ECB128 PKCS5Padding zm add*/
int app_aes_ecb_decrypt(char *encrypted_txt, int encrypted_txt_len, char *key, char *plain_txt, int *plain_txt_len)
{
    int encrypt_len = 0;
    int inc_len = 0;
    AES_KEY aes_ks;

    assert(plain_txt != NULL);
    assert(key != NULL);
    assert(encrypted_txt != NULL);
    if (*plain_txt_len <= encrypted_txt_len) {
        printf("app_aes_ecb_decrypt plain_txt_len :%d is less than encrypted_txt_len :%d\n", *plain_txt_len, encrypted_txt_len);
        return -1;
    }

    AES_set_decrypt_key(key, 128, &aes_ks);
    inc_len = 0;
    encrypt_len = encrypted_txt_len ;
    while (inc_len < encrypt_len) {
        AES_decrypt(&encrypted_txt[inc_len], &plain_txt[inc_len], &aes_ks);
        inc_len += 16;
    }
    pkcs5_padding_remove(plain_txt, encrypt_len);
    return 0;
}
