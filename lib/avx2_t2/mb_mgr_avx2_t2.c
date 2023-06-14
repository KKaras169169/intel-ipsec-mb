/*******************************************************************************
 Copyright (c) 2022-2023, Intel Corporation

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVX2

#include "intel-ipsec-mb.h"
#include "include/ipsec_ooo_mgr.h"
#include "include/kasumi_interface.h"
#include "include/zuc_internal.h"
#include "include/snow3g.h"
#include "include/snow3g_submit.h"
#include "include/gcm.h"
#include "include/chacha20_poly1305.h"

#include "include/save_xmms.h"
#include "include/des.h"
#include "include/cpu_feature.h"
#include "include/noaesni.h"
#include "include/aesni_emu.h"
#include "include/error.h"

#include "include/arch_sse_type1.h" /* poly1305, snow3g */
#include "include/arch_sse_type2.h" /* shani */
#include "include/arch_avx_type1.h"
#include "include/arch_avx2_type1.h"
#include "include/arch_avx2_type2.h"

#include "include/ooo_mgr_reset.h"

#define SAVE_XMMS               save_xmms_avx
#define RESTORE_XMMS            restore_xmms_avx

/* JOB API */
#define SUBMIT_JOB         submit_job_avx2_t2
#define FLUSH_JOB          flush_job_avx2_t2
#define QUEUE_SIZE         queue_size_avx2_t2
#define SUBMIT_JOB_NOCHECK submit_job_nocheck_avx2_t2
#define GET_NEXT_JOB       get_next_job_avx2_t2
#define GET_COMPLETED_JOB  get_completed_job_avx2_t2
#define GET_NEXT_BURST     get_next_burst_avx2_t2
#define SUBMIT_BURST       submit_burst_avx2_t2
#define SUBMIT_BURST_NOCHECK submit_burst_nocheck_avx2_t2
#define FLUSH_BURST       flush_burst_avx2_t2
#define SUBMIT_CIPHER_BURST submit_cipher_burst_avx2_t2
#define SUBMIT_CIPHER_BURST_NOCHECK submit_cipher_burst_nocheck_avx2_t2
#define SUBMIT_HASH_BURST submit_hash_burst_avx2_t2
#define SUBMIT_HASH_BURST_NOCHECK submit_hash_burst_nocheck_avx2_t2
#define SET_SUITE_ID_FN   set_suite_id_avx2_t2

/* Hash */
#define SUBMIT_JOB_HASH    SUBMIT_JOB_HASH_AVX2
#define FLUSH_JOB_HASH     FLUSH_JOB_HASH_AVX2

/* Cipher encrypt / decrypt */
#define SUBMIT_JOB_CIPHER_ENC SUBMIT_JOB_CIPHER_ENC_AVX2
#define FLUSH_JOB_CIPHER_ENC  FLUSH_JOB_CIPHER_ENC_AVX2
#define SUBMIT_JOB_CIPHER_DEC SUBMIT_JOB_CIPHER_DEC_AVX2

/* AES-GCM */
#define AES_GCM_DEC_IV_128   aes_gcm_dec_var_iv_128_avx_gen4
#define AES_GCM_ENC_IV_128   aes_gcm_enc_var_iv_128_avx_gen4
#define AES_GCM_DEC_IV_192   aes_gcm_dec_var_iv_192_avx_gen4
#define AES_GCM_ENC_IV_192   aes_gcm_enc_var_iv_192_avx_gen4
#define AES_GCM_DEC_IV_256   aes_gcm_dec_var_iv_256_avx_gen4
#define AES_GCM_ENC_IV_256   aes_gcm_enc_var_iv_256_avx_gen4

#define SUBMIT_JOB_AES_GCM_DEC submit_job_gcm_dec_avx2
#define SUBMIT_JOB_AES_GCM_ENC submit_job_gcm_enc_avx2

/* AES-CBC */
#define SUBMIT_JOB_AES_CBC_128_ENC submit_job_aes128_enc_avx
#define SUBMIT_JOB_AES_CBC_128_DEC submit_job_aes128_dec_avx
#define FLUSH_JOB_AES_CBC_128_ENC  flush_job_aes128_enc_avx

