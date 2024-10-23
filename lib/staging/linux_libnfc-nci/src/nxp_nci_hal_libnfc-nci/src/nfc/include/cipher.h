/******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/


/******************************************************************************
 *
 *  This file contains the CIPHER API definitions
 *
 ******************************************************************************/
#ifndef CIPHER_H
#define CIPHER_H

#if(NFC_NXP_LLCP_SECURED_P2P == TRUE)
#include <openssl/sha.h>
#include <openssl/cmac.h>
#include <openssl/bn.h>
//#include <openssl/ec_lcl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "data_types.h"
#include <openssl/ec.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CCM_TAG_LEN 4
#define CCM_NONCE_LEN 13
#define KEY_LEN 32
#define RN_LEN 8

#define ECDH_CURVE    NID_X9_62_prime256v1
static const char rnd_seed[] = "string to make the random number generator think it has entropy";
static const int KDF1_SHA1_len = 32;
static const int aes_cmac_len = 16;

typedef struct {
    UINT8 tag[CCM_TAG_LEN];
    UINT16 tag_len;
    UINT8 ccmNonce[CCM_NONCE_LEN];
    UINT16 ccmNonce_len;

    UINT8 privKey[KEY_LEN];
    UINT8 pubKey_local_x_a[KEY_LEN];
    UINT8 pubKey_local_y_a[KEY_LEN];
    UINT8 pubKey_remote_x_a[KEY_LEN];
    UINT8 pubKey_remote_y_a[KEY_LEN];
    UINT16 key_len;

    UINT8 randomNonce_local[RN_LEN];
    UINT8 randomNonce_remote[RN_LEN];
    UINT8 randomNonce[RN_LEN*2];
    UINT16 rn_len;

    UINT8* sharedSecretKey[KEY_LEN];
    UINT16 sharedSecretKey_len;

    UINT8* finalKey[KEY_LEN];
    UINT16 finalKey_len;

    UINT32 packet_counter_send;
    UINT32 packet_counter_recv;

} tCIPHER_SUITE;
tCIPHER_SUITE cipher_suite;

typedef struct {
    EC_KEY* privKey_local;
    EC_KEY* privKey_remote;
    EC_POINT* pubKey_local;
    EC_POINT* pubKey_remote;
    BIGNUM* x_a_local;
    BIGNUM* y_a_local;
    BIGNUM* x_a_remote;
    BIGNUM* y_a_remote;
    const EC_GROUP* group_local;
    const EC_GROUP* group_remote;
    BN_CTX* ctx;
    CMAC_CTX* cctx;
    EVP_CIPHER_CTX* ccmctx;
}tECDH_KEY;
tECDH_KEY ecdh_key;

void cipher_init(void);
void cipher_deinit(void);
void cipher_generate_keys(void);
void cipher_generate_final_key_kenc(void);

void ecdh_init(void);
void ecdh_deinit(void);
void ecdh_get_localkeys(void);
void ecdh_get_randomnonce(UINT8 *randomNum);
void ecdh_get_remotekeys(void);
int ecdh_compute_sharedkey(EC_POINT *remotEcPnt,EC_KEY *loclecKey);
void kenc_compute_cipherkey();

void aes_ccm_encrypt_data(UINT8*pInBuff,UINT16 BuffLen, UINT8 *aad, UINT16 aad_len,UINT16 *pOutBuff);
void aes_ccm_decrypt_data(UINT8*pInBuff,UINT16 BuffLen, UINT8 *aad, UINT16 aad_len,UINT16 *pOutBuff);

void display_local_keys(void);
void display_remote_keys(void);

#ifdef  __cplusplus
}
#endif

#endif
#endif   //NFC_NXP_LLCP_SECURED_P2P End
