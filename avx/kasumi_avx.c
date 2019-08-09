/*******************************************************************************
  Copyright (c) 2009-2019, Intel Corporation

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

#include <limits.h>

#include "include/save_xmms.h"
#include "include/kasumi_internal.h"
#include "include/save_xmms.h"
#include "include/clear_regs_mem.h"

#define SAVE_XMMS       save_xmms_avx
#define RESTORE_XMMS    restore_xmms_avx

void
kasumi_f8_1_buffer_avx(const kasumi_key_sched_t *pCtx, const uint64_t IV,
                       const void *pBufferIn, void *pBufferOut,
                       const uint32_t cipherLengthInBytes)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL || pBufferIn == NULL || pBufferOut == NULL)
                return;

        /* Check input data is in range of supported length */
        if (cipherLengthInBytes == 0 ||
            cipherLengthInBytes > (KASUMI_MAX_LEN / CHAR_BIT))
                return;
#endif
        kasumi_f8_1_buffer(pCtx, IV, pBufferIn, pBufferOut,
                           cipherLengthInBytes);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f8_1_buffer_bit_avx(const kasumi_key_sched_t *pCtx, const uint64_t IV,
                           const void *pBufferIn,
                           void *pBufferOut,
                           const uint32_t cipherLengthInBits,
                           const uint32_t offsetInBits)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL || pBufferIn == NULL || pBufferOut == NULL)
                return;

        /* Check input data is in range of supported length */
        if (cipherLengthInBits == 0 ||
            cipherLengthInBits > KASUMI_MAX_LEN)
                return;
#endif
        kasumi_f8_1_buffer_bit(pCtx, IV, pBufferIn, pBufferOut,
                               cipherLengthInBits, offsetInBits);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f8_2_buffer_avx(const kasumi_key_sched_t *pCtx, const uint64_t IV1,
                       const uint64_t IV2, const void *pBufferIn1,
                       void *pBufferOut1, const uint32_t lengthInBytes1,
                       const void *pBufferIn2, void *pBufferOut2,
                       const uint32_t lengthInBytes2)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL)
                return;

        if (pBufferIn1 == NULL || pBufferOut1 == NULL)
                return;

        if (pBufferIn2 == NULL || pBufferOut2 == NULL)
                return;

        /* Check input data is in range of supported length */
        if (lengthInBytes1 == 0 || lengthInBytes1 > (KASUMI_MAX_LEN / CHAR_BIT))
                return;

        if (lengthInBytes2 == 0 || lengthInBytes2 > (KASUMI_MAX_LEN / CHAR_BIT))
                return;
#endif
        kasumi_f8_2_buffer(pCtx, IV1, IV2,
                           pBufferIn1, pBufferOut1, lengthInBytes1,
                           pBufferIn2, pBufferOut2, lengthInBytes2);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f8_3_buffer_avx(const kasumi_key_sched_t *pCtx, const uint64_t IV1,
                       const uint64_t IV2, const uint64_t IV3,
                       const void *pBufferIn1, void *pBufferOut1,
                       const void *pBufferIn2, void *pBufferOut2,
                       const void *pBufferIn3, void *pBufferOut3,
                       const uint32_t lengthInBytes)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL)
                return;

        if (pBufferIn1 == NULL || pBufferOut1 == NULL)
                return;

        if (pBufferIn2 == NULL || pBufferOut2 == NULL)
                return;

        if (pBufferIn3 == NULL || pBufferOut3 == NULL)
                return;

        /* Check input data is in range of supported length */
        if (lengthInBytes == 0 || lengthInBytes > (KASUMI_MAX_LEN / CHAR_BIT))
                return;