#define SUBMIT_JOB_AES_CBC_192_ENC submit_job_aes192_enc_avx
#define SUBMIT_JOB_AES_CBC_192_DEC submit_job_aes192_dec_avx
#define FLUSH_JOB_AES_CBC_192_ENC  flush_job_aes192_enc_avx

#define SUBMIT_JOB_AES_CBC_256_ENC submit_job_aes256_enc_avx
#define SUBMIT_JOB_AES_CBC_256_DEC submit_job_aes256_dec_avx
#define FLUSH_JOB_AES_CBC_256_ENC  flush_job_aes256_enc_avx

#define AES_CBC_DEC_128       aes_cbc_dec_128_avx
#define AES_CBC_DEC_192       aes_cbc_dec_192_avx
#define AES_CBC_DEC_256       aes_cbc_dec_256_avx

/* AES-CBCS */
#define SUBMIT_JOB_AES128_CBCS_1_9_ENC submit_job_aes128_cbcs_1_9_enc_avx
#define FLUSH_JOB_AES128_CBCS_1_9_ENC  flush_job_aes128_cbcs_1_9_enc_avx
#define SUBMIT_JOB_AES128_CBCS_1_9_DEC submit_job_aes128_cbcs_1_9_dec_avx
#define AES_CBCS_1_9_DEC_128           aes_cbcs_1_9_dec_128_avx

/* AES-ECB */
#define SUBMIT_JOB_AES_ECB_128_ENC submit_job_aes_ecb_128_enc_vaes_avx2
#define SUBMIT_JOB_AES_ECB_128_DEC submit_job_aes_ecb_128_dec_vaes_avx2
#define SUBMIT_JOB_AES_ECB_192_ENC submit_job_aes_ecb_192_enc_vaes_avx2
#define SUBMIT_JOB_AES_ECB_192_DEC submit_job_aes_ecb_192_dec_vaes_avx2
#define SUBMIT_JOB_AES_ECB_256_ENC submit_job_aes_ecb_256_enc_vaes_avx2
#define SUBMIT_JOB_AES_ECB_256_DEC submit_job_aes_ecb_256_dec_vaes_avx2

#define AES_ECB_ENC_128       aes_ecb_enc_128_vaes_avx2
#define AES_ECB_ENC_192       aes_ecb_enc_192_vaes_avx2
#define AES_ECB_ENC_256       aes_ecb_enc_256_vaes_avx2
#define AES_ECB_DEC_128       aes_ecb_dec_128_vaes_avx2
#define AES_ECB_DEC_192       aes_ecb_dec_192_vaes_avx2
#define AES_ECB_DEC_256       aes_ecb_dec_256_vaes_avx2

/* AES-CTR */
#define AES_CTR_128       aes_cntr_128_avx
#define AES_CTR_192       aes_cntr_192_avx
#define AES_CTR_256       aes_cntr_256_avx
#define AES_CTR_128_BIT   aes_cntr_bit_128_avx
#define AES_CTR_192_BIT   aes_cntr_bit_192_avx
#define AES_CTR_256_BIT   aes_cntr_bit_256_avx

/* AES-CCM */
#define AES_CNTR_CCM_128   aes_cntr_ccm_128_avx
#define AES_CNTR_CCM_256   aes_cntr_ccm_256_avx

#define FLUSH_JOB_AES128_CCM_AUTH     flush_job_aes128_ccm_auth_avx
#define SUBMIT_JOB_AES128_CCM_AUTH    submit_job_aes128_ccm_auth_avx

#define FLUSH_JOB_AES256_CCM_AUTH     flush_job_aes256_ccm_auth_avx
#define SUBMIT_JOB_AES256_CCM_AUTH    submit_job_aes256_ccm_auth_avx

/* AES-CMAC */
#define FLUSH_JOB_AES128_CMAC_AUTH    flush_job_aes128_cmac_auth_avx
#define SUBMIT_JOB_AES128_CMAC_AUTH   submit_job_aes128_cmac_auth_avx

#define FLUSH_JOB_AES256_CMAC_AUTH    flush_job_aes256_cmac_auth_avx
#define SUBMIT_JOB_AES256_CMAC_AUTH   submit_job_aes256_cmac_auth_avx

