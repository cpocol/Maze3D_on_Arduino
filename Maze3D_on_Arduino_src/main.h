#pragma once

#define sq(x) ((x)*(x))

typedef struct {
    fptype xHit_fp, yHit_fp;
    int16_t xMap, yMap;
    bool horizontalWall;
} TCastResponse_fp;

typedef struct {
    int16_t xHit, yHit;
    int16_t xMap, yMap;
    bool horizontalWall;
} TCastResponse;

TCastResponse Cast(int16_t ang);
