- 먼저 그림판에서 아무렇게나 그림을 그려주고 BMP 파일로 저장을 해주자.

  ![](https://velog.velcdn.com/images/kansun12/post/83c211bd-4c54-42aa-8711-84bccb58321c/image.png)


먼저 파일 크기를 알아보고 가보자.

### ✔8bit Gray (256 level)

- `BITMAPFILEHEADER hf;` // 14바이트
- `BITMAPINFOHEADER hInfo;` // 40바이트
- `RGBQUAD hRGB[256];` // 1024바이트
- 그리고 픽셀정보 (1픽셀당 **8bits (=1 byte)**)

### ✔24bit True Color (1600만 컬러)

- `BITMAPFILEHEADER hf;` // 14바이트
- `BITMAPINFOHEADER hInfo;` // 40바이트
- 픽셀정보(1픽셀을  표현하는데 **24bit(=3 byte)**를 사용)
    - 팔레트는 없음

---

# ✅코드

**먼저 트루컬러일 때와 인덱스 컬러일때를 나누어서 파일 구조를 잡아준다.**

```c
BYTE * Image;
	BYTE * Output;
	if (hInfo.biBitCount == 24) { //트루컬러일때
		Image = (BYTE *)malloc(ImgSize*3);
		Output = (BYTE*)malloc(ImgSize*3);
		fread(Image, sizeof(BYTE), ImgSize*3, fp);
	}
	else{ //인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE *)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
```

```c
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE * fp = fopen(FileName, "wb");
	if (hInfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W*H*3, fp);
	}
	if (hInfo.biBitCount == 8) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W*H , fp);
	}
	fclose(fp);
}
```

### 1️⃣특정 위치의 색을 바꿔보자. (빨간색으로)

```c
//(50,40)위치를 특정색상으로 
//트루컬러에서는 2차원적 인덱싱 방법이 y좌표x영상의 가로 사이즈x3이어야함. 
//x좌표도 x3을 해줘야함
	Image[40 * W * 3 + 50 * 3] = 0; //B
	Image[40 * W * 3 + 50 * 3+1] = 0; //G
	Image[40 * W * 3 + 50 * 3+2] = 255; //R
```

트루컬러에서는  한 픽셀을 표현할 수 있는  3개의 컬러가 있으니까 이미지 배열이 바이트 단위이고 1바이트임. 한 픽셀을 표현하는데 3바이트를 쓰기 때문에 *3씩 해주는 것임.

![](https://velog.velcdn.com/images/kansun12/post/0b4ecec3-7487-452d-802f-0e72e097ee61/image.png)

보일지 모르겠지만 아래 딱 한 픽셀 빨간색으로 점이 찍혀있는 것을 확인할  수 있따.

- **`FillColor 함수`**

    ```c
    void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
    	//(50,40)위치를 특정색상으로 
    	//트루컬러에서는 2차원적 인덱싱 방법이 y좌표x영상의 가로 사이즈x3이어야함. x좌표도 x3을 해줘야함
    	Image[Y * W * 3 + X * 3] = 0; //B
    	Image[Y * W * 3 + X * 3 + 1] = 0; //G
    	Image[Y * W * 3 + X * 3 + 2] = 255; //R
    }
    ```


### 2️⃣라인으로 색을 채워보기

```c
for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 0, 255, 255);
	}
```

- 반복문을 활용해서 Y좌표는 200으로 고정 후에 X좌표만 변경되도록 했다.

![](https://velog.velcdn.com/images/kansun12/post/ce8ce3ba-b699-422a-b57e-787e18232e3d/image.png)

이렇게 파란색 선이 생긴 것을 확인할 수 있다.

### 3️⃣(50,100) ~ (300, 400)의 직사각형을 만들고 채워보자

```c
//(50,100) ~ (300, 400)의 직사각형의 외각선을 그려보자
	for (int i = 100; i <= 400; i++) {
		for (int j = 50; j <= 300; j++) {
			FillColor(Image, j, i, W, H, 255, 0, 255);
		}
	}
```

![](https://velog.velcdn.com/images/kansun12/post/e96b25d1-da5f-4e67-baa2-919bd1dcdeae/image.png)

### 전체 코드

```c
//컬러 가로 라인 그리기
	for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 0, 255, 255);
	}
	//한 픽셀 컬러 채우기
	FillColor(Image, 100, 200, W, H, 0, 255, 255);
	//(50,100) ~ (300, 400)의 직사각형을 만들고 채워보자
	for (int i = 100; i <= 400; i++) {
		for (int j = 50; j <= 300; j++) {
			FillColor(Image, j, i, W, H, 255, 0, 255);
		}
	}
```

### 4️⃣가로 띠 만들기

```c
//가로 띠 만들기
	//초기화
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W * 3 + j * 3] = 0;
			Image[i*W * 3 + j * 3 + 1] = 0;
			Image[i*W * 3 + j * 3 + 2] = 0;
		}
	}

	//y좌표 기준 0~239(red)
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W*3 + j*3+2] = 255; //레드
		}
	}
	//y좌표 기준 120~359(green)
	for (int i = 120; i < 360; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W * 3 + j * 3 + 1] = 255;
		}
	}
	//y좌표 기준 240~479(blue)
	for (int i = 240; i < 480; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W * 3 + j * 3] = 255;
		}
	}
```

![](https://velog.velcdn.com/images/kansun12/post/9f108322-a3d5-4dd6-9522-187a89ffe9ff/image.png)

결과

### 5️⃣그라데이션 코드

```c
// 그라데이션 만들기(블루에서 레드로)
	double wt;
	for (int i = 0; i < W; i++) {
		wt = i / (double)(W - 1);
		Image[240*W * 3 + i * 3] = (BYTE)(255*(1.0-wt)); // blue
		Image[240 *W * 3 + i * 3 + 1] = 0;  // green
		Image[240 *W * 3 + i * 3 + 2] = (BYTE)(255*wt); // red
	}
```

- 가중치를 하나 만들어줘서 그 가중치의 값에 따라 블루와 레드에 영향이 가도록 만들어주는 것이다.

![](https://velog.velcdn.com/images/kansun12/post/95baa790-1bf5-4c9a-8a91-a8c72e01c4a4/image.png)

- 박스 형태로 보고싶다면?

```c
for (int a = 0; a < 120; a++) {
		for (int i = 0; i < W; i++) {
				wt = i / (double)(W - 1);
				Image[a*W * 3 + i * 3] = (BYTE)(255*(1.0-wt)); // blue
				Image[a *W * 3 + i * 3 + 1] = 0;  // green
				Image[a *W * 3 + i * 3 + 2] = (BYTE)(255*wt); // red
			}
	}
```

- y좌표에 해당하는 친구를 반복문으로 처리하면 된다.

![](https://velog.velcdn.com/images/kansun12/post/9d805ed0-6d8e-4970-82bd-103b31dbdbb5/image.png)

# ✅실습

### 그라데이션 아래 이미지처럼 만들기

![](https://velog.velcdn.com/images/kansun12/post/0392b6da-4810-44a5-9013-7738df5f4b80/image.png)


```c
// 초기화
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	//그라데이션 만들기
	double wt; // 가중치
	// y좌표 기준 0 ~ 239 (위) Red -> Cyan
	for (int i = H / 3 * 2; i < H; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * (1.0 - wt); // Red
		}
	}
	// y좌표 기준 120 ~ 359 Green -> Magenta
	for (int i = H / 3; i < H / 3 * 2; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * (1.0 - wt); // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
	// y좌표 기준 340 ~ 479 Blue -> Yellow
	for (int i = 0; i < H / 3; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * (1.0 - wt); // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
```