/* AES-CFB */
#define AES_CFB_128_ONE    aes_cfb_128_one_avx2
#define AES_CFB_256_ONE    aes_cfb_256_one_avx2

/* AES-XCBC */
#define SUBMIT_JOB_AES_XCBC   submit_job_aes_xcbc_avx
#define FLUSH_JOB_AES_XCBC    flush_job_aes_xcbc_avx

/* PON */
#define SUBMIT_JOB_PON_ENC        submit_job_pon_enc_avx
#define SUBMIT_JOB_PON_DEC        submit_job_pon_dec_avx
#define SUBMIT_JOB_PON_ENC_NO_CTR submit_job_pon_enc_no_ctr_avx
#define SUBMIT_JOB_PON_DEC_NO_CTR submit_job_pon_dec_no_ctr_avx

/* SHA1/224/256/384/512 */
/* note: SHA1 MB is better than SHANI on Xeon processors */
#define SUBMIT_JOB_SHA1     submit_job_sha1_avx2
#define FLUSH_JOB_SHA1      flush_job_sha1_avx2
#define SUBMIT_JOB_SHA224   submit_job_sha224_ni_sse
#define FLUSH_JOB_SHA224    flush_job_sha224_ni_sse
#define SUBMIT_JOB_SHA256   submit_job_sha256_ni_sse
#define FLUSH_JOB_SHA256    flush_job_sha256_ni_sse
#define SUBMIT_JOB_SHA384   submit_job_sha384_avx2
#define FLUSH_JOB_SHA384    flush_job_sha384_avx2
#define SUBMIT_JOB_SHA512   submit_job_sha512_avx2
#define FLUSH_JOB_SHA512    flush_job_sha512_avx2

/* HMAC-SHA1/224/256/384/512 */
#define SUBMIT_JOB_HMAC               submit_job_hmac_avx2
#define FLUSH_JOB_HMAC                flush_job_hmac_avx2
#define SUBMIT_JOB_HMAC_SHA_224       submit_job_hmac_sha_224_ni_sse
#define FLUSH_JOB_HMAC_SHA_224        flush_job_hmac_sha_224_ni_sse
#define SUBMIT_JOB_HMAC_SHA_256       submit_job_hmac_sha_256_ni_sse
#define FLUSH_JOB_HMAC_SHA_256        flush_job_hmac_sha_256_ni_sse
#define SUBMIT_JOB_HMAC_SHA_384       submit_job_hmac_sha_384_avx2
#define FLUSH_JOB_HMAC_SHA_384        flush_job_hmac_sha_384_avx2
#define SUBMIT_JOB_HMAC_SHA_512       submit_job_hmac_sha_512_avx2
#define FLUSH_JOB_HMAC_SHA_512        flush_job_hmac_sha_512_avx2
#define SUBMIT_JOB_HMAC_MD5           submit_job_hmac_md5_avx2
#define FLUSH_JOB_HMAC_MD5            flush_job_hmac_md5_avx2

/* CHACHA20 & POLY1305 */
#define SUBMIT_JOB_CHACHA20_ENC_DEC      submit_job_chacha20_enc_dec_avx2
#define SUBMIT_JOB_CHACHA20_POLY1305     aead_chacha20_poly1305_avx2
#define SUBMIT_JOB_CHACHA20_POLY1305_SGL aead_chacha20_poly1305_sgl_avx2
#define POLY1305_MAC                     poly1305_mac_scalar

/* ZUC EEA3 & EIA3 */
#define SUBMIT_JOB_ZUC_EEA3   submit_job_zuc_eea3_gfni_avx2
#define FLUSH_JOB_ZUC_EEA3    flush_job_zuc_eea3_gfni_avx2
#define SUBMIT_JOB_ZUC_EIA3   submit_job_zuc_eia3_gfni_avx2
#define FLUSH_JOB_ZUC_EIA3    flush_job_zuc_eia3_gfni_avx2
#define SUBMIT_JOB_ZUC256_EEA3   submit_job_zuc256_eea3_gfni_avx2
#define FLUSH_JOB_ZUC256_EEA3    flush_job_zuc256_eea3_gfni_avx2
#define SUBMIT_JOB_ZUC256_EIA3   submit_job_zuc256_eia3_gfni_avx2
#define FLUSH_JOB_ZUC256_EIA3    flush_job_zuc256_eia3_gfni_avx2

