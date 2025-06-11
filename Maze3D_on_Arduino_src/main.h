#pragma once

#define sq(x) ((x)*(x))

typedef struct {
    union {
        fptype xHit_fp;
        struct {
            int16_t unused0x;
            int16_t x;
        };
        struct {
            int8_t unused0xMap;
            int8_t unused1xMap;
            int8_t unused2xMap;
            int8_t xMap;
        };
    };

    union {
        fptype yHit_fp;
        struct {
            int16_t unused0y;
            int16_t y;
        };
        struct {
            int8_t unused0yMap;
            int8_t unused1yMap;
            int8_t unused2yMap;
            int8_t yMap;
        };
    };

    bool horizontalWall;
} TCastResponse_fp;

typedef struct {
    union {
        int16_t xHit;
        struct {
            uint8_t xHit0;
            uint8_t xHit1;
        };
    };
    union {
        int16_t yHit;
        struct {
            uint8_t yHit0;
            uint8_t yHit1;
        };
    };
    bool horizontalWall;
} TCastResponse;

TCastResponse Cast(int16_t ang);
