#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>

#define deltaY 100

using std::cin;
using std::cout;
using std::endl;
using std::vector;

// 1x3 matrix
class Pixel
{
public:
    unsigned char Bytes[3];
    double YUV[3];
    void ConvertRGB()
    {
        double buf1, buf2, buf3;
        buf1 = 1.0000 * YUV[0] + 1.1398 * YUV[2];
        buf2 = 0.9996 * YUV[0] - 0.3954 * YUV[1] - 0.5805 * YUV[2];
        buf3 = 1.0020 * YUV[0] + 2.0361 * YUV[1] - 0.0005 * YUV[2];
        if (buf1 > 255)
            buf1 = 255;
        if (buf2 > 255)
            buf2 = 255;
        if (buf3 > 255)
            buf3 = 255;
        Bytes[0] = round(buf1);
        Bytes[1] = round(buf2);
        Bytes[2] = round(buf3);
    }
    void ConvertYUV()
    {
        double buf1, buf2, buf3;
        buf1 = 0.299 * Bytes[0] + 0.587 * Bytes[1] + 0.114 * Bytes[2];
        buf2 = -0.147 * Bytes[0] - 0.289 * Bytes[1] + 0.435 * Bytes[2];
        buf3 = 0.615 * Bytes[0] - 0.515 * Bytes[1] - 0.100 * Bytes[2];
        YUV[0] = buf1;
        YUV[1] = buf2;
        YUV[2] = buf3;
    }
};

class BMPHeader
{
public:
    unsigned char bfType[2];
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BMPHeader;

class BMPInfoHeader
{
public:
    unsigned int biSize;
    unsigned int biWidth;
    unsigned int biHeight;
    unsigned short int biPlanes;
    unsigned short int biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BMPInfoHeader;

void ReadHeaders(FILE *Input)
{
    fread(&BMPHeader.bfType, sizeof(char), 2, Input);
    fread(&BMPHeader.bfSize, sizeof(int), 1, Input);
    fread(&BMPHeader.bfReserved1, sizeof(short), 1, Input);
    fread(&BMPHeader.bfReserved2, sizeof(short), 1, Input);
    fread(&BMPHeader.bfOffBits, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biSize, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biWidth, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biHeight, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biPlanes, sizeof(short), 1, Input);
    fread(&BMPInfoHeader.biBitCount, sizeof(short), 1, Input);
    fread(&BMPInfoHeader.biCompression, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biSizeImage, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biXPelsPerMeter, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biYPelsPerMeter, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biClrUsed, sizeof(int), 1, Input);
    fread(&BMPInfoHeader.biClrImportant, sizeof(int), 1, Input);
}

void WriteHeaders(FILE *Output)
{
    fwrite(&BMPHeader.bfType, sizeof(char), 2, Output);
    fwrite(&BMPHeader.bfSize, sizeof(int), 1, Output);
    fwrite(&BMPHeader.bfReserved1, sizeof(short), 1, Output);
    fwrite(&BMPHeader.bfReserved2, sizeof(short), 1, Output);
    fwrite(&BMPHeader.bfOffBits, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biSize, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biWidth, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biHeight, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biPlanes, sizeof(short), 1, Output);
    fwrite(&BMPInfoHeader.biBitCount, sizeof(short), 1, Output);
    fwrite(&BMPInfoHeader.biCompression, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biSizeImage, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biXPelsPerMeter, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biYPelsPerMeter, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biClrUsed, sizeof(int), 1, Output);
    fwrite(&BMPInfoHeader.biClrImportant, sizeof(int), 1, Output);
}

// remap range[a,b] into [0,255]
double remap(double a, double b, double x)
{
    if (a == b)
        return 255.0 / 2;
    else
        return 255.0 * (x - a) / (b - a);
}

// for channel R and B to swap
void swap(unsigned char *a, unsigned char *b)
{
    int buf = *a;
    *a = *b;
    *b = buf;
}

int main()
{
    FILE *BMP = fopen(".\\input.bmp", "rb");
    FILE *Output1 = fopen(".\\output1.bmp", "wb+");
    FILE *Output2 = fopen(".\\output2.bmp", "wb+");
    if (BMP)
    {
        // step1: read BMPHeader and output1 in file1 and file2
        ReadHeaders(BMP);
        // output header information
        WriteHeaders(Output1);
        WriteHeaders(Output2);
        for (int i = 55; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, BMP);
            fwrite(&buf, sizeof(char), 1, Output1);
            fwrite(&buf, sizeof(char), 1, Output2);
        }
        // step2: read RGB color and convert into YUV, to determine the range of Y
        double HighestY = 0, LowestY = 255;
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        {
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel pixel;
                fread(pixel.Bytes, sizeof(char), 3, BMP);
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                pixel.ConvertYUV();
                if (LowestY > pixel.YUV[0])
                    LowestY = pixel.YUV[0];
                if (HighestY < pixel.YUV[0])
                    HighestY = pixel.YUV[0];
            }
            // please notice the number of pixels because there will be some rear zeros.
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf;
                fread(&buf, sizeof(buf), 1, BMP);
            }
        }
        rewind(BMP);
        for (int i = 1; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, BMP);
        }
        // traverse the bitmap again
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        {
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                // step3: get a copy of the pixel
                Pixel pixel, Copy;
                fread(pixel.Bytes, sizeof(char), 3, BMP);
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                pixel.ConvertYUV();
                Copy = pixel;
                // remapping Y to [0,255]
                Copy.YUV[0] = remap(LowestY, HighestY, pixel.YUV[0]);
                // step4: apply transform1 to the copy
                Copy.YUV[1] = Copy.YUV[2] = 0;
                // step5: convert the pixel into RGB
                Copy.ConvertRGB();
                // step6: output the pixel to file1
                swap(&Copy.Bytes[0], &Copy.Bytes[2]);
                fwrite(Copy.Bytes, sizeof(char), 3, Output1);
                // step7: apply transform2 to original image
                if (deltaY > 0)
                {
                    pixel.YUV[0] = (pixel.YUV[0] + deltaY >= 255) ? 255 : pixel.YUV[0] + deltaY;
                }
                else
                {
                    pixel.YUV[0] = (pixel.YUV[0] + deltaY < 0) ? 0 : pixel.YUV[0] + deltaY;
                }
                // step8: convert the pixel into RGB
                pixel.ConvertRGB();
                // step9: output the pixel to file2
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                fwrite(pixel.Bytes, sizeof(char), 3, Output2);
            }
            // please notice the number of pixels because there will be some rear zeros.
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf;
                fread(&buf, sizeof(buf), 1, BMP);
                fwrite(&buf, sizeof(buf), 1, Output1);
                fwrite(&buf, sizeof(buf), 1, Output2);
            }
        }
        fclose(BMP);
        fclose(Output1);
        fclose(Output2);
    }
    else
    {
        printf("\033[0m\033[1;33mThe file doesn't exist!\033[0m");
    }

    return 0;
}