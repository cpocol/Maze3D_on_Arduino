#pragma once

#define MOVE_SPD (0.1f * sqRes)
#define ROTATE_SPD 3
#define VERTICAL_SPD (sqRes/25)

// bits of fptype >= (number of bits for sqRes) + fp + (1 bit for sign) + X
// where X = max((number of bits for mapSize), (number of bits for the integral part of tan/ctan))

const int fp = 14; // fixed point position
typedef int32_t fptype;

const int16_t screenW = 128, screenH = 64, screenWh = screenW / 2, screenHh = screenH / 2;
const int16_t around = 6 * screenW, aroundh = around / 2, aroundq = around / 4, around3q = 3 * aroundq; // FOV = 60 degs (6 FOVs = 360 degrees)

const int8_t sqRes_pow2 = 8;
const int sqRes = (1 << (int)sqRes_pow2), sqResh = sqRes / 2;
const int sqRes_LSBmask = 0b11111111;
const float sqRes_f = float(sqRes);
const fptype sqRes_fp = (fptype)sqRes << fp;
const int safety_dist = 30; // to wall

const uint16_t viewerToScreen_sq = screenWh * screenWh * 3; // FOV = 60 degs => viewerToScreen = screenWh * sqrt(3)

// viewer Current position, orientation and elevation [%]
extern int32_t xC, yC;
extern int16_t angleC;
extern int elevation_perc; // percentage
