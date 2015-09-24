#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct gifImage {
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t depth;
    struct LocalFlag {
        unsigned palBits: 3;
        unsigned reserved: 2;
        unsigned sortFlag: 1;
        unsigned interlace: 1;
        unsigned localPal: 1;
    } __attribute__((packed))localFlag;
} __attribute__((packed))GIFIMAGE;

typedef struct gifHeader {
    uint8_t signature[3];
    uint8_t versicon[3];
} GIFHEADER;

typedef struct gifScrDesc {
    uint16_t width;
    uint16_t depth;
    struct GlobalFlag {
        unsigned palBits: 3;
        unsigned sortFlag: 1;
        unsigned colorRes: 3;
        unsigned globalPal: 1;
    } __attribute__((packed))globalFlag;
    uint8_t backGround;
    uint8_t aspect;
} __attribute__((packed))GIFSCRDESC;

int bmp_init(size_t width, size_t height, char *bmpbuf)
{
    size_t width3 = (width + 3) & ~0x3;
    BITMAPFILEHEADER fhdr;
    memcpy(&fhdr.bfType, "BM", 2);
    fhdr.bfReserved1 = 0;
    fhdr.bfReserved2 = 0;
    fhdr.bfOffBits = sizeof(BITMAPFILEHEADER);
    fhdr.bfOffBits += sizeof(BITMAPINFOHEADER);
    fhdr.bfOffBits += 256 * 4;

    fhdr.bfSize = width3 * height + fhdr.bfOffBits;
    BITMAPINFOHEADER ihdr;
    ihdr.biSize = sizeof(ihdr);
    ihdr.biWidth = width;
    ihdr.biHeight = -height;
    ihdr.biPlanes = 1;
    ihdr.biBitCount = 8;
    ihdr.biCompression = 0;
    ihdr.biSizeImage = 0; //width3*height;
    ihdr.biXPelsPerMeter = 0x1075;
    ihdr.biYPelsPerMeter = 0x1075;
    ihdr.biClrUsed = 256;
    ihdr.biClrImportant = 256;
    memcpy(bmpbuf, &fhdr, sizeof(fhdr));
    memcpy(bmpbuf + sizeof(fhdr), &ihdr, sizeof(ihdr));
    return sizeof(fhdr) + sizeof(ihdr);
}

static char buffer[1024 * 1024 * 3];
static char output[1024 * 1024 * 3 * 3];

static size_t old = 0;
static size_t cmax = 0;
static size_t cclear = 0;
static char *outptr = output;

static size_t mask = 0;
static size_t bsize = 9;

static char *dictp[4096];
static size_t dictl[4096];

static const char *gifpath = "a.gif";

char *keepput = 0;

int lzw_init()
{
    int i;
    static char color[256];
    assert(cclear == 256);
    for (i = 0; i < cclear; i++) {
        dictp[i] = color + i;
        dictl[i] = 1;
        color[i] = i;
    }
    for (bsize = 0; cclear >> bsize; bsize++) {
        if (bsize > 8)
            printf("path: %s\n", gifpath);
        assert(bsize <= 8);
    }
    assert(bsize == 9);
    mask = (1 << bsize) - 1;
    cmax = cclear + 1;
    old = 0;
    keepput = outptr;
    return cmax;
}

int lzw_code(size_t code)
{
    if (code > cmax) {
        printf("%x %x\n", code, cmax);
    }
    assert(code <= cmax);
    char *oldptr = outptr;
    if (code < cmax) {
        size_t dl = dictl[code];
        memcpy(outptr, dictp[code], dl);
        dictl[cmax] = dictl[old] + 1;
        char *p = outptr - dictl[old];
        dictp[cmax] = p;
        outptr += dl;
    } else {
        size_t dl = dictl[old];
        memcpy(outptr, dictp[old], dl);
        outptr[dl] = *outptr;
        dictp[cmax] = outptr;
        dictl[cmax] = dl + 1;
        outptr += (dl + 1);
    }
#if 0
    while (oldptr < outptr)
        printf("%02x\n", 0xff & *oldptr++);
#endif
    old = code;
    return ++cmax;
}

