// 201910782 김성은
//[과제] Median filter 사이즈에 따른 결과 비교
// 복붙방지로 조금 틀리게 적어놓음
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp);
	fclose(fp);
}


void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}


int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("lenna_gauss.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Temp = (BYTE*)malloc(ImgSize); // 임시배열
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* Median filtering */

	int Length = 5;  // 마스크의 한 변의 길이
	int Margin = Length / 2;
	int WSize = Length * Length; // 필터링 할 윈도우 크기-원본 영상 밖은 처리하지 않기 위해 length의 중앙값을 margin으로 설정
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // 필터링을  위한 임시 변수
	int W = hInfo.biWidth, H = hInfo.biHeight;// 원본 이미지의 가로(W), 세로(H) 사이즈
	int i, j, m;
	for (i = Margin; i < H - Margin; i++) {  // 원본영상의 열(세로방향이동)
		for (j = Margin; j < W - Margin; j++) { // 원본영상의 행 (가로방향이동)
			for (m = -Margin; m <= Margin; m++) { //Length X Length (윈도우) 화소값을 temp배열에 넣고 윈도우의 열(세로방향이동)
				for (n = -Margin; n <= Margin; n++) { // 윈도우의 행(가로방향이동)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n];
				}
			}
			Output[i * W + j] = Median(temp, WSize);
		}
	}
	free(temp);
	free(Image);
	free(Output);
	free(Temp);
	return 0;
}