#pragma once

#define sq(x) ((x)*(x))

int32_t Cast(int16_t ang, fptype& xHit, fptype& yHit);

// !not working with ROM, why?
//#define map(row, col) ((int8_t)pgm_read_byte_near(Map + row * mapWidth + col))
#define map(row, col) (Map[row][col])