#endif
        kasumi_f8_3_buffer(pCtx, IV1, IV2, IV3,
                           pBufferIn1, pBufferOut1,
                           pBufferIn2, pBufferOut2,
                           pBufferIn3, pBufferOut3, lengthInBytes);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f8_4_buffer_avx(const kasumi_key_sched_t *pCtx,
                       const uint64_t IV1, const uint64_t IV2,
                       const uint64_t IV3, const uint64_t IV4,
                       const void *pBufferIn1, void *pBufferOut1,
                       const void *pBufferIn2, void *pBufferOut2,
                       const void *pBufferIn3, void *pBufferOut3,
                       const void *pBufferIn4, void *pBufferOut4,
                       const uint32_t lengthInBytes)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL)
                return;

        if (pBufferIn1 == NULL || pBufferOut1 == NULL)
                return;

        if (pBufferIn2 == NULL || pBufferOut2 == NULL)
                return;

        if (pBufferIn3 == NULL || pBufferOut3 == NULL)
                return;

        if (pBufferIn4 == NULL || pBufferOut4 == NULL)
                return;

        /* Check input data is in range of supported length */
        if (lengthInBytes == 0 || lengthInBytes > (KASUMI_MAX_LEN / CHAR_BIT))
                return;
#endif
        kasumi_f8_4_buffer(pCtx, IV1, IV2, IV3, IV4,
                           pBufferIn1, pBufferOut1,
                           pBufferIn2, pBufferOut2,
                           pBufferIn3, pBufferOut3,
                           pBufferIn4, pBufferOut4,
                           lengthInBytes);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f8_n_buffer_avx(const kasumi_key_sched_t *pKeySchedule,
                       const uint64_t IV[],
                       const void *pDataIn[], void *pDataOut[],
                       const uint32_t dataLen[], const uint32_t dataCount)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
        uint32_t numLeft = dataCount;
        const uint64_t *IVPtr;
        const void **pDataInPtr;
        void **pDataOutPtr;
        const uint32_t *dataLenPtr;
        uint32_t i = 0;
        uint32_t numBuffs;

#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pKeySchedule == NULL || pDataIn == NULL || pDataOut == NULL ||
            dataLen == NULL || IV == NULL)
                return;

        for (i = 0; i < dataCount; i++) {
                /* Check for NULL pointers */
                if (pDataIn[i] == NULL || pDataOut[i] == NULL)
                        return;

                /* Check input data is in range of supported length */
                if (dataLen[i] == 0 || dataLen[i] > (KASUMI_MAX_LEN / CHAR_BIT))
                        return;
        }
#endif

        i = 0;

        /* KASUMI F8 n buffer function can handle up to 16 buffers */
        while (numLeft > 0) {
                IVPtr = &IV[i];
                pDataInPtr = &pDataIn[i];
                pDataOutPtr = &pDataOut[i];
                dataLenPtr = &dataLen[i];
                numBuffs = (numLeft > 16) ? 16 : numLeft;

                kasumi_f8_n_buffer(pKeySchedule, IVPtr, pDataInPtr, pDataOutPtr,
                                   dataLenPtr, numBuffs);
                i += numBuffs;
                numLeft -= numBuffs;
        }
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}


void
kasumi_f9_1_buffer_avx(const kasumi_key_sched_t *pCtx, const void *pBufferIn,
                       const uint32_t lengthInBytes, void *pDigest)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL || pBufferIn == NULL || pDigest == NULL)
                return;

        /* Check input data is in range of supported length */
        if (lengthInBytes == 0 || lengthInBytes > (KASUMI_MAX_LEN / CHAR_BIT))
                return;
#endif
        kasumi_f9_1_buffer(pCtx, pBufferIn, lengthInBytes, pDigest);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}

void
kasumi_f9_1_buffer_user_avx(const kasumi_key_sched_t *pCtx, const uint64_t IV,
                            const void *pBufferIn, const uint32_t lengthInBits,
                            void *pDigest, const uint32_t direction)
{
#ifndef LINUX
        DECLARE_ALIGNED(uint128_t xmm_save[10], 16);

        SAVE_XMMS(xmm_save);
#endif
#ifdef SAFE_PARAM
        /* Check for NULL pointers */
        if (pCtx == NULL || pBufferIn == NULL || pDigest == NULL)
                return;

        /* Check input data is in range of supported length */
        if (lengthInBits == 0 || lengthInBits > KASUMI_MAX_LEN)
                return;
#endif
        kasumi_f9_1_buffer_user(pCtx, IV, pBufferIn, lengthInBits,
                                pDigest, direction);
#ifdef SAFE_DATA
        /* Clear sensitive data in registers */
        clear_gps();
        clear_xmms_avx();
#endif
#ifndef LINUX
        RESTORE_XMMS(xmm_save);
#endif
}