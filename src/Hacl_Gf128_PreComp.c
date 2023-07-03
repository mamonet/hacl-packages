/* MIT License
 *
 * Copyright (c) 2016-2022 INRIA, CMU and Microsoft Corporation
 * Copyright (c) 2022-2023 HACL* Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "Hacl_Gf128_PreComp.h"

void Hacl_Impl_Gf128_FieldPreComp_fmul(uint64_t *x, uint64_t *y)
{
  uint64_t res[2U] = { 0U };
  uint64_t y_[2U] = { 0U };
  y_[0U] = y[0U];
  y_[1U] = y[1U];
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    uint64_t m = (uint64_t)0U - (x[1U] >> ((uint32_t)63U - i) & (uint64_t)1U);
    res[0U] = res[0U] ^ (y_[0U] & m);
    res[1U] = res[1U] ^ (y_[1U] & m);
    uint64_t m0 = (uint64_t)0U - (y_[0U] & (uint64_t)1U);
    y_[0U] = y_[0U] >> (uint32_t)1U | y_[1U] << (uint32_t)63U;
    y_[1U] = y_[1U] >> (uint32_t)1U ^ ((uint64_t)0xE100000000000000U & m0);
  }
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    uint64_t m = (uint64_t)0U - (x[0U] >> ((uint32_t)63U - i) & (uint64_t)1U);
    res[0U] = res[0U] ^ (y_[0U] & m);
    res[1U] = res[1U] ^ (y_[1U] & m);
    uint64_t m0 = (uint64_t)0U - (y_[0U] & (uint64_t)1U);
    y_[0U] = y_[0U] >> (uint32_t)1U | y_[1U] << (uint32_t)63U;
    y_[1U] = y_[1U] >> (uint32_t)1U ^ ((uint64_t)0xE100000000000000U & m0);
  }
  x[0U] = res[0U];
  x[1U] = res[1U];
}

static inline void prepare(uint64_t *pre, uint64_t *r)
{
  memset(pre, 0U, (uint32_t)256U * sizeof (uint64_t));
  uint64_t sh[2U] = { 0U };
  sh[0U] = r[0U];
  sh[1U] = r[1U];
  uint64_t *pre1 = pre;
  uint64_t *pre2 = pre + (uint32_t)128U;
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    memcpy(pre1 + (uint32_t)2U * i, sh, (uint32_t)2U * sizeof (uint64_t));
    uint64_t m = (uint64_t)0U - (sh[0U] & (uint64_t)1U);
    sh[0U] = sh[0U] >> (uint32_t)1U | sh[1U] << (uint32_t)63U;
    sh[1U] = sh[1U] >> (uint32_t)1U ^ ((uint64_t)0xE100000000000000U & m);
  }
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    memcpy(pre2 + (uint32_t)2U * i, sh, (uint32_t)2U * sizeof (uint64_t));
    uint64_t m = (uint64_t)0U - (sh[0U] & (uint64_t)1U);
    sh[0U] = sh[0U] >> (uint32_t)1U | sh[1U] << (uint32_t)63U;
    sh[1U] = sh[1U] >> (uint32_t)1U ^ ((uint64_t)0xE100000000000000U & m);
  }
}

void Hacl_Impl_Gf128_FieldPreComp_load_precompute_r(uint64_t *pre, uint8_t *key)
{
  uint64_t *r4321 = pre;
  uint64_t *r1 = r4321 + (uint32_t)6U;
  uint64_t *r2 = r4321 + (uint32_t)4U;
  uint64_t *r3 = r4321 + (uint32_t)2U;
  uint64_t *r4 = r4321;
  uint64_t *table2 = pre + (uint32_t)8U;
  uint64_t u = load64_be(key);
  r1[1U] = u;
  uint64_t u0 = load64_be(key + (uint32_t)8U);
  r1[0U] = u0;
  r4[0U] = r1[0U];
  r4[1U] = r1[1U];
  r3[0U] = r1[0U];
  r3[1U] = r1[1U];
  r2[0U] = r1[0U];
  r2[1U] = r1[1U];
  Hacl_Impl_Gf128_FieldPreComp_fmul(r2, r1);
  Hacl_Impl_Gf128_FieldPreComp_fmul(r3, r2);
  Hacl_Impl_Gf128_FieldPreComp_fmul(r4, r3);
  prepare(table2, r4);
}

static inline void fmul_pre(uint64_t *x, uint64_t *pre)
{
  uint64_t *tab = pre + (uint32_t)8U;
  uint64_t tmp[2U] = { 0U };
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    uint64_t *uu____0 = tab + (uint32_t)2U * i;
    uint64_t m = (uint64_t)0U - (x[1U] >> ((uint32_t)63U - i) & (uint64_t)1U);
    tmp[0U] = tmp[0U] ^ (uu____0[0U] & m);
    tmp[1U] = tmp[1U] ^ (uu____0[1U] & m);
  }
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i++)
  {
    uint64_t *uu____1 = tab + (uint32_t)128U + (uint32_t)2U * i;
    uint64_t m = (uint64_t)0U - (x[0U] >> ((uint32_t)63U - i) & (uint64_t)1U);
    tmp[0U] = tmp[0U] ^ (uu____1[0U] & m);
    tmp[1U] = tmp[1U] ^ (uu____1[1U] & m);
  }
  x[0U] = tmp[0U];
  x[1U] = tmp[1U];
}

void Hacl_Impl_Gf128_FieldPreComp_fmul_r4(uint64_t *x, uint64_t *pre)
{
  fmul_pre(x, pre);
  fmul_pre(x + (uint32_t)2U, pre);
  fmul_pre(x + (uint32_t)4U, pre);
  fmul_pre(x + (uint32_t)6U, pre);
}

void Hacl_Impl_Gf128_FieldPreComp_normalize4(uint64_t *acc, uint64_t *x, uint64_t *pre)
{
  uint64_t *x1 = x;
  uint64_t *x2 = x + (uint32_t)2U;
  uint64_t *x3 = x + (uint32_t)4U;
  uint64_t *x4 = x + (uint32_t)6U;
  fmul_pre(x, pre);
  Hacl_Impl_Gf128_FieldPreComp_fmul(x + (uint32_t)2U, pre + (uint32_t)2U);
  Hacl_Impl_Gf128_FieldPreComp_fmul(x + (uint32_t)4U, pre + (uint32_t)4U);
  Hacl_Impl_Gf128_FieldPreComp_fmul(x + (uint32_t)6U, pre + (uint32_t)6U);
  acc[0U] = x1[0U];
  acc[1U] = x1[1U];
  acc[0U] = acc[0U] ^ x2[0U];
  acc[1U] = acc[1U] ^ x2[1U];
  acc[0U] = acc[0U] ^ x3[0U];
  acc[1U] = acc[1U] ^ x3[1U];
  acc[0U] = acc[0U] ^ x4[0U];
  acc[1U] = acc[1U] ^ x4[1U];
}

void Hacl_Gf128_PreComp_gcm_init(uint64_t *ctx, uint8_t *key)
{
  uint64_t *acc = ctx;
  uint64_t *pre = ctx + (uint32_t)2U;
  acc[0U] = (uint64_t)0U;
  acc[1U] = (uint64_t)0U;
  Hacl_Impl_Gf128_FieldPreComp_load_precompute_r(pre, key);
}

void Hacl_Gf128_PreComp_gcm_update_blocks(uint64_t *ctx, uint32_t len, uint8_t *text)
{
  uint64_t *acc = ctx;
  uint64_t *pre = ctx + (uint32_t)2U;
  uint32_t len0 = len / (uint32_t)64U * (uint32_t)64U;
  uint8_t *t0 = text;
  if (len0 > (uint32_t)0U)
  {
    uint64_t f0[8U] = { 0U };
    uint64_t *b4 = f0;
    uint64_t f[8U] = { 0U };
    uint64_t *acc4 = f;
    uint8_t *tb = t0;
    memcpy(acc4, acc, (uint32_t)2U * sizeof (uint64_t));
    uint64_t *x00 = b4;
    uint8_t *y00 = tb;
    uint64_t *x10 = b4 + (uint32_t)2U;
    uint8_t *y10 = tb + (uint32_t)16U;
    uint64_t *x20 = b4 + (uint32_t)4U;
    uint8_t *y20 = tb + (uint32_t)32U;
    uint64_t *x30 = b4 + (uint32_t)6U;
    uint8_t *y30 = tb + (uint32_t)48U;
    uint64_t u0 = load64_be(y00);
    x00[1U] = u0;
    uint64_t u1 = load64_be(y00 + (uint32_t)8U);
    x00[0U] = u1;
    uint64_t u2 = load64_be(y10);
    x10[1U] = u2;
    uint64_t u3 = load64_be(y10 + (uint32_t)8U);
    x10[0U] = u3;
    uint64_t u4 = load64_be(y20);
    x20[1U] = u4;
    uint64_t u5 = load64_be(y20 + (uint32_t)8U);
    x20[0U] = u5;
    uint64_t u6 = load64_be(y30);
    x30[1U] = u6;
    uint64_t u7 = load64_be(y30 + (uint32_t)8U);
    x30[0U] = u7;
    uint64_t *x01 = acc4;
    uint64_t *y01 = b4;
    uint64_t *x11 = acc4 + (uint32_t)2U;
    uint64_t *y11 = b4 + (uint32_t)2U;
    uint64_t *x21 = acc4 + (uint32_t)4U;
    uint64_t *y21 = b4 + (uint32_t)4U;
    uint64_t *x31 = acc4 + (uint32_t)6U;
    uint64_t *y31 = b4 + (uint32_t)6U;
    x01[0U] = x01[0U] ^ y01[0U];
    x01[1U] = x01[1U] ^ y01[1U];
    x11[0U] = x11[0U] ^ y11[0U];
    x11[1U] = x11[1U] ^ y11[1U];
    x21[0U] = x21[0U] ^ y21[0U];
    x21[1U] = x21[1U] ^ y21[1U];
    x31[0U] = x31[0U] ^ y31[0U];
    x31[1U] = x31[1U] ^ y31[1U];
    uint32_t len1 = len0 - (uint32_t)64U;
    uint8_t *text1 = t0 + (uint32_t)64U;
    uint32_t nb = len1 / (uint32_t)64U;
    for (uint32_t i = (uint32_t)0U; i < nb; i++)
    {
      uint8_t *tb1 = text1 + i * (uint32_t)64U;
      uint64_t *x0 = b4;
      uint8_t *y02 = tb1;
      uint64_t *x12 = b4 + (uint32_t)2U;
      uint8_t *y12 = tb1 + (uint32_t)16U;
      uint64_t *x22 = b4 + (uint32_t)4U;
      uint8_t *y22 = tb1 + (uint32_t)32U;
      uint64_t *x32 = b4 + (uint32_t)6U;
      uint8_t *y32 = tb1 + (uint32_t)48U;
      uint64_t u = load64_be(y02);
      x0[1U] = u;
      uint64_t u8 = load64_be(y02 + (uint32_t)8U);
      x0[0U] = u8;
      uint64_t u9 = load64_be(y12);
      x12[1U] = u9;
      uint64_t u10 = load64_be(y12 + (uint32_t)8U);
      x12[0U] = u10;
      uint64_t u11 = load64_be(y22);
      x22[1U] = u11;
      uint64_t u12 = load64_be(y22 + (uint32_t)8U);
      x22[0U] = u12;
      uint64_t u13 = load64_be(y32);
      x32[1U] = u13;
      uint64_t u14 = load64_be(y32 + (uint32_t)8U);
      x32[0U] = u14;
      Hacl_Impl_Gf128_FieldPreComp_fmul_r4(acc4, pre);
      uint64_t *x02 = acc4;
      uint64_t *y0 = b4;
      uint64_t *x1 = acc4 + (uint32_t)2U;
      uint64_t *y1 = b4 + (uint32_t)2U;
      uint64_t *x2 = acc4 + (uint32_t)4U;
      uint64_t *y2 = b4 + (uint32_t)4U;
      uint64_t *x3 = acc4 + (uint32_t)6U;
      uint64_t *y3 = b4 + (uint32_t)6U;
      x02[0U] = x02[0U] ^ y0[0U];
      x02[1U] = x02[1U] ^ y0[1U];
      x1[0U] = x1[0U] ^ y1[0U];
      x1[1U] = x1[1U] ^ y1[1U];
      x2[0U] = x2[0U] ^ y2[0U];
      x2[1U] = x2[1U] ^ y2[1U];
      x3[0U] = x3[0U] ^ y3[0U];
      x3[1U] = x3[1U] ^ y3[1U];
    }
    Hacl_Impl_Gf128_FieldPreComp_normalize4(acc, acc4, pre);
  }
  uint32_t len1 = len - len0;
  uint8_t *t1 = text + len0;
  uint64_t *r1 = pre + (uint32_t)6U;
  uint32_t nb = len1 / (uint32_t)16U;
  uint32_t rem = len1 % (uint32_t)16U;
  for (uint32_t i = (uint32_t)0U; i < nb; i++)
  {
    uint8_t *tb = t1 + i * (uint32_t)16U;
    uint64_t elem[2U] = { 0U };
    uint64_t u = load64_be(tb);
    elem[1U] = u;
    uint64_t u0 = load64_be(tb + (uint32_t)8U);
    elem[0U] = u0;
    acc[0U] = acc[0U] ^ elem[0U];
    acc[1U] = acc[1U] ^ elem[1U];
    Hacl_Impl_Gf128_FieldPreComp_fmul(acc, r1);
  }
  if (rem > (uint32_t)0U)
  {
    uint8_t *last = t1 + nb * (uint32_t)16U;
    uint64_t elem[2U] = { 0U };
    uint8_t b[16U] = { 0U };
    memcpy(b, last, rem * sizeof (uint8_t));
    uint64_t u = load64_be(b);
    elem[1U] = u;
    uint64_t u0 = load64_be(b + (uint32_t)8U);
    elem[0U] = u0;
    acc[0U] = acc[0U] ^ elem[0U];
    acc[1U] = acc[1U] ^ elem[1U];
    Hacl_Impl_Gf128_FieldPreComp_fmul(acc, r1);
    return;
  }
}

void
(*Hacl_Gf128_PreComp_gcm_update_blocks_padded)(uint64_t *x0, uint32_t x1, uint8_t *x2) =
  Hacl_Gf128_PreComp_gcm_update_blocks;

void Hacl_Gf128_PreComp_gcm_emit(uint8_t *tag, uint64_t *ctx)
{
  uint64_t *acc = ctx;
  uint64_t r0 = acc[1U];
  uint64_t r1 = acc[0U];
  store64_be(tag, r0);
  store64_be(tag + (uint32_t)8U, r1);
}

void Hacl_Gf128_PreComp_ghash(uint8_t *tag, uint32_t len, uint8_t *text, uint8_t *key)
{
  uint64_t ctx[266U] = { 0U };
  uint64_t *acc = ctx;
  uint64_t *pre0 = ctx + (uint32_t)2U;
  acc[0U] = (uint64_t)0U;
  acc[1U] = (uint64_t)0U;
  Hacl_Impl_Gf128_FieldPreComp_load_precompute_r(pre0, key);
  uint64_t *acc0 = ctx;
  uint64_t *pre = ctx + (uint32_t)2U;
  uint32_t len0 = len / (uint32_t)64U * (uint32_t)64U;
  uint8_t *t0 = text;
  if (len0 > (uint32_t)0U)
  {
    uint64_t f0[8U] = { 0U };
    uint64_t *b4 = f0;
    uint64_t f[8U] = { 0U };
    uint64_t *acc4 = f;
    uint8_t *tb = t0;
    memcpy(acc4, acc0, (uint32_t)2U * sizeof (uint64_t));
    uint64_t *x00 = b4;
    uint8_t *y00 = tb;
    uint64_t *x10 = b4 + (uint32_t)2U;
    uint8_t *y10 = tb + (uint32_t)16U;
    uint64_t *x20 = b4 + (uint32_t)4U;
    uint8_t *y20 = tb + (uint32_t)32U;
    uint64_t *x30 = b4 + (uint32_t)6U;
    uint8_t *y30 = tb + (uint32_t)48U;
    uint64_t u0 = load64_be(y00);
    x00[1U] = u0;
    uint64_t u1 = load64_be(y00 + (uint32_t)8U);
    x00[0U] = u1;
    uint64_t u2 = load64_be(y10);
    x10[1U] = u2;
    uint64_t u3 = load64_be(y10 + (uint32_t)8U);
    x10[0U] = u3;
    uint64_t u4 = load64_be(y20);
    x20[1U] = u4;
    uint64_t u5 = load64_be(y20 + (uint32_t)8U);
    x20[0U] = u5;
    uint64_t u6 = load64_be(y30);
    x30[1U] = u6;
    uint64_t u7 = load64_be(y30 + (uint32_t)8U);
    x30[0U] = u7;
    uint64_t *x01 = acc4;
    uint64_t *y01 = b4;
    uint64_t *x11 = acc4 + (uint32_t)2U;
    uint64_t *y11 = b4 + (uint32_t)2U;
    uint64_t *x21 = acc4 + (uint32_t)4U;
    uint64_t *y21 = b4 + (uint32_t)4U;
    uint64_t *x31 = acc4 + (uint32_t)6U;
    uint64_t *y31 = b4 + (uint32_t)6U;
    x01[0U] = x01[0U] ^ y01[0U];
    x01[1U] = x01[1U] ^ y01[1U];
    x11[0U] = x11[0U] ^ y11[0U];
    x11[1U] = x11[1U] ^ y11[1U];
    x21[0U] = x21[0U] ^ y21[0U];
    x21[1U] = x21[1U] ^ y21[1U];
    x31[0U] = x31[0U] ^ y31[0U];
    x31[1U] = x31[1U] ^ y31[1U];
    uint32_t len1 = len0 - (uint32_t)64U;
    uint8_t *text1 = t0 + (uint32_t)64U;
    uint32_t nb = len1 / (uint32_t)64U;
    for (uint32_t i = (uint32_t)0U; i < nb; i++)
    {
      uint8_t *tb1 = text1 + i * (uint32_t)64U;
      uint64_t *x0 = b4;
      uint8_t *y02 = tb1;
      uint64_t *x12 = b4 + (uint32_t)2U;
      uint8_t *y12 = tb1 + (uint32_t)16U;
      uint64_t *x22 = b4 + (uint32_t)4U;
      uint8_t *y22 = tb1 + (uint32_t)32U;
      uint64_t *x32 = b4 + (uint32_t)6U;
      uint8_t *y32 = tb1 + (uint32_t)48U;
      uint64_t u = load64_be(y02);
      x0[1U] = u;
      uint64_t u8 = load64_be(y02 + (uint32_t)8U);
      x0[0U] = u8;
      uint64_t u9 = load64_be(y12);
      x12[1U] = u9;
      uint64_t u10 = load64_be(y12 + (uint32_t)8U);
      x12[0U] = u10;
      uint64_t u11 = load64_be(y22);
      x22[1U] = u11;
      uint64_t u12 = load64_be(y22 + (uint32_t)8U);
      x22[0U] = u12;
      uint64_t u13 = load64_be(y32);
      x32[1U] = u13;
      uint64_t u14 = load64_be(y32 + (uint32_t)8U);
      x32[0U] = u14;
      Hacl_Impl_Gf128_FieldPreComp_fmul_r4(acc4, pre);
      uint64_t *x02 = acc4;
      uint64_t *y0 = b4;
      uint64_t *x1 = acc4 + (uint32_t)2U;
      uint64_t *y1 = b4 + (uint32_t)2U;
      uint64_t *x2 = acc4 + (uint32_t)4U;
      uint64_t *y2 = b4 + (uint32_t)4U;
      uint64_t *x3 = acc4 + (uint32_t)6U;
      uint64_t *y3 = b4 + (uint32_t)6U;
      x02[0U] = x02[0U] ^ y0[0U];
      x02[1U] = x02[1U] ^ y0[1U];
      x1[0U] = x1[0U] ^ y1[0U];
      x1[1U] = x1[1U] ^ y1[1U];
      x2[0U] = x2[0U] ^ y2[0U];
      x2[1U] = x2[1U] ^ y2[1U];
      x3[0U] = x3[0U] ^ y3[0U];
      x3[1U] = x3[1U] ^ y3[1U];
    }
    Hacl_Impl_Gf128_FieldPreComp_normalize4(acc0, acc4, pre);
  }
  uint32_t len1 = len - len0;
  uint8_t *t1 = text + len0;
  uint64_t *r10 = pre + (uint32_t)6U;
  uint32_t nb = len1 / (uint32_t)16U;
  uint32_t rem = len1 % (uint32_t)16U;
  for (uint32_t i = (uint32_t)0U; i < nb; i++)
  {
    uint8_t *tb = t1 + i * (uint32_t)16U;
    uint64_t elem[2U] = { 0U };
    uint64_t u = load64_be(tb);
    elem[1U] = u;
    uint64_t u0 = load64_be(tb + (uint32_t)8U);
    elem[0U] = u0;
    acc0[0U] = acc0[0U] ^ elem[0U];
    acc0[1U] = acc0[1U] ^ elem[1U];
    Hacl_Impl_Gf128_FieldPreComp_fmul(acc0, r10);
  }
  if (rem > (uint32_t)0U)
  {
    uint8_t *last = t1 + nb * (uint32_t)16U;
    uint64_t elem[2U] = { 0U };
    uint8_t b[16U] = { 0U };
    memcpy(b, last, rem * sizeof (uint8_t));
    uint64_t u = load64_be(b);
    elem[1U] = u;
    uint64_t u0 = load64_be(b + (uint32_t)8U);
    elem[0U] = u0;
    acc0[0U] = acc0[0U] ^ elem[0U];
    acc0[1U] = acc0[1U] ^ elem[1U];
    Hacl_Impl_Gf128_FieldPreComp_fmul(acc0, r10);
  }
  uint64_t *acc1 = ctx;
  uint64_t r0 = acc1[1U];
  uint64_t r1 = acc1[0U];
  store64_be(tag, r0);
  store64_be(tag + (uint32_t)8U, r1);
}

