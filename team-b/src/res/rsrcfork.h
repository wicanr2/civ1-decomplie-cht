/*
 * res/rsrcfork.h — Apple Mac Resource Fork parser
 *
 * 對應 spec 03 §3.3 容器格式。1993 Windows CIV.EXE 用 5 個
 * Civdata*.RSC 攜帶資產，本身就是 Mac Resource Fork (big-endian)，
 * 對應原版 source `resmgr.c::OpenResFile` (Mac Resource Manager port)。
 *
 * 本層只暴露「以 fourCC + id 取得 payload」介面，所有 endian / offset
 * 計算封裝在 .c 內。Team B 不模仿 Mac Handle / NewPtr 抽象 — payload
 * 是 plain pointer，lifetime 綁在 civ_rsrc_t 上。
 */
#ifndef CIV_RES_RSRCFORK_H
#define CIV_RES_RSRCFORK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 把 4 個 char literal 合成 32-bit big-endian fourCC，方便比對。
 * 範例：CIV_FOURCC('C','v','P','c') == 0x43765063 */
#define CIV_FOURCC(a,b,c,d) \
    ((uint32_t)(a) << 24 | (uint32_t)(b) << 16 | \
     (uint32_t)(c) <<  8 | (uint32_t)(d))

typedef struct civ_rsrc civ_rsrc_t;

typedef struct civ_rsrc_entry {
    uint32_t       type;        /* fourCC big-endian */
    int16_t        id;
    const char    *name;        /* optional, NULL 表示沒名字 */
    const uint8_t *data;        /* payload；lifetime 綁 civ_rsrc_t */
    size_t         len;
} civ_rsrc_entry_t;

/* 載入 .RSC；失敗回 NULL（不印錯誤）。 */
civ_rsrc_t *civ_rsrc_open(const char *path);

void civ_rsrc_close(civ_rsrc_t *r);

/* 該 type 內的 resource 數量；type 不存在回 0。 */
size_t civ_rsrc_count(const civ_rsrc_t *r, uint32_t type);

/* 按 (type, id) 找；找不到回 NULL。 */
const civ_rsrc_entry_t *
civ_rsrc_find(const civ_rsrc_t *r, uint32_t type, int16_t id);

/* 按 idx 迭代該 type 內的 resource；idx 超界回 NULL。 */
const civ_rsrc_entry_t *
civ_rsrc_iter(const civ_rsrc_t *r, uint32_t type, size_t idx);

#endif /* CIV_RES_RSRCFORK_H */
