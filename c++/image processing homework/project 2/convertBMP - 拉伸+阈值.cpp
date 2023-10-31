#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <string.h>

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
        if (buf1 > 255)
            buf1 = 255;
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

// for channel R and B to swap
void swap(unsigned char *a, unsigned char *b)
{
    int buf = *a;
    *a = *b;
    *b = buf;
}
void ReadLine(vector<Pixel> *Vec, FILE *Src)
{
    for (int i = 0; i < BMPInfoHeader.biWidth; i++)
    {
        Pixel p;
        fread(p.Bytes, sizeof(char), 3, Src);
        swap(&p.Bytes[0], &p.Bytes[2]);
        p.ConvertYUV();
        swap(&p.Bytes[0], &p.Bytes[2]);
        (*Vec).push_back(p);
    }
    for (int i = BMPInfoHeader.biWidth * 3; i % 4 != 0; i++)
    {
        unsigned char buf;
        fread(&buf, sizeof(char), 1, Src);
    }
}

long long N1[256], N2[256];
double AVG1[256], AVG2[256];
vector<Pixel> v1, v2, v3;
vector<Pixel> *VecMap[3];

void swapVec()
{
    vector<Pixel> *buf;
    buf = VecMap[0];
    VecMap[0] = VecMap[1];
    VecMap[1] = buf;
    buf = VecMap[1];
    VecMap[1] = VecMap[2];
    VecMap[2] = buf;
}

