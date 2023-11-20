#include "BMPIO.hpp"
#include <cmath>
#include <string.h>
#include <stdio.h>

#define STRIP 10000

int main()
{
    // Step1: read an BMP
    FILE *Input = fopen("input.bmp", "rb");
    BMPMetric *BMP = new BMPMetric();
    ReadBMP(Input, BMP); // In the mean time, HSL is converted
    // Step2: determine the minimum and maximum lightness
    double MaxL = 0, MinL = 1;
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            double Lx = (*BMP)[i][j].L();
            MaxL = MAX(Lx, MaxL);
            MinL = MIN(Lx, MinL);
        }
    // Step3: Change the lightness of the bitmap by traversing pixels
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            (*BMP)[i][j].L() = (log((*BMP)[i][j].L() + 1) / log(MaxL + 1));
            (*BMP)[i][j].Conv_HSL_RGB();
        }
    // Step4: Output the Bitmap
    FILE *Output1 = fopen("Output1.bmp", "wb+");
    WriteBMP(BMP, Output1);
    // Now we finished logarithmic operation
    // Step 5: determine the distribution of lightness
    // since the lightness is distributed between [0,1]
    // So we divide the lightness into [STRIP] parts.
    long long *LD = new long long[STRIP + 1];
    memset(LD, 0, sizeof(long long[STRIP + 1]));
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            Pixel pixel = (*BMP)[i][j];
            int index = round(pixel.L() * STRIP);
            LD[index]++;
        }
    // Step6: calculate the transformation map
    // s_k = sum{p_i}, i = 0,1,...,k
    double *L_map = new double[STRIP + 1];
    long long N = BMPInfoHeader.biWidth * BMPInfoHeader.biHeight;
    L_map[0] = (double)LD[0] / N;
    for (int Light = 1; Light <= STRIP; Light++)
    {
        L_map[Light] = L_map[Light - 1] + (double)LD[Light] / N;
    }
    // Step7: traverse the bitmap and change the lightness according to the map
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            int index = round((*BMP)[i][j].L() * STRIP);
            (*BMP)[i][j].L() = L_map[index];
            (*BMP)[i][j].Conv_HSL_RGB(); // turn back to RGB
        }
    // Step8: output the bitmap
    FILE *Output2 = fopen("Output2.bmp", "wb+");
    WriteBMP(BMP, Output2);
    // finish the histogram equalization
    return 0;
}