int lzw_data(uint8_t *buff, size_t count)
{
    size_t cnt;
    uint8_t *sp = buff;
    size_t csize = *buff++;

    outptr = output;
    cclear = (1 << csize);
    lzw_init();

    size_t i;
    size_t code = 0;
    size_t avail_bits = 0;

    int outed = 1;
    while (cnt = *buff++) {
        for (i = 0; i < cnt; i++) {
            code |= (*buff++ << avail_bits);
            avail_bits += 8;
            while (avail_bits >= bsize) {
                size_t sc = mask & code;
                avail_bits -= bsize;
                code >>= bsize;

                //printf("%x %x \n", mask, bsize);
                if (sc == cclear) {
                    lzw_init();
                    outed = 0;
                    printf("clear: %p %d\n", outptr, bsize);
                    continue;
                }
                if (sc == cclear + 1) {
                    printf("lzw end\n");
                    break;
                }
                printf("%x\n", sc);
                if (outed == 0) {
                    outed = 1;
                }
                if (lzw_code(sc) > mask) {
                    if (bsize < 12) {
                        mask = mask * 2 + 1;
                        bsize++;
                    }
                }
            }
        }
    }
    return buff - sp;
}

static char defColormap[256 * 3];

int gifDump(const char *path);

int main(int argc, char *argv[])
{
    int k;
    for (k = 0; k < 256; k++) {
        defColormap[k * 3] = k;
        defColormap[k * 3 + 1] = k;
        defColormap[k * 3 + 2] = k;
    }
    for (k = 1; k < argc; k++) {
        gifpath = argv[k];
        gifDump(argv[k]);
    }
    return 0;
}

int gifDump(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return -1;
    }

    int count = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    GIFSCRDESC *desc = (GIFSCRDESC *)(buffer + 6);
    printf("width: %d\n", desc->width);
    printf("depth: %d\n", desc->depth);
    printf("palBits: %d\n", desc->globalFlag.palBits);

    unsigned palBits = desc->globalFlag.palBits;
    unsigned palBytes = desc->globalFlag.globalPal ? (1 << palBits) * 6 : 0;

    int off = palBytes + 6 + 7;

    printf("dumping start!\n");
    while (off < count) {
        size_t cnt;
        switch (0xFF & buffer[off++]) {
        case 0x21:
            switch (0xFF & buffer[off++]) {
            case 0xfe:
            case 0x01:
            case 0xff:
            case 0xf9:
                while (cnt = buffer[off++])
                    off += (cnt & 0xFF);
                break;
            default:
                printf("bad format!\n");
                exit(0);
                break;
            }
            break;
        case 0x2c:
            printf("Found Image!\n");
            GIFIMAGE *img = (GIFIMAGE *)(buffer + off);
            unsigned palBitsl = img->localFlag.palBits;
            unsigned palBytesl = img->localFlag.localPal ? (1 << palBitsl) * 6 : 0;
            char *word = (char *)(img + 1) + palBytesl;
            printf("width: %d\n", img->width);
            printf("depth: %d\n", img->depth);
            printf("localPal: %d\n", img->localFlag.localPal);
            off += lzw_data(word, buffer + count - word);
            off += sizeof(GIFIMAGE);
            off += palBytesl;
            {
                char outname[256];
                static size_t outindex = 0;
                sprintf(outname, "a-%d.bmp", outindex++);
                FILE *fp = fopen(outname, "wb");
                if (fp != NULL) {
                    size_t cc = bmp_init(img->width, img->depth, outname);
                    fwrite(outname, 1, cc, fp);

                    int i, j;
                    uint8_t *p = (uint8_t *)output;
                    char *colors = defColormap;
                    if (desc->globalFlag.globalPal)
                        colors = (char *)(buffer + 13);
                    if (img->localFlag.localPal)
                        colors = (char *)(img + 1);

                    char clr[4];
                    clr[3] = 0;
                    for (i = 0; i < 256; i++) {
                        clr[0] = colors[3 * i + 2];
                        clr[1] = colors[3 * i + 1];
                        clr[2] = colors[3 * i + 0];
                        fwrite(clr, 1, 4, fp);
                    }

                    char *pd = output;
                    size_t line = ((img->width + 3) & ~0x3);
                    while (pd < outptr) {
                        fwrite(pd, 1, line, fp);
                        pd += img->width;
                    }
                    fclose(fp);
                }
            }
            break;
        case 0x3b:
            printf("End Stream!\n");
            break;
        default:
            printf("unkown: %06x: %x\n", off - 1, buffer[off - 1] & 0xFF);
            break;
        }
    }
    return 0;
}