/* SNOW-V */
#define SUBMIT_JOB_SNOW_V      snow_v_avx
#define SUBMIT_JOB_SNOW_V_AEAD snow_v_aead_init_avx

/* SNOW3G UE2 & UIA2 */
static IMB_JOB *
submit_snow3g_uea2_job_avx2_t2(IMB_MGR *state, IMB_JOB *job)
{
        MB_MGR_SNOW3G_OOO *snow3g_uea2_ooo = state->snow3g_uea2_ooo;

        if ((job->msg_len_to_cipher_in_bits & 7) ||
            (job->cipher_start_offset_in_bits & 7))
                return def_submit_snow3g_uea2_job(state, job);

        return submit_job_snow3g_uea2_sse(snow3g_uea2_ooo, job);
}

static IMB_JOB *
flush_snow3g_uea2_job_avx2_t2(IMB_MGR *state)
{
        MB_MGR_SNOW3G_OOO *snow3g_uea2_ooo = state->snow3g_uea2_ooo;

        return flush_job_snow3g_uea2_sse(snow3g_uea2_ooo);
}

#define SUBMIT_JOB_SNOW3G_UEA2 submit_snow3g_uea2_job_avx2_t2
#define FLUSH_JOB_SNOW3G_UEA2  flush_snow3g_uea2_job_avx2_t2

#define SUBMIT_JOB_SNOW3G_UIA2 submit_job_snow3g_uia2_sse
#define FLUSH_JOB_SNOW3G_UIA2  flush_job_snow3g_uia2_sse

/* AES-DOCSIS */
#define ETHERNET_FCS ethernet_fcs_avx_local

static void reset_ooo_mgrs(IMB_MGR *state)
{
        /* Init AES out-of-order fields */
        ooo_mgr_aes_reset(state->aes128_ooo, 8);
        ooo_mgr_aes_reset(state->aes192_ooo, 8);
        ooo_mgr_aes_reset(state->aes256_ooo, 8);

        /* DOCSIS SEC BPI (AES CBC + AES CFB for partial block)
         * uses same settings as AES CBC.
         */
        ooo_mgr_docsis_aes_reset(state->docsis128_sec_ooo, 8);
        ooo_mgr_docsis_aes_reset(state->docsis128_crc32_sec_ooo, 8);
        ooo_mgr_docsis_aes_reset(state->docsis256_sec_ooo, 8);
        ooo_mgr_docsis_aes_reset(state->docsis256_crc32_sec_ooo, 8);

        /* Init ZUC out-of-order fields */
        ooo_mgr_zuc_reset(state->zuc_eea3_ooo, 8);
        ooo_mgr_zuc_reset(state->zuc_eia3_ooo, 8);
        ooo_mgr_zuc_reset(state->zuc256_eea3_ooo, 8);
        ooo_mgr_zuc_reset(state->zuc256_eia3_ooo, 8);

        /* Init HMAC/SHA1 out-of-order fields */
        ooo_mgr_hmac_sha1_reset(state->hmac_sha_1_ooo, AVX2_NUM_SHA1_LANES);

        /* Init HMAC/SHA224 out-of-order fields */
        ooo_mgr_hmac_sha224_reset(state->hmac_sha_224_ooo, 2);

        /* Init HMAC/SHA_256 out-of-order fields */
        ooo_mgr_hmac_sha256_reset(state->hmac_sha_256_ooo, 2);

        /* Init HMAC/SHA384 out-of-order fields */
        ooo_mgr_hmac_sha384_reset(state->hmac_sha_384_ooo,
                                  AVX2_NUM_SHA512_LANES);

        /* Init HMAC/SHA512 out-of-order fields */
        ooo_mgr_hmac_sha512_reset(state->hmac_sha_512_ooo,
                                  AVX2_NUM_SHA512_LANES);

        /* Init HMAC/MD5 out-of-order fields */
        ooo_mgr_hmac_md5_reset(state->hmac_md5_ooo, AVX2_NUM_MD5_LANES);

        /* Init AES/XCBC OOO fields */
        ooo_mgr_aes_xcbc_reset(state->aes_xcbc_ooo, 8);

        /* Init AES-CCM auth out-of-order fields */
        ooo_mgr_ccm_reset(state->aes_ccm_ooo, 8);
        ooo_mgr_ccm_reset(state->aes256_ccm_ooo, 8);

        /* Init AES-CMAC auth out-of-order fields */
        ooo_mgr_cmac_reset(state->aes_cmac_ooo, 8);
        ooo_mgr_cmac_reset(state->aes256_cmac_ooo, 8);

        /* Init AES CBC-S out-of-order fields */
        ooo_mgr_aes_reset(state->aes128_cbcs_ooo, 8);

        /* Init SHA1 out-of-order fields */
        ooo_mgr_sha1_reset(state->sha_1_ooo, AVX2_NUM_SHA1_LANES);

        /* Init SHA224 out-of-order fields */
        ooo_mgr_sha256_reset(state->sha_224_ooo, 2);

        /* Init SHA256 out-of-order fields */
        ooo_mgr_sha256_reset(state->sha_256_ooo, 2);

        /* Init SHA384 out-of-order fields */
        ooo_mgr_sha512_reset(state->sha_384_ooo, AVX2_NUM_SHA512_LANES);

        /* Init SHA512 out-of-order fields */
        ooo_mgr_sha512_reset(state->sha_512_ooo, AVX2_NUM_SHA512_LANES);

        /* Init SNOW3G-UEA out-of-order fields */
        ooo_mgr_snow3g_reset(state->snow3g_uea2_ooo, 4);

        /* Init SNOW3G-UIA out-of-order fields */
        ooo_mgr_snow3g_reset(state->snow3g_uia2_ooo, 4);
}

