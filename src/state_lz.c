#ifdef LITE
#include <stdint.h>
#include <stddef.h>
#include "state_lz.h"

/*
* stream format, a sequence of tokens:
*   0x00-0x7F   literal run, token + 1 literal bytes follow
*   0x80-0xFF   match, followed by a 16 bit big endian distance (1-65535).
*               length = (token & 0x7F) + LZ_MIN_MATCH.  when (token & 0x7F)
*               is 0x7F, extension bytes follow, each is added to the length,
*               and another extension byte follows when one is 0xFF.
*/

#define LZ_MIN_MATCH    3
#define LZ_MAX_LIT      0x80
#define LZ_MAX_DIST     0xFFFF
#define LZ_LEN_EXT      0x7F

static inline uint32_t lz_hash(const uint8_t *p) {
    uint32_t v = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
    return (v * 2654435761u) >> (32 - LZ_HASH_BITS);
}

static inline int lz_put(lz_stream_t *out, uint8_t b) {
    if(out->p == out->end && !out->next(out)) {
        return 0;
    }
    *out->p++ = b;
    return 1;
}

static inline uint8_t lz_get(lz_stream_t *in) {
    if(in->p == in->end) {
        in->next(in);
    }
    return *in->p++;
}

static int lz_emit_literals(lz_stream_t *out, const uint8_t *lit, size_t cnt) {
    while(cnt > 0) {
        size_t run = cnt > LZ_MAX_LIT ? LZ_MAX_LIT : cnt;
        if(!lz_put(out, run - 1)) {
            return 0;
        }
        for(size_t i = 0; i < run; i++) {
            if(!lz_put(out, lit[i])) {
                return 0;
            }
        }
        lit += run;
        cnt -= run;
    }
    return 1;
}

static int lz_emit_match(lz_stream_t *out, size_t dist, size_t len) {
    len -= LZ_MIN_MATCH;
    if(!lz_put(out, 0x80 | (len >= LZ_LEN_EXT ? LZ_LEN_EXT : len)) ||
       !lz_put(out, dist >> 8) || !lz_put(out, dist)) {
        return 0;
    }
    if(len >= LZ_LEN_EXT) {
        len -= LZ_LEN_EXT;
        while(len >= 0xFF) {
            if(!lz_put(out, 0xFF)) {
                return 0;
            }
            len -= 0xFF;
        }
        if(!lz_put(out, len)) {
            return 0;
        }
    }
    return 1;
}

int lz_compress(lz_stream_t *out, const void *src, size_t len, lz_hash_t *hash) {
    const uint8_t *start = src;
    const uint8_t *ip = start;
    const uint8_t *end = start + len;
    const uint8_t *lit = start;

    if(len >= LZ_MIN_MATCH) {
        const uint8_t *match_end = end - (LZ_MIN_MATCH - 1);
        while(ip < match_end) {
            uint32_t h = lz_hash(ip);
            const uint8_t *ref = hash[h];
            hash[h] = ip;

            /* entries left over from other chunks are rejected by the range check */
            if(ref < start || ref >= ip || ip - ref > LZ_MAX_DIST ||
               ref[0] != ip[0] || ref[1] != ip[1] || ref[2] != ip[2]) {
                ip++;
                continue;
            }

            size_t max = end - ip;
            size_t mlen = LZ_MIN_MATCH;
            while(mlen < max && ref[mlen] == ip[mlen]) {
                mlen++;
            }

            if(!lz_emit_literals(out, lit, ip - lit) || !lz_emit_match(out, ip - ref, mlen)) {
                return 0;
            }

            ip += mlen;
            lit = ip;

            /* keep the table fresh for data following a long match */
            if(ip < match_end) {
                hash[lz_hash(ip - 1)] = ip - 1;
            }
        }
    }

    return lz_emit_literals(out, lit, end - lit);
}

void lz_decompress(lz_stream_t *in, void *dst, size_t len) {
    uint8_t *op = dst;
    uint8_t *end = op + len;

    while(op < end) {
        uint8_t token = lz_get(in);
        if(token < 0x80) {
            size_t run = token + 1;
            for(size_t i = 0; i < run; i++) {
                *op++ = lz_get(in);
            }
        } else {
            size_t mlen = token & LZ_LEN_EXT;
            size_t dist = lz_get(in) << 8;
            dist |= lz_get(in);
            if(mlen == LZ_LEN_EXT) {
                uint8_t ext;
                do {
                    ext = lz_get(in);
                    mlen += ext;
                } while(ext == 0xFF);
            }
            mlen += LZ_MIN_MATCH;

            /* byte by byte so overlapping matches act as run length encoding */
            const uint8_t *ref = op - dist;
            for(size_t i = 0; i < mlen; i++) {
                *op++ = *ref++;
            }
        }
    }
}

#endif