int main()
{
    FILE *BMP = fopen(".\\input.bmp", "rb");
    if (BMP)
    {
        FILE *Output0 = fopen(".\\output0.bmp", "wb+");
        // step1. read headers
        ReadHeaders(BMP);
        // step2. output headers
        WriteHeaders(Output0);
        for (int i = 55; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, BMP);
            fwrite(&buf, sizeof(char), 1, Output0);
        }
        // step3. read pixels and record the minimum Y and the maximum Y
        int MINY = 255, MAXY = 0;
        long long N0 = BMPInfoHeader.biHeight * BMPInfoHeader.biWidth;
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        {
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel pixel;
                fread(pixel.Bytes, sizeof(char), 3, BMP);
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                pixel.ConvertYUV();
                if (floor(pixel.YUV[0]) < MINY)
                    MINY = floor(pixel.YUV[0]);
                if (ceil(pixel.YUV[0]) > MAXY)
                    MAXY = ceil(pixel.YUV[0]);
            }
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf;
                fread(&buf, sizeof(char), 1, BMP);
            }
        }
        // step 4. read the pixels again and determine N1,N2,AVG1,AVG2
        memset(N1, 0, sizeof(N1));
        memset(N2, 0, sizeof(N2));
        memset(AVG1, 0, sizeof(AVG1));
        memset(AVG2, 0, sizeof(AVG2));
        rewind(BMP);
        for (int i = 1; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, BMP);
        }
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        {
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel pixel;
                fread(pixel.Bytes, sizeof(char), 3, BMP);
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                pixel.ConvertYUV();
                for (int TH = MINY; TH <= MAXY; TH++)
                {
                    // brighter N1, darker N2.
                    if (pixel.YUV[0] > TH)
                    {
                        AVG1[TH] = (double)N1[TH] / (N1[TH] + 1) * AVG1[TH] + pixel.YUV[0] / (N1[TH] + 1);
                        ++N1[TH];
                    }
                    else
                    {
                        AVG2[TH] = (double)N2[TH] / (N2[TH] + 1) * AVG2[TH] + pixel.YUV[0] / (N2[TH] + 1);
                        ++N2[TH];
                    }
                }
            }
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf;
                fread(&buf, sizeof(char), 1, BMP);
            }
        }
        // step 5. calculate sigma of each threshold, choose the largest one as the opitimal threshold.
        int OptTH = MINY;
        double MaxSig = 0;
        for (int i = MINY; i <= MAXY; i++)
        {
            double Sigma = (double)N1[i] / N0 * N2[i] / N0 * (AVG1[i] - AVG2[i]) * (AVG1[i] - AVG2[i]);
            if (Sigma > MaxSig)
            {
                MaxSig = Sigma;
                OptTH = i;
            }
        }
        // step 6. read the pixels again, turn it to YUV and set UV to both 0.
        // step 7. Output the bitmap
        rewind(BMP);
        for (int i = 1; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(buf), 1, BMP);
        }
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
        {
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel pixel;
                fread(pixel.Bytes, sizeof(char), 3, BMP);
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                pixel.ConvertYUV();
                pixel.YUV[0] = (pixel.YUV[0] > OptTH) ? 255 : 0;
                pixel.YUV[1] = pixel.YUV[2] = 0;
                pixel.ConvertRGB();
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                fwrite(pixel.Bytes, sizeof(char), 3, Output0);
            }
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf;
                fread(&buf, sizeof(char), 1, BMP);
                fwrite(&buf, sizeof(char), 1, Output0);
            }
        }
        fclose(BMP);
        fclose(Output0);
        // Now, we try to apply dialation and erosion to the image.
        // Step1. read the basic info of binarized image
        FILE *BInput = fopen(".\\Output0.bmp", "rb");
        FILE *Output1 = fopen(".\\Output1.bmp", "wb+");
        FILE *Output2 = fopen(".\\Output2.bmp", "wb+");
        ReadHeaders(BInput);
        WriteHeaders(Output1);
        for (int i = 55; i <= BMPHeader.bfOffBits; i++)
        {
            unsigned char buf;
            fread(&buf, sizeof(char), 1, BInput);
            fwrite(&buf, sizeof(char), 1, Output1);
            fwrite(&buf, sizeof(char), 1, Output2);
        }
        // Step2. Initialize the vector map.
        VecMap[0] = &v1;
        VecMap[1] = &v2;
        VecMap[2] = &v3;
        // Step3. Read first two lines.
        ReadLine(VecMap[0], BInput);
        ReadLine(VecMap[1], BInput);
        // Step4. Reserve data on the first line.
        for (int i = 0; i < BMPInfoHeader.biWidth; i++)
        {
            fwrite((*VecMap[0])[i].Bytes, sizeof(char), 3, Output1);
            fwrite((*VecMap[0])[i].Bytes, sizeof(char), 3, Output2);
        }
        for (int i = BMPInfoHeader.biWidth * 3; i % 4 != 0; i++)
        {
            unsigned char buf = 0;
            fwrite(&buf, sizeof(char), 1, Output1);
            fwrite(&buf, sizeof(char), 1, Output2);
        }
        // Step5. Begin dialation process while reading
        for (int i = 0; i < BMPInfoHeader.biHeight - 2; i++)
        {
            // Read a line.
            ReadLine(VecMap[2], BInput);
            // Reserve the first element.
            Pixel p;
            fread(p.Bytes, sizeof(char), 3, BInput);
            fwrite(p.Bytes, sizeof(char), 3, Output1);
            fwrite(p.Bytes, sizeof(char), 3, Output2);
            // Process in the mid.
            for (int j = 1; j <= BMPInfoHeader.biWidth - 2; j++)
            {
                int flag1 = 0, flag2 = 1;
                if ((*VecMap[0])[j].YUV[0] >= 128)
                    flag1 = 1;
                else
                    flag2 = 0;
                if ((*VecMap[2])[j].YUV[0] >= 128)
                    flag1 = 1;
                else
                    flag2 = 0;
                if ((*VecMap[1])[j - 1].YUV[0] >= 128)
                    flag1 = 1;
                else
                    flag2 = 0;
                if ((*VecMap[1])[j + 1].YUV[0] >= 128)
                    flag1 = 1;
                else
                    flag2 = 0;
                if ((*VecMap[1])[j].YUV[0] >= 128)
                    flag1 = 1;
                else
                    flag2 = 0;
                Pixel pixel;
                pixel.YUV[0] = flag1 ? 255 : 0;
                pixel.YUV[1] = pixel.YUV[2] = 0;
                pixel.ConvertRGB();
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                fwrite(pixel.Bytes, sizeof(char), 3, Output1);
                pixel.YUV[0] = flag2 ? 0 : 255;
                pixel.YUV[1] = pixel.YUV[2] = 0;
                pixel.ConvertRGB();
                swap(&pixel.Bytes[0], &pixel.Bytes[2]);
                fwrite(pixel.Bytes, sizeof(char), 3, Output2);
            }
            // Reserve the last element.
            fread(p.Bytes, sizeof(char), 3, BInput);
            fwrite(p.Bytes, sizeof(char), 3, Output1);
            fwrite(p.Bytes, sizeof(char), 3, Output2);
            // zeros in the end.
            for (int j = BMPInfoHeader.biWidth * 3; j % 4 != 0; j++)
            {
                unsigned char buf = 0;
                fwrite(&buf, sizeof(char), 1, Output1);
                fwrite(&buf, sizeof(char), 1, Output2);
            }
            // clear the first Line.
            VecMap[0]->clear();
            // Reset the VecMap
            swapVec();
        }
        // Reserve the last line.
        for (int i = 0; i < BMPInfoHeader.biWidth; i++)
        {
            fwrite((*VecMap[2])[i].Bytes, sizeof(char), 3, Output1);
            fwrite((*VecMap[2])[i].Bytes, sizeof(char), 3, Output2);
        }
        for (int i = BMPInfoHeader.biWidth * 3; i % 4 != 0; i++)
        {
            unsigned char buf = 0;
            fwrite(&buf, sizeof(char), 1, Output1);
            fwrite(&buf, sizeof(char), 1, Output2);
        }
        fclose(BInput);
        fclose(Output1);
        fclose(Output2);
    }
    else
    {
        printf("The file doesn't exist!\n");
    }
    return 0;
}