IMB_DLL_LOCAL void
init_mb_mgr_avx2_t2_internal(IMB_MGR *state, const int reset_mgrs)
{
        /* Check if CPU flags needed for AVX2 interface are present */
        if ((state->features & IMB_CPUFLAGS_AVX2) != IMB_CPUFLAGS_AVX2) {
                imb_set_errno(state, IMB_ERR_MISSING_CPUFLAGS_INIT_MGR);
                return;
        }

        /* Set architecture for future checks */
        state->used_arch = (uint32_t) IMB_ARCH_AVX2;

        if (reset_mgrs) {
                reset_ooo_mgrs(state);

                /* Init "in order" components */
                state->next_job = 0;
                state->earliest_job = -1;
        }

        /* set handlers */
        state->get_next_job        = GET_NEXT_JOB;
        state->submit_job          = SUBMIT_JOB;
        state->submit_job_nocheck  = SUBMIT_JOB_NOCHECK;
        state->get_completed_job   = GET_COMPLETED_JOB;
        state->flush_job           = FLUSH_JOB;
        state->queue_size          = QUEUE_SIZE;
        state->get_next_burst      = GET_NEXT_BURST;
        state->submit_burst        = SUBMIT_BURST;
        state->submit_burst_nocheck= SUBMIT_BURST_NOCHECK;
        state->flush_burst         = FLUSH_BURST;
        state->submit_cipher_burst = SUBMIT_CIPHER_BURST;
        state->submit_cipher_burst_nocheck = SUBMIT_CIPHER_BURST_NOCHECK;
        state->submit_hash_burst   = SUBMIT_HASH_BURST;
        state->submit_hash_burst_nocheck = SUBMIT_HASH_BURST_NOCHECK;
        state->set_suite_id        = SET_SUITE_ID_FN;

        state->keyexp_128          = aes_keyexp_128_avx2;
        state->keyexp_192          = aes_keyexp_192_avx2;
        state->keyexp_256          = aes_keyexp_256_avx2;

        state->cmac_subkey_gen_128 = aes_cmac_subkey_gen_avx2;
        state->cmac_subkey_gen_256 = aes_cmac_256_subkey_gen_avx2;

        state->xcbc_keyexp         = aes_xcbc_expand_key_avx2;
        state->des_key_sched       = des_key_schedule;

        state->sha1_one_block      = sha1_one_block_sse_shani;
        state->sha1                = sha1_sse_shani;
        state->sha224_one_block    = sha224_one_block_sse_shani;
        state->sha224              = sha224_sse_shani;
        state->sha256_one_block    = sha256_one_block_sse_shani;
        state->sha256              = sha256_sse_shani;
        state->sha384_one_block    = sha384_one_block_avx2;
        state->sha384              = sha384_avx2;
        state->sha512_one_block    = sha512_one_block_avx2;
        state->sha512              = sha512_avx2;
        state->md5_one_block       = md5_one_block_avx2;

        state->aes128_cfb_one      = aes_cfb_128_one_avx2;
        state->aes256_cfb_one      = aes_cfb_256_one_avx2;

        state->eea3_1_buffer       = zuc_eea3_1_buffer_avx2;
        state->eea3_4_buffer       = zuc_eea3_4_buffer_avx;
        state->eea3_n_buffer       = zuc_eea3_n_buffer_avx2;
        state->eia3_1_buffer       = zuc_eia3_1_buffer_avx2;
        state->eia3_n_buffer       = zuc_eia3_n_buffer_avx2;

        state->f8_1_buffer         = kasumi_f8_1_buffer_avx;
        state->f8_1_buffer_bit     = kasumi_f8_1_buffer_bit_avx;
        state->f8_2_buffer         = kasumi_f8_2_buffer_avx;
        state->f8_3_buffer         = kasumi_f8_3_buffer_avx;
        state->f8_4_buffer         = kasumi_f8_4_buffer_avx;
        state->f8_n_buffer         = kasumi_f8_n_buffer_avx;
        state->f9_1_buffer         = kasumi_f9_1_buffer_avx;
        state->f9_1_buffer_user    = kasumi_f9_1_buffer_user_avx;
        state->kasumi_init_f8_key_sched = kasumi_init_f8_key_sched_avx;
        state->kasumi_init_f9_key_sched = kasumi_init_f9_key_sched_avx;
        state->kasumi_key_sched_size = kasumi_key_sched_size_avx;

        state->snow3g_f8_1_buffer_bit = snow3g_f8_1_buffer_bit_avx2;
        state->snow3g_f8_1_buffer  = snow3g_f8_1_buffer_avx2;
        state->snow3g_f8_2_buffer  = snow3g_f8_2_buffer_avx2;
        state->snow3g_f8_4_buffer  = snow3g_f8_4_buffer_avx2;
        state->snow3g_f8_8_buffer  = snow3g_f8_8_buffer_avx2;
        state->snow3g_f8_n_buffer  = snow3g_f8_n_buffer_avx2;
        state->snow3g_f8_8_buffer_multikey = snow3g_f8_8_buffer_multikey_avx2;
        state->snow3g_f8_n_buffer_multikey = snow3g_f8_n_buffer_multikey_avx2;
        state->snow3g_f9_1_buffer = snow3g_f9_1_buffer_avx2;
        state->snow3g_init_key_sched = snow3g_init_key_sched_avx2;
        state->snow3g_key_sched_size = snow3g_key_sched_size_avx2;

        state->hec_32              = hec_32_avx;
        state->hec_64              = hec_64_avx;

        state->crc32_ethernet_fcs  = ethernet_fcs_avx;
        state->crc16_x25           = crc16_x25_avx;
        state->crc32_sctp          = crc32_sctp_avx;
        state->crc24_lte_a         = crc24_lte_a_avx;
        state->crc24_lte_b         = crc24_lte_b_avx;
        state->crc16_fp_data       = crc16_fp_data_avx;
        state->crc11_fp_header     = crc11_fp_header_avx;
        state->crc7_fp_header      = crc7_fp_header_avx;
        state->crc10_iuup_data     = crc10_iuup_data_avx;
        state->crc6_iuup_header    = crc6_iuup_header_avx;
        state->crc32_wimax_ofdma_data = crc32_wimax_ofdma_data_avx;
        state->crc8_wimax_ofdma_hcs = crc8_wimax_ofdma_hcs_avx;

        state->chacha20_poly1305_init = init_chacha20_poly1305_avx;
        state->chacha20_poly1305_enc_update = update_enc_chacha20_poly1305_avx2;
        state->chacha20_poly1305_dec_update = update_dec_chacha20_poly1305_avx2;
        state->chacha20_poly1305_finalize = finalize_chacha20_poly1305_avx;

        state->gcm128_enc          = aes_gcm_enc_128_avx_gen4;
        state->gcm192_enc          = aes_gcm_enc_192_avx_gen4;
        state->gcm256_enc          = aes_gcm_enc_256_avx_gen4;
        state->gcm128_dec          = aes_gcm_dec_128_avx_gen4;
        state->gcm192_dec          = aes_gcm_dec_192_avx_gen4;
        state->gcm256_dec          = aes_gcm_dec_256_avx_gen4;
        state->gcm128_init         = aes_gcm_init_128_avx_gen4;
        state->gcm192_init         = aes_gcm_init_192_avx_gen4;
        state->gcm256_init         = aes_gcm_init_256_avx_gen4;
        state->gcm128_init_var_iv  = aes_gcm_init_var_iv_128_avx_gen4;
        state->gcm192_init_var_iv  = aes_gcm_init_var_iv_192_avx_gen4;
        state->gcm256_init_var_iv  = aes_gcm_init_var_iv_256_avx_gen4;
        state->gcm128_enc_update   = aes_gcm_enc_128_update_avx_gen4;
        state->gcm192_enc_update   = aes_gcm_enc_192_update_avx_gen4;
        state->gcm256_enc_update   = aes_gcm_enc_256_update_avx_gen4;
        state->gcm128_dec_update   = aes_gcm_dec_128_update_avx_gen4;
        state->gcm192_dec_update   = aes_gcm_dec_192_update_avx_gen4;
        state->gcm256_dec_update   = aes_gcm_dec_256_update_avx_gen4;
        state->gcm128_enc_finalize = aes_gcm_enc_128_finalize_avx_gen4;
        state->gcm192_enc_finalize = aes_gcm_enc_192_finalize_avx_gen4;
        state->gcm256_enc_finalize = aes_gcm_enc_256_finalize_avx_gen4;
        state->gcm128_dec_finalize = aes_gcm_dec_128_finalize_avx_gen4;
        state->gcm192_dec_finalize = aes_gcm_dec_192_finalize_avx_gen4;
        state->gcm256_dec_finalize = aes_gcm_dec_256_finalize_avx_gen4;
        state->gcm128_precomp      = aes_gcm_precomp_128_avx_gen4;
        state->gcm192_precomp      = aes_gcm_precomp_192_avx_gen4;
        state->gcm256_precomp      = aes_gcm_precomp_256_avx_gen4;
        state->gcm128_pre          = aes_gcm_pre_128_avx_gen4;
        state->gcm192_pre          = aes_gcm_pre_192_avx_gen4;
        state->gcm256_pre          = aes_gcm_pre_256_avx_gen4;

        state->ghash               = ghash_avx_gen4;
        state->ghash_pre           = ghash_pre_avx_gen4;

        state->gmac128_init        = imb_aes_gmac_init_128_avx_gen4;
        state->gmac192_init        = imb_aes_gmac_init_192_avx_gen4;
        state->gmac256_init        = imb_aes_gmac_init_256_avx_gen4;
        state->gmac128_update      = imb_aes_gmac_update_128_avx_gen4;
        state->gmac192_update      = imb_aes_gmac_update_192_avx_gen4;
        state->gmac256_update      = imb_aes_gmac_update_256_avx_gen4;
        state->gmac128_finalize    = imb_aes_gmac_finalize_128_avx_gen4;
        state->gmac192_finalize    = imb_aes_gmac_finalize_192_avx_gen4;
        state->gmac256_finalize    = imb_aes_gmac_finalize_256_avx_gen4;

        state->aes_ecb_128_quic    = aes_ecb_quic_enc_128_avx;
        state->aes_ecb_256_quic    = aes_ecb_quic_enc_256_avx;

        state->chacha20_poly1305_quic = aead_chacha20_poly1305_avx2;
}

#include "mb_mgr_code.h"
