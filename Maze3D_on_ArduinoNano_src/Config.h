#pragma once

#define MOVE_SPD (10 * sqRes / 100)
#define ROTATE_SPD 3
#define VERTICAL_SPD (sqRes/25)

// bits of fptype >= (number of bits for sqRes) + fp + (1 bit for sign) + X
// where X = max((number of bits for mapSize), (number of bits for the integral part of tan/ctan))

const int fp = 14; // fixed point position
typedef int32_t fptype;

const int16_t screenW = 128, screenH = 64, screenWh = screenW / 2, screenHh = screenH / 2;
const int16_t around = 6 * screenW, aroundh = around / 2, aroundq = around / 4, around3q = 3 * aroundq; // FOV = 60 degs (6 FOVs = 360 degrees)

const int sqRes_pow2 = 10;
const int sqRes = (1 << sqRes_pow2), sqResh = sqRes / 2;
const float sqRes_f = float(sqRes);
const fptype sqRes_fp = (fptype)sqRes << fp;
const int safety_dist = 3; // to wall

// viewer Current position, orientation and elevation [%]
extern int32_t xC, yC;
extern int16_t angleC;
extern int elevation_perc; // percentage
