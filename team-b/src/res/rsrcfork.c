/*
 * res/rsrcfork.c — Apple Mac Resource Fork parser 實作
 *
 * 規格來源：spec 03 §3.3。所有多 byte 整數 big-endian。
 *
 * file header (offset 0):
 *   u32 data_offset    (一律 0x00000100)
 *   u32 map_offset
 *   u32 data_length
 *   u32 map_length
 *   240 B reserved
 *
 * data section (從 data_offset 起): 每個 resource 為
 *   u32 entry_size
 *   N B bytes
 *
 * resource map (從 map_offset 起):
 *   16 B copy of file header (忽略)
 *   u32 next_map_handle / u16 file_ref / u16 attributes
 *   u16 type_list_offset    (相對 map_offset)
 *   u16 name_list_offset    (相對 map_offset)
 *
 * type list (從 map_offset + type_list_offset 起):
 *   u16 (n_types - 1)
 *   每個 entry 8 bytes:
 *     u32 fourCC
 *     u16 (n_refs - 1)
 *     u16 ref_list_offset (相對 type_list 起點 = map_offset + type_list_offset)
 *
 * ref list (從 type_list_offset_anchor + ref_list_offset 起):
 *   每個 entry 12 bytes:
 *     u16 id
 *     i16 name_list_offset (相對 map_offset + name_list_offset；-1 = 無名)
 *     u32 (attributes:8 | data_offset:24)  (data_offset 相對 data section 起點)
 *     u32 handle (in-memory，忽略)
 */
#include "rsrcfork.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* big-endian load helpers */
static inline uint16_t be16(const uint8_t *p)
{
    return (uint16_t)p[0] << 8 | p[1];
}

static inline int16_t be16s(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] << 8 | p[1]);
}

static inline uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
           (uint32_t)p[2] <<  8 | (uint32_t)p[3];
}

struct civ_rsrc {
    uint8_t            *file_buf;     /* 整個 .RSC bytes，own */
    size_t              file_len;
    civ_rsrc_entry_t   *entries;      /* 所有 resource entry */
    char              **name_buf;     /* 對應 entries 的 name 存放（NULL 表沒名）*/
    size_t              n_entries;
};

static int parse_fork(civ_rsrc_t *r)
{
    const uint8_t *d = r->file_buf;
    if (r->file_len < 0x100) return -1;

    uint32_t data_off = be32(d + 0);
    uint32_t map_off  = be32(d + 4);
    uint32_t data_len = be32(d + 8);
    uint32_t map_len  = be32(d + 12);
    if ((uint64_t)map_off + map_len > r->file_len)  return -1;
    if ((uint64_t)data_off + data_len > r->file_len) return -1;

    /* map header — 跳過前 24 byte（file header copy + next/ref/attr） */
    uint16_t type_list_off_rel = be16(d + map_off + 0x18);
    uint16_t name_list_off_rel = be16(d + map_off + 0x1A);
    size_t   type_list_off     = map_off + type_list_off_rel;
    size_t   name_list_off     = map_off + name_list_off_rel;
    if (type_list_off + 2 > r->file_len) return -1;

    uint16_t n_types = (uint16_t)(be16(d + type_list_off) + 1);

    /* 第一輪：算總 entry 數 */
    size_t total = 0;
    for (uint16_t i = 0; i < n_types; i++) {
        size_t e = type_list_off + 2 + (size_t)i * 8;
        if (e + 8 > r->file_len) return -1;
        uint16_t n_refs = (uint16_t)(be16(d + e + 4) + 1);
        total += n_refs;
    }

    r->entries  = calloc(total, sizeof *r->entries);
    r->name_buf = calloc(total, sizeof *r->name_buf);
    if (!r->entries || !r->name_buf) return -1;
    r->n_entries = 0;

    /* 第二輪：填入每個 entry */
    for (uint16_t i = 0; i < n_types; i++) {
        size_t e = type_list_off + 2 + (size_t)i * 8;
        uint32_t fourcc = be32(d + e);
        uint16_t n_refs = (uint16_t)(be16(d + e + 4) + 1);
        uint16_t ref_off_rel = be16(d + e + 6);
        size_t   ref_off = type_list_off + ref_off_rel;

        for (uint16_t j = 0; j < n_refs; j++) {
            size_t rp = ref_off + (size_t)j * 12;
            if (rp + 12 > r->file_len) return -1;

            uint16_t id = be16(d + rp);
            int16_t  name_off_rel = be16s(d + rp + 2);
            uint32_t packed = be32(d + rp + 4);
            uint32_t d_rel = packed & 0x00FFFFFF;

            size_t entry_start = data_off + d_rel;
            if (entry_start + 4 > r->file_len) return -1;
            uint32_t entry_size = be32(d + entry_start);
            if (entry_start + 4 + entry_size > r->file_len) return -1;

            civ_rsrc_entry_t *ent = &r->entries[r->n_entries];
            ent->type = fourcc;
            ent->id   = (int16_t)id;
            ent->data = d + entry_start + 4;
            ent->len  = entry_size;

            if (name_off_rel >= 0) {
                size_t np = name_list_off + (size_t)name_off_rel;
                if (np < r->file_len) {
                    uint8_t nlen = d[np];
                    if (np + 1 + nlen <= r->file_len) {
                        char *buf = malloc((size_t)nlen + 1);
                        if (buf) {
                            memcpy(buf, d + np + 1, nlen);
                            buf[nlen] = 0;
                            r->name_buf[r->n_entries] = buf;
                            ent->name = buf;
                        }
                    }
                }
            }
            r->n_entries++;
        }
    }
    return 0;
}

civ_rsrc_t *civ_rsrc_open(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0 || sz > (long)(64 * 1024 * 1024)) { fclose(f); return NULL; }
    rewind(f);

    civ_rsrc_t *r = calloc(1, sizeof *r);
    if (!r) { fclose(f); return NULL; }
    r->file_buf = malloc((size_t)sz);
    if (!r->file_buf) { free(r); fclose(f); return NULL; }
    if (fread(r->file_buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f); civ_rsrc_close(r); return NULL;
    }
    r->file_len = (size_t)sz;
    fclose(f);

    if (parse_fork(r) != 0) { civ_rsrc_close(r); return NULL; }
    return r;
}

void civ_rsrc_close(civ_rsrc_t *r)
{
    if (!r) return;
    if (r->name_buf) {
        for (size_t i = 0; i < r->n_entries; i++) free(r->name_buf[i]);
        free(r->name_buf);
    }
    free(r->entries);
    free(r->file_buf);
    free(r);
}

size_t civ_rsrc_count(const civ_rsrc_t *r, uint32_t type)
{
    if (!r) return 0;
    size_t n = 0;
    for (size_t i = 0; i < r->n_entries; i++) {
        if (r->entries[i].type == type) n++;
    }
    return n;
}

const civ_rsrc_entry_t *
civ_rsrc_find(const civ_rsrc_t *r, uint32_t type, int16_t id)
{
    if (!r) return NULL;
    for (size_t i = 0; i < r->n_entries; i++) {
        if (r->entries[i].type == type && r->entries[i].id == id)
            return &r->entries[i];
    }
    return NULL;
}

const civ_rsrc_entry_t *
civ_rsrc_iter(const civ_rsrc_t *r, uint32_t type, size_t idx)
{
    if (!r) return NULL;
    size_t hit = 0;
    for (size_t i = 0; i < r->n_entries; i++) {
        if (r->entries[i].type == type) {
            if (hit == idx) return &r->entries[i];
            hit++;
        }
    }
    return NULL;
}
