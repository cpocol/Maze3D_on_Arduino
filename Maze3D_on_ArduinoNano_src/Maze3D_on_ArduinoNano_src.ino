#include "Adafruit_SSD1306.h"

#include "Config.h"
#include "Controller.h"
#include "LUTs.h"
#include "Map.h"
#include "Texture.h"

#define sq(x) ((x)*(x))

Adafruit_SSD1306 display(screenW, screenH);

const int screenSize = screenW * ((screenH + 7) / 8);
uint8_t screen[screenSize];

const int16_t mapSizeHeight = mapHeight * sqRes, mapSizeWidth = mapWidth * sqRes;
const fptype mapSizeWidth_fp = (((fptype)mapSizeWidth) << fp), mapSizeHeight_fp = (((fptype)mapSizeHeight) << fp);

//initial
int32_t xC = 2.5 * sqRes;
int32_t yC = 2.5 * sqRes;
int16_t angleC = 600;
int elevation_perc = 0; //as percentage from wall half height

float X2Rad(int X) {
    return (float)X * 3.1415f / aroundh;
}

void setup() {
    Serial.begin(250000);

    display.setBuffer(screen);
    if(!display.begin()) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    initController();
    Serial.print("Map size (W x H): ");   Serial.print(mapWidth);   Serial.print(" x ");   Serial.println(mapHeight);
}

int8_t map(fptype row, fptype col) {
    return pgm_read_byte_near(Map + row * mapWidth + col);
    //return Map[row][col];
}

//returns wall ID (as map position and cell face)
int32_t CastX(int16_t angle, fptype& xHit_fp, fptype& yHit_fp) { // hit vertical walls ||
    if ((angle == aroundq) || (angle == around3q))
        return -1; // CastY() will hit a wall correctly

    // prepare as for 1st or 4th quadrant
    fptype x = (xC / sqRes) * sqRes + sqRes;
    fptype dx = sqRes,   adjXMap = 0;
    fptype tan_fp = (fptype) pgm_read_dword_near(Tan_fp + angle / screenW_lowResFactor);
    fptype dy_fp = sqRes * tan_fp;
    // 2nd or 3rd quadrant
    if ((aroundq < angle) && (angle < around3q)) {
        x -= sqRes;
        adjXMap = -1;
        dx = -dx;
        dy_fp = -dy_fp;
    }
    yHit_fp = (((fptype)yC) << fp) + (x - xC) * tan_fp;

    while ((0 < x) && (x < mapSizeWidth) && (0 < yHit_fp) && (yHit_fp < mapSizeHeight_fp) &&
           (map((yHit_fp >> fp) / sqRes, x / sqRes + adjXMap) == 0)) {
        x += dx;
        yHit_fp += dy_fp;
    }

    xHit_fp = x << fp;

    return int32_t((yHit_fp / sqRes_fp) * mapWidth + (x / sqRes + adjXMap));
}

// returns wall ID (as map position and cell face)
int32_t CastY(int16_t angle, fptype& xHit_fp, fptype& yHit_fp) { // hit horizontal walls ==
    if ((angle == 0) || (angle == aroundh))
        return -1; // CastX() will hit a wall correctly

    // prepare as for 1st or 2nd quadrant
    fptype y = (yC / sqRes) * sqRes + sqRes;
    fptype dy = sqRes,   adjYMap = 0;
    fptype ctan_fp = (fptype) pgm_read_dword_near(CTan_fp + angle / screenW_lowResFactor);
    fptype dx_fp = sqRes * ctan_fp;
    if (angle > aroundh) { // 3rd or 4th quadrants
        y -= sqRes;
        adjYMap = -1;
        dy = -dy;
        dx_fp = -dx_fp;
    }
    xHit_fp = (((fptype)xC) << fp) + (y - yC) * ctan_fp;

    while ((0 < xHit_fp) && (xHit_fp < mapSizeWidth_fp) && (0 < y) && (y < mapSizeHeight) &&
           (map(y / sqRes + adjYMap, (xHit_fp >> fp) / sqRes) == 0)) {
        xHit_fp += dx_fp;
        y += dy;
    }

    yHit_fp = y << fp;

    return int32_t((y / sqRes + adjYMap) * mapWidth + (xHit_fp / sqRes_fp));
}

// returns wall ID (as map position and cell face)
int32_t Cast(int16_t angle, fptype& xHit, fptype& yHit) {
    fptype xX_fp = 1000000000, yX_fp = xX_fp, xY_fp = xX_fp, yY_fp = xX_fp;
    int32_t wallIDX = CastX(angle, xX_fp, yX_fp);
    int32_t wallIDY = CastY(angle, xY_fp, yY_fp);
    // choose the nearest hit point
    if (abs(((fptype)xC << fp) - xX_fp) < abs(((fptype)xC << fp) - xY_fp)) { // vertical wall ||
        xHit = fptype(xX_fp >> fp);
        yHit = fptype(yX_fp >> fp);
        return 2 * wallIDX + 0;
    }
    else { // horizontal wall ==
        xHit = fptype(xY_fp >> fp);
        yHit = fptype(yY_fp >> fp);
        return 2 * wallIDY + 1;
    }
}

