#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#define TEXTURE_1bpp // 8bpp if not defined

const int pow2 = 5;
const int texRes = (1 << pow2);
char Texture[texRes * texRes];

int main()
{
    // generate texture
    for (int i = 0; i < texRes; i++)
        for (int j = 0; j < texRes; j++)
            *(Texture + i * texRes + j) = ((0.1 * texRes < i) && (i < 0.7 * texRes) && (0.2 * texRes < j) && (j < 0.8 * texRes)) ? 0 : 1;

    // save to file
    FILE* fp = fopen("..\\Maze3D_on_ArduinoNano_src\\Texture.h", "w");
    if (!fp)
        printf("error");

#ifdef TEXTURE_1bpp
    fprintf(fp, "#define TEXTURE_1bpp // 8bpp if not defined\n\n");
#endif
    fprintf(fp, "const int32_t texRes = (1 << %d);\n\n", pow2);
    fprintf(fp, "const uint8_t PROGMEM Texture[] = {");

#ifdef TEXTURE_1bpp
    // save as binary
    for (int i = 0; i < texRes * texRes;) {
        if (i % texRes == 0)
            fprintf(fp, "\n");
        int packetPixels = 0;
        for (int j = 0; j < 8; j++, i++)
            packetPixels |= (Texture[i] << j);

        char str[200] = "0b00000000";
        char str1[200];
        itoa(packetPixels, str1, 2);
        strcpy(str + 10 - strlen(str1), str1);

        fprintf(fp, "%s,", str);
    }
#else
    for (int i = 0; i < texRes * texRes; i++) {
        if (i % texRes == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%d, ", Texture[i]);
    }
#endif
    fprintf(fp, "\n};\n");
    fclose(fp);

    return 0;
}
