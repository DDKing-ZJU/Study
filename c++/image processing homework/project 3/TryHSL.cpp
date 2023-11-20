#include "BMPIO.hpp"

int main()
{
    FILE *Input = fopen("input.bmp", "rb");
    FILE *Output = fopen("output.bmp", "wb+");
    BMPMetric *BMP = new BMPMetric();
    ReadBMP(Input, BMP);
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            (*BMP)[i][j].Conv_HSL_RGB();
        }
    WriteBMP(BMP, Output);
    return 0;
}