#include <cmath>
#include <stdio.h>
#include <vector>

#define Y() YUV[0]
#define U() YUV[1]
#define V() YUV[2]
#define R() RGB[0]
#define G() RGB[1]
#define B() RGB[2]
#define H() HSL[0]
#define S() HSL[1]
#define L() HSL[2]
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

using std::vector;

class Pixel
{
public:
    unsigned char RGB[3];
    double YUV[3];
    double HSL[3];
    double AVGTH; // for threshold
    int THNum;    // for threshold
    void Conv_YUV_RGB()
    {
        double buf1, buf2, buf3;
        buf1 = 1.0000 * Y() + 0.0000 * U() + 1.1398 * V();
        buf2 = 0.9996 * Y() - 0.3954 * U() - 0.5805 * V();
        buf3 = 1.0020 * Y() + 2.0361 * U() - 0.0005 * V();
        if (buf1 > 255)
            buf1 = 255;
        if (buf2 > 255)
            buf2 = 255;
        if (buf3 > 255)
            buf3 = 255;
        R() = round(buf1);
        G() = round(buf2);
        B() = round(buf3);
    }
    void Conv_RGB_YUV()
    {
        double buf1, buf2, buf3;
        buf1 = 0.299 * R() + 0.587 * G() + 0.114 * B();
        buf2 = -0.147 * R() - 0.289 * G() + 0.435 * B();
        buf3 = 0.615 * R() - 0.515 * G() - 0.100 * B();
        if (buf1 > 255)
            buf1 = 255;
        Y() = buf1;
        U() = buf2;
        V() = buf3;
    }
    void Conv_HSL_RGB()
    {
        // judge if the HSL is within the range.
        if (H() < 0 || H() >= 360.00001)
        {
            printf("\nError:Hue Out Of Range!");
            exit(0);
        }
        if (S() < 0 || S() > 1.00001)
        {
            printf("\nError:Saturation Out Of Range!");
            exit(0);
        }
        if (L() < 0)
        {
            printf("\nError:Lightness Out Of Range!");
            exit(0);
        }
        if (L() > 1)
            L() = 1;
        // Special case: s = 0
        if (S() == 0)
            R() = G() = B() = L();
        // s != 0
        double q = (L() < 0.5) ? (L() * (1 + S())) : (L() + S() - (L() * S()));
        double p = 2 * L() - q;
        double tRGB[3];
        tRGB[0] = (H() / 360.0 + 1.0 / 3 > 1) ? (H() / 360.0 - 2.0 / 3.0) : (H() / 360.0 + 1.0 / 3.0);
        tRGB[1] = H() / 360.0;
        tRGB[2] = (H() / 360.0 - 1.0 / 3 < 0) ? (H() / 360.0 + 2.0 / 3.0) : (H() / 360.0 - 1.0 / 3.0);
        int i;
        for (i = 0; i <= 2; i++)
        {
            if (tRGB[i] < 1.0 / 6)
                RGB[i] = round(255 * (p + ((q - p) * 6 * tRGB[i])));
            else if (tRGB[i] < 1.0 / 2)
                RGB[i] = round(255 * q);
            else if (tRGB[i] < 2.0 / 3)
                RGB[i] = round(255 * (p + ((q - p) * 6 * (2.0 / 3 - tRGB[i]))));
            else
                RGB[i] = round(255 * p);
        }
    }
    void Conv_RGB_HSL()
    {
        double Maximum = (MAX(MAX(R(), G()), B())) / 255.0;
        double Minimum = (MIN(MIN(R(), G()), B())) / 255.0;
        double R = R() / 255.0;
        double G = G() / 255.0;
        double B = B() / 255.0;
        // special case
        if (Maximum == Minimum)
        {
            H() = 0;
            L() = Maximum;
            S() = 0;
            return;
        }
        // calculate H
        if (Maximum == R)
        {
            if (G >= B)
            {
                H() = 60.0 * (G - B) / (Maximum - Minimum);
            }
            else
            {
                H() = 60.0 * (G - B) / (Maximum - Minimum) + 360;
            }
        }
        else if (Maximum == G)
        {
            H() = 60.0 * (B - R) / (Maximum - Minimum) + 120;
        }
        else
            H() = 60.0 * (R - G) / (Maximum - Minimum) + 240;
        // calculate l
        L() = (Maximum + Minimum) / 2;
        // calculate S
        S() = (L() < 1 / 2) ? (Maximum - Minimum) / (2 * L()) : (Maximum - Minimum) / (2 * (1 - L()));
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
            pixel.Conv_RGB_YUV();
            pixel.Conv_RGB_HSL();
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