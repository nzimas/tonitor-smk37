/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* The SMK-37 Pro's flash map: which ranges the RAM flash driver (fm1_flash.h) may erase / program.
 * After Felucca's smk37 branch (genkigenki/Felucca b67312c, hal/fm1_flash_map.h), with Tonitor's own windows.
 * Macros and compile-time checks only, so the host tests can include it.
 * Hardware notes: https://github.com/genkigenki/fm1-to-smk37 (docs/01-hardware.md); docs/smk37/research.md.
 *
 *   0x00000-0x03FFF  head (SPL, isd_config)                       NEVER
 *   0x04000-0x9CFFF  app area (stock PRCT end)                    update loader only (FL_APP)
 *   0x9D000-0xBAFFF  VM: unused by Tonitor                        (the loader's record window, FL_REC)
 *   0xBB000-0xBFFFF  VM: update loader staging + record 0xBFF00   FL_OTA (ota.c)
 *   0xC0000-0xC0FFF  BTIF (Bluetooth pairing)                     NEVER
 *   0xC1000-0xC1FFF  USRTRIM (factory key, pad and wheel calibration, "trimdat")   NEVER
 *   0xC2000-0xE9FFF  USRFLASH: the sessions' store, 40 sectors    FL_SES (sessions.c)
 *   0xEA000-0xEAFFF  USRFLASH tail: the stock end marker "usrdata" at 0xEAFF8; erased once before the first
 *                    write into FL_SES (FL_STOCKTAG), so a return to stock rebuilds its settings
 *   0xEB000-0xF3FFF  unlisted: unused
 *   0xF4000-0xFDFFF  USR (stock resources, for a return to stock) NEVER
 *   0xFE000-0xFEFFF  unused
 *   0xFF000-0xFFFFF  key_mac                                      NEVER
 * Everything below is an explicit allow-list; the asserts keep each allowed window out of the protected ranges
 * at compile time, and FL_GUARD checks them again inside the RAM driver. */
#pragma once
#include <stdint.h>

/* [off, off + n) inside [lo, hi), without wrapping: off + n can overflow, and
 * the 1 MiB part ignores the high address bits, so a wrapped range lands low. */
#define FL_IN(off, n, lo, hi) ((uint32_t)(off) >= (lo) && (uint32_t)(off) <= (hi) && \
                               (uint32_t)(n) <= (hi) - (uint32_t)(off))
/* the SMK-37 Pro's part (JEDEC 0xEB6014, read over UBOOT by the kit's author); the FM-1's 0x856014 kept, as
 * Felucca does: the driver uses only generic SPI NOR commands (9F, 05/35, 06, 20, 02, 0B) */
#define FL_JEDEC_OK(id) ((id) == 0xEB6014u || (id) == 0x856014u)

#define FL_HEAD_HI      0x00004000u                /* protected: [0, FL_HEAD_HI) */
#define FL_CAL_LO       0x000C0000u                /* protected: BTIF + USRTRIM */
#define FL_CAL_HI       0x000C2000u
#define FL_USRRES_LO    0x000F4000u                /* protected: stock USR resources */
#define FL_USRRES_HI    0x000FE000u
#define FL_KEYMAC_LO    0x000FF000u                /* protected: key_mac and above */
#define FL_APP_LO       0x00004000u                /* the update loader: app area */
#define FL_APP_HI       0x0009D000u
#define FL_REC_LO       0x0009D000u                /* the update loader: where it may drop records (VM only) */
#define FL_REC_HI       0x000C0000u
#define FL_PLAIN_LO     0x0009D000u                /* plain (unencrypted) XIP window from the VM start */
#define FL_OTA_LO       0x000BB000u                /* update loader staging, ota.c */
#define FL_OTA_HI       0x000C0000u
#define FL_SES_LO       0x000C2000u                /* the sessions' store (sessions.c) */
#define FL_SES_HI       0x000EA000u
#define FL_STOCKTAG_LO  0x000EA000u
#define FL_STOCKTAG_HI  0x000EB000u
#define FL_STOCKTAG_OFF 0x000EAFF8u
#define FL_STOCKTAG_OK(off, n) ((uint32_t)(off) == FL_STOCKTAG_LO && (uint32_t)(n) == FL_STOCKTAG_HI - FL_STOCKTAG_LO)
/* a window [lo, hi) is allowed: sector aligned, non-empty, and clear of every protected range */
#define FL_WIN_SAFE(lo, hi) ((lo) % 0x1000u == 0u && (hi) % 0x1000u == 0u && (lo) < (hi) && \
                             (lo) >= FL_HEAD_HI && (hi) <= FL_KEYMAC_LO && \
                             ((hi) <= FL_CAL_LO || (lo) >= FL_CAL_HI) && \
                             ((hi) <= FL_USRRES_LO || (lo) >= FL_USRRES_HI))
