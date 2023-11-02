#include <cmath>
#include <stdio.h>
#include <vector>

using std::vector;

class Pixel
{
public:
    unsigned char RGB[3];
    double YUV[3];
    double AVGTH;
    int THNum;
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
        RGB[0] = round(buf1);
        RGB[1] = round(buf2);
        RGB[2] = round(buf3);
    }
    void ConvertYUV()
    {
        double buf1, buf2, buf3;
        buf1 = 0.299 * RGB[0] + 0.587 * RGB[1] + 0.114 * RGB[2];
        buf2 = -0.147 * RGB[0] - 0.289 * RGB[1] + 0.435 * RGB[2];
        buf3 = 0.615 * RGB[0] - 0.515 * RGB[1] - 0.100 * RGB[2];
        if (buf1 > 255)
            buf1 = 255;
        YUV[0] = buf1;
        YUV[1] = buf2;
        YUV[2] = buf3;
    }
};

typedef vector<Pixel> BMPLine;
typedef vector<BMPLine> BMPMetric;

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
    vector<unsigned int> Paletee;
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

// for channel R and B to swap
void swap(unsigned char *a, unsigned char *b)
{
    int buf = *a;
    *a = *b;
    *b = buf;
}

int ReadBMP(FILE *Input, BMPMetric *Output)
{
    if (Input == NULL)
    {
        printf("The file doesn't exist!");
        return 0;
    }
    rewind(Input);
    ReadHeaders(Input);
    BMPInfoHeader.Paletee.clear();
    for (int i = 55; i <= BMPHeader.bfOffBits; i++)
    {
        unsigned char buf;
        fread(&buf, sizeof(char), 1, Input);
        BMPInfoHeader.Paletee.push_back(buf);
    }
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
    {
        BMPLine *Line = new BMPLine();
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            Pixel pixel;
            fread(pixel.RGB, sizeof(char), 3, Input);
            swap(&pixel.RGB[0], &pixel.RGB[2]); // R and B is inverted
            pixel.ConvertYUV();
            Line->push_back(pixel);
        }
        for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, Input);
        }
        Output->push_back(*Line);
        delete Line;
    }
    return 1;
}

void WriteBMP(BMPMetric *Input, FILE *Output)
{
    WriteHeaders(Output);
    for (int i = 55; i <= BMPHeader.bfOffBits; i++)
    {
        unsigned char buf = BMPInfoHeader.Paletee[i - 55];
        fwrite(&buf, sizeof(char), 1, Output);
    }
    for (int i = 0; i < BMPInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < BMPInfoHeader.biWidth; j++)
        {
            Pixel pixel = (*Input)[i][j];
            swap(&pixel.RGB[0], &pixel.RGB[2]);
            fwrite(pixel.RGB, sizeof(char), 3, Output);
        }
        for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
        {
            unsigned char buf = 0;
            fwrite(&buf, sizeof(char), 1, Output);
        }
    }
}