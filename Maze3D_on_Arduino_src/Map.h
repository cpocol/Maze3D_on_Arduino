#define mapWidth 14
#define mapHeight 16

extern const int8_t Map[mapHeight][mapWidth];

// !not working with ROM, why?
//#define MAP(row, col) ((int8_t)pgm_read_byte_near(Map + row * mapWidth + col))
#define MAP(row, col) (Map[row][col])
