#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int fp = 14; // fixed point position
typedef int fptype;

const int screenW_lowResFactor = 1; // lower res
const int screenW = 128 / screenW_lowResFactor;
const int around = 6 * screenW, aroundh = around / 2, aroundq = around / 4, around3q = 3 * aroundq; // FOV = 60 degs (6 FOVs = 360 degrees)

int Tan_fp[around]; // fp bits fixed point
int CTan_fp[around];


float X2Rad(int X) {
    return (float)X * 3.1415f / aroundh;
}

int main()
{
    int i, j;
    // precalculate
    for (int a = 0; a < around; a++) {
        float angf = X2Rad(a);

        // tangent (theoretical range is [-inf..+inf], in practice (-128..+128) is fine)
        fptype maxTan = (((fptype)128) << fp) - 1;
        float temp = tan(angf) * (1 << fp);
        Tan_fp[a] = (fptype)temp;
        if (temp > maxTan)
            Tan_fp[a] = maxTan;
        if (temp < -maxTan)
            Tan_fp[a] = -maxTan;

        // cotangent
        temp = 1 / tanf(angf) * (((fptype)1) << fp);
        CTan_fp[a] = (fptype)temp;
        if (temp > maxTan)
            CTan_fp[a] = maxTan;
        if (temp < -maxTan)
            CTan_fp[a] = -maxTan;
    }

    // save to file
    FILE* fp = fopen("..\\Maze3D_on_ArduinoNano\\LUTs.h", "w");
    if (!fp)
        printf("error");

    fprintf(fp, "const int32_t screenW_lowResFactor = %d;\n\n", screenW_lowResFactor);
    fprintf(fp, "const int32_t PROGMEM Tan_fp[] = {");
    for (int a = 0; a < around; a++) {
        if (a % 16 == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%10d, ", Tan_fp[a]);
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "const int32_t PROGMEM CTan_fp[] = {");
    for (int a = 0; a < around; a++) {
        if (a % 16 == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%10d, ", CTan_fp[a]);
    }
    fprintf(fp, "};\n");

    fclose(fp);

    return 0;
}
