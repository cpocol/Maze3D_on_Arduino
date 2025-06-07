#define mapWidth 16
#define mapHeight 14

extern const int8_t Map[mapHeight][mapWidth];

// !not working with ROM, why?
//#define MAP(row, col) ((int8_t)pgm_read_byte_near(Map + int16_t(row) * mapWidth + col))
#define MAP(row, col) (Map[row][col])