void RenderColumn(int col, fptype h, int32_t textureColumn) {
    fptype Dh_fp = (texRes << 6) / h; // 1 row in screen space is this many rows in texture space; use fixed point
    fptype textureRow_fp = 0;
    //int minRow = screenHh - h / 2; // no elevation
    int minRow = ((100 - elevation_perc) * (2 * screenHh - h) / 2 + elevation_perc * screenHh) / 100;
    int maxRow = min(minRow + h, screenH);

    if (minRow < 0) { // clip
        textureRow_fp = -(minRow * Dh_fp);
        minRow = 0;
    }

    uint16_t textureOffsetInit = textureColumn * texRes; // huge speedup: 90 degs pre-rotated texture
    unsigned char* pScreen = screen + col;
    const unsigned char* pTexture = Texture + textureOffsetInit;
    static unsigned char texCol[texRes];
    memcpy_P(texCol, Texture + textureColumn * texRes, texRes); // prefetch the whole column
    for (int row = minRow; row < maxRow; row++) {
#ifdef TEXTURE_1bpp // configured in "Generate Texture.cpp"
        uint16_t textureOffset = textureOffsetInit + uint16_t(textureRow_fp >> 6);
        uint16_t textureOffsetByte = textureOffset / 8;
        uint8_t textureOffsetBit = uint8_t(textureOffset) % 8;

        //unsigned char pixelByte = *(Texture + textureOffsetByte);
        unsigned char pixelByte = pgm_read_byte_near(Texture + textureOffsetByte);
        unsigned char pixel = (pixelByte >> textureOffsetBit) & 1;
#else
        //unsigned char pixel = *(pTexture + uint16_t(textureRow_fp >> 6));
        //unsigned char pixel = pgm_read_byte_near(pTexture + uint16_t(textureRow_fp >> 6));
        //unsigned char pixel = 1;
        unsigned char pixel = texCol[uint16_t(textureRow_fp >> 6)];
#endif

        unsigned char* screenAddr = pScreen + (row / 8) * screenW;
        *screenAddr |= (pixel << (row & 7));

        textureRow_fp += Dh_fp;
    }
}

void Render() {
    static long t_prev = millis();
    static long t0 = millis();
    memset(screen, 0, screenSize);
    static long t1 = millis();   static int dt_clear = int(t1 - t0);   t0 = t1;

    uint16_t viewerToScreen_sq = sq(screenWh) * 3; // FOV = 60 degs => viewerToScreen = screenWh * sqrt(3)
    uint32_t textureColumn;
    for (int16_t col = 0; col < screenW; col++) {
        int16_t ang = (screenWh - col + angleC + around) % around;
        fptype xHit, yHit;
        Cast(ang, xHit, yHit);

        textureColumn = ((xHit + yHit) % sqRes) * texRes / sqRes;

        fptype dist_sq = sq(xC - xHit) + sq(yC - yHit) + 1; // +1 avoids division by zero
        fptype h = fptype(sqRes_f * sqrtf((viewerToScreen_sq + sq(screenWh - col)) / (float)dist_sq));
        h = h / 4; // adjust until it looks fine

        RenderColumn(col, h, textureColumn);
    }
    t1 = millis();   static int dt_render = int(t1 - t0);   t0 = t1;

    // mirror image; we need this because the map's CS is left handed while the ray casting works right handed
    for (int r = 0; r < (screenH + 7) / 8; r++)
        for (int col = 0; col < screenWh; col++) {
            int8_t aux = *(screen + r * screenW + col);
            *(screen + r * screenW + col) = *(screen + r * screenW + screenW - 1 - col);
            *(screen + r * screenW + screenW - 1 - col) = aux;
        }
    t1 = millis();   static int dt_mirror = int(t1 - t0);   t0 = t1;

    display.display();
    t1 = millis();   static int dt_flush = int(t1 - t0);   t0 = t1;

    static float FPS = 1000.f / (t1 - t_prev);
    Serial.print("FPS: ");         Serial.print(FPS);
    // Serial.print("   Clear: ");    Serial.print(dt_clear);
    Serial.print("   Render: ");   Serial.print(dt_render);
    //Serial.print("   Mirror: ");   Serial.print(dt_mirror);
    Serial.print("   Flush: ");    Serial.print(dt_flush);
    Serial.println("");
    t_prev = millis();
}

void loop() {
    Render();
    loopController(xC, yC, angleC, around);
}
