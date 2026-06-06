#include "strings.h"

#include <stdlib.h>
#include <string.h>

int civ_strlist_parse(const uint8_t *payload, size_t len,
                      int16_t id, civ_strlist_t *out)
{
    if (!payload || !out || len < 2) return -1;
    int n = ((int)payload[0] << 8) | payload[1];  /* BE16 count */
    if (n < 0 || n > 4096) return -1;

    /* 兩 pass：第一 pass 算總長 + 算 count */
    size_t p = 2;
    size_t blob_len = 0;
    for (int i = 0; i < n; i++) {
        if (p >= len) return -1;
        size_t L = payload[p++];
        if (p + L > len) return -1;
        blob_len += L + 1;   /* +1 for null terminator */
        p += L;
    }

    out->id = id;
    out->count = n;
    out->blob = malloc(blob_len > 0 ? blob_len : 1);
    out->offsets = malloc((size_t)n * sizeof *out->offsets);
    if (!out->blob || !out->offsets) {
        free(out->blob);  free(out->offsets);
        out->blob = NULL; out->offsets = NULL;
        return -1;
    }

    /* 第二 pass：複製 + 加 null term */
    p = 2;
    size_t bp = 0;
    for (int i = 0; i < n; i++) {
        size_t L = payload[p++];
        out->offsets[i] = bp;
        memcpy(out->blob + bp, payload + p, L);
        out->blob[bp + L] = '\0';
        bp += L + 1;
        p += L;
    }
    return 0;
}

void civ_strlist_free(civ_strlist_t *s)
{
    if (!s) return;
    free(s->blob);
    free(s->offsets);
    s->blob = NULL;
    s->offsets = NULL;
    s->count = 0;
}

const char *civ_strlist_get(const civ_strlist_t *s, int index)
{
    if (!s || index < 0 || index >= s->count) return NULL;
    return s->blob + s->offsets[index];
}
