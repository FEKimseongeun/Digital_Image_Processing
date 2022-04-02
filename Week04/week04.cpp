
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// 히스토그램
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

// 이진화
void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) { // 임계치보다 작으면 0
			Out[i] = 0;
		}
		else {
			Out[i] = 255;
		}
	}
}

// 초기 임계치
BYTE Gonzalez(int* H) { //배열 받아옴
	BYTE ep = 3; //입실론 값 제한
	BYTE low, high=0;
	int m1 = 0, m2 = 0;
	int cnt1 = 0, cnt2 = 0;
	int mu1, mu2;

	for (int i = 0; i < 256; i++) {	// 밝기 최소값
		if (H[i] > 0) {
			low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) { // 밝기 최대값
		if (H[i] > 0) {
			high = i;
			break;
		}
	}

	int Thr = (low + high) / 2;

	while (1) {
		for (int i = Thr + 1; i <= high; i++) {
			m1 += (H[i] * i); //밝기값의 개수 * 밝기값
			cnt1 += H[i];
		}

		for (int i = low; i <= Thr; i++) {
			m2 += (H[i] * i);
			cnt2 += H[i];
		}
		if (cnt1 == 0) cnt1 = 1;
		if (cnt2 == 0) cnt2 = 1;
		mu1 = m1 / cnt1;
		mu2 = m2 / cnt2;

		int NewTh = (mu1 + mu2) / 2;

		if (abs(NewTh - Thr) < ep) {
			Thr = NewTh;
			break;
		}
		else {
			Thr = NewTh;
		}
		m1 = m2 = cnt1 = cnt2 = 0;
	}
	return Thr;
}

int main() {
	// -----------------------------------
	// 영상 입력
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 1024 bytes

	FILE* fp;
	fp = fopen("lenna.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
	if (fp == NULL) {
		printf("File not found!/n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hinfo.biWidth * hinfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize); // 영상 처리 결과 담기
	fread(Image, sizeof(BYTE), ImgSize, fp); // 영상의 화수 정보 담기
	fclose(fp); //파일 포인터와 영성파일 간의 연결 끊기


	//---------------------------------------
	// 영상 처리

	int Histo[256] = { 0 }; // 히스토그램
	BYTE Th;

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // 히스토그램
	Th = Gonzalez(Histo); // 경계값 T 초기값
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, Th); // 이진화

	//---------------------------------------
	// 처리 결과 출력
	fp = fopen("output.bmp", "wb"); //bmp파일 생성, wb = write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(저장할 변수, 크기, 횟수, 현재 파일 포인터)
	fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // 기록을 할때는 1byte 단위로 해야함
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // 영상 처리 결과 출력
	fclose(fp); // 파일포인터와 영상파일 관계 끊기

	free(Image); // 동적할당 해제
	free(Output); // 하지않으면 memory leak 현상발생

	return 0;
}