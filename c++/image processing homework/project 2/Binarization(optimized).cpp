#include "BMPIO.hpp"
#include <stdio.h>
#include <string.h>
#include <cmath>

#define WindowSize 16
#define GlobalPercent 0.1
#define Step 1

long long N1[256], N2[256];
double AVG1[256], AVG2[256];

int main()
{
    FILE *BMP = fopen(".\\input.bmp", "rb");
    BMPMetric *InputMetr = new BMPMetric();
    if (ReadBMP(BMP, InputMetr))
    {
        // Binarize the bitmap
        // step 1. Record the maximum and the minimum of Y
        int MaxY = 0, MinY = 255;
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel pixel = (*InputMetr)[i][j];
                if (floor(pixel.YUV[0]) < MinY)
                    MinY = floor(pixel.YUV[0]);
                if (ceil(pixel.YUV[0]) > MaxY)
                    MaxY = ceil(pixel.YUV[0]);
            }
        // step 2. traverse the pixels and threshold, record N1,N2,μ1,μ2.
        memset(N1, sizeof(N1), 0);
        memset(N2, sizeof(N2), 0);
        memset(AVG1, sizeof(AVG1), 0);
        memset(AVG2, sizeof(AVG2), 0);
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
                for (int TH = MinY; TH <= MaxY; TH++)
                {
                    // assume N1 as brighter part
                    Pixel pixel = (*InputMetr)[i][j];
                    if (pixel.YUV[0] > TH)
                    {
                        // μ' = (Nμ+X)/(N+1) = Nμ/(N+1) = X/(N+1)
                        AVG1[TH] = (double)N1[TH] / (N1[TH] + 1) * AVG1[TH] + pixel.YUV[0] / (N1[TH] + 1);
                        N1[TH]++;
                    }
                    else
                    {
                        AVG2[TH] = (double)N2[TH] / (N2[TH] + 1) * AVG2[TH] + pixel.YUV[0] / (N2[TH] + 1);
                        N2[TH]++;
                    }
                }
        // step 3. calculate σ of each threshold and choose the optimal one
        long long N0 = BMPInfoHeader.biHeight * BMPInfoHeader.biWidth;
        int TH_Global;
        double OptSig = 0;
        for (int TH = MinY; TH <= MaxY; TH++)
        {
            double Sigma = (double)N1[TH] / N0 * N2[TH] / N0 * (AVG1[TH] - AVG2[TH]) * (AVG1[TH] - AVG2[TH]);
            if (Sigma > OptSig)
            {
                OptSig = Sigma;
                TH_Global = TH;
            }
        }
        // step 4. traverse the upper-left corner of the window and process regional shreshold
        for (int i = 0; i <= BMPInfoHeader.biHeight - WindowSize; i += Step)
            for (int j = 0; j <= BMPInfoHeader.biWidth - WindowSize; j += Step)
            {
                // Find the minimum and maximum Y in the window.
                int MinY = 255, MaxY = 0;
                for (int x = i; x < i + WindowSize; x++)
                    for (int y = j; y < j + WindowSize; y++)
                    {
                        Pixel pixel = (*InputMetr)[x][y];
                        if (floor(pixel.YUV[0]) < MinY)
                            MinY = floor(pixel.YUV[0]);
                        if (ceil(pixel.YUV[0]) > MaxY)
                            MaxY = ceil(pixel.YUV[0]);
                    }
                // record N1,N2,μ1,μ2.
                memset(N1, sizeof(N1), 0);
                memset(N2, sizeof(N2), 0);
                memset(AVG1, sizeof(AVG1), 0);
                memset(AVG2, sizeof(AVG2), 0);
                for (int x = i; x < i + WindowSize; x++)
                    for (int y = j; y < j + WindowSize; y++)
                        for (int TH = MinY; TH <= MaxY; TH++)
                        {
                            // assume N1 as brighter part
                            Pixel pixel = (*InputMetr)[x][y];
                            if (pixel.YUV[0] > TH)
                            {
                                // μ' = (Nμ+X)/(N+1) = Nμ/(N+1) = X/(N+1)
                                AVG1[TH] = (double)N1[TH] / (N1[TH] + 1) * AVG1[TH] + pixel.YUV[0] / (N1[TH] + 1);
                                N1[TH]++;
                            }
                            else
                            {
                                AVG2[TH] = (double)N2[TH] / (N2[TH] + 1) * AVG2[TH] + pixel.YUV[0] / (N2[TH] + 1);
                                N2[TH]++;
                            }
                        }
                // calculate σ of each threshold and choose the optimal one
                long long N0 = WindowSize * WindowSize;
                int TH_Window = TH_Global;
                double OptSig = 0;
                for (int TH = MinY; TH <= MaxY; TH++)
                {
                    double Sigma = (double)N1[TH] / N0 * N2[TH] / N0 * (AVG1[TH] - AVG2[TH]) * (AVG1[TH] - AVG2[TH]);
                    if (Sigma > OptSig)
                    {
                        OptSig = Sigma;
                        TH_Window = TH;
                    }
                }
                // traverse the window and make average of threshold.
                for (int x = i; x < i + WindowSize; x++)
                    for (int y = j; y < j + WindowSize; y++)
                    {
                        Pixel *pixel = &(*InputMetr)[x][y];
                        pixel->AVGTH = (double)pixel->THNum / (pixel->THNum + 1) * pixel->AVGTH + (double)TH_Window / (pixel->THNum + 1);
                    }
            }
        // step 5. traverse the pixels, find the final threshold for each pixel, and apply binarization.
        for (int i = 0; i < BMPInfoHeader.biHeight; i++)
            for (int j = 0; j < BMPInfoHeader.biWidth; j++)
            {
                Pixel *pixel = &(*InputMetr)[i][j];
                double TH_Final = GlobalPercent * TH_Global + (1 - GlobalPercent) * pixel->AVGTH;
                pixel->YUV[1] = pixel->YUV[2] = 0;
                pixel->YUV[0] = (pixel->YUV[0] > TH_Final) ? 255 : 0;
                pixel->ConvertRGB();
            }
        // step 6. Output the bitmap
        FILE *Output0 = fopen(".\\Binarize-Optimize.bmp", "wb+");
        WriteBMP(InputMetr, Output0);
        fclose(Output0);
    }
    fclose(BMP);
    delete InputMetr;
}