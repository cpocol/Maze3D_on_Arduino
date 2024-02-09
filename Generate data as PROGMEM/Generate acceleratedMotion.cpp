#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// !!! sqRes must be the same as in the Arduino project !!!

const int sqRes = (1 << 10), sqResh = sqRes / 2;

float fFPS = 30; // approximate FPS

int acceleratedMotion[200];
int maxJumpHeight = int(1.2 * sqResh); // jump this high
int maxJump_idx;

int main()
{
    // generate
    float fDist = 0, fSpeed = 0, G = 10.f * sqRes; // G was empirically chosen as we don't have a proper world scale here
    for (int i = 0; i < 200; i++) {
        acceleratedMotion[i] = (int)fDist;

        fSpeed += G / fFPS;
        fDist += fSpeed / fFPS;
    }

    // search for the acceleratedMotion entry so that we'll decelerate to zero speed at max jump height
    for (maxJump_idx = 0; maxJump_idx < 200; maxJump_idx++)
        if (acceleratedMotion[maxJump_idx] > maxJumpHeight)
            break;

    if (maxJump_idx >= 200) maxJump_idx = 199;

    // save to file
    FILE* fp = fopen("..\\Maze3D_on_ArduinoNano_src\\acceleratedMotion._cpp", "w");
    if (!fp)
        printf("error");

    fprintf(fp, "int maxJumpHeight = int(1.2 * sqResh); // jump this high\n");
    fprintf(fp, "int maxJump_idx = %d;\n", maxJump_idx);
    fprintf(fp, "const uint16_t PROGMEM acceleratedMotion[] = {");

    for (int i = 0; i < 200; i++) {
        if (i % 16 == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%7d, ", acceleratedMotion[i]);
    }
    fprintf(fp, "\n};\n");
    fclose(fp);

    return 0;
}