_Static_assert(FL_WIN_SAFE(FL_OTA_LO, FL_OTA_HI), "SMK: FL_OTA overlaps a protected range");
_Static_assert(FL_WIN_SAFE(FL_SES_LO, FL_SES_HI), "SMK: FL_SES overlaps a protected range");
_Static_assert(FL_WIN_SAFE(FL_STOCKTAG_LO, FL_STOCKTAG_HI) && FL_STOCKTAG_LO == FL_SES_HI &&
               FL_STOCKTAG_OFF + 8u == FL_STOCKTAG_HI, "SMK: the stock marker sector");
_Static_assert(FL_WIN_SAFE(FL_APP_LO, FL_APP_HI), "SMK: FL_APP overlaps a protected range");
_Static_assert(FL_WIN_SAFE(FL_REC_LO, FL_REC_HI), "SMK: FL_REC overlaps a protected range");
_Static_assert(FL_REC_HI == FL_CAL_LO && FL_OTA_HI <= FL_REC_HI && FL_OTA_LO >= FL_REC_LO,
               "SMK: the update record (OTA area) must lie in the loader's record window, below BTIF");
_Static_assert(FL_SES_LO >= FL_REC_HI, "SMK: the sessions must lie outside the loader's record window");
_Static_assert(FL_APP_HI <= FL_REC_LO && FL_PLAIN_LO <= FL_OTA_LO, "SMK: windows overlap");
/* the update loader (loader.c): the app area, and records only in the VM below BTIF */
#define FL_LDR_RANGE_OK(off, n) (FL_IN(off, n, FL_APP_LO, FL_APP_HI) || FL_IN(off, n, FL_REC_LO, FL_REC_HI))
/* Tonitor's own store */
#define FL_STORE_OK(off, n) FL_IN(off, n, FL_SES_LO, FL_SES_HI)
#ifndef FL_RANGE_OK
#define FL_RANGE_OK(off, n) (FL_STORE_OK(off, n) || FL_IN(off, n, FL_OTA_LO, FL_OTA_HI) || FL_STOCKTAG_OK(off, n))
#endif
/* second, independent net in the RAM driver: [off, off + n) touches a protected range (also when a
 * caller supplies its own FL_RANGE_OK, as the update loader does) */
#define FL_HITS(off, n, lo, hi) ((uint32_t)(off) < (hi) && \
                                 ((uint32_t)(off) >= (lo) || (uint32_t)(n) > (lo) - (uint32_t)(off)))
#define FL_PROTECTED(off, n) (FL_HITS(off, n, 0u, FL_HEAD_HI) || FL_HITS(off, n, FL_CAL_LO, FL_CAL_HI) || \
                              FL_HITS(off, n, FL_USRRES_LO, FL_USRRES_HI) || (uint32_t)(off) >= FL_KEYMAC_LO || \
                              (uint32_t)(n) > FL_KEYMAC_LO - (uint32_t)(off))
#define FL_GUARD(off, n) (FL_RANGE_OK(off, n) && !FL_PROTECTED(off, n))
