# ✅침식

```c
void Erosion(BYTE * Image, BYTE* Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i*W + j] == 255)//전경화소라면
			{
				if (!(Image[(i - 1)*W + j] == 255 &&
					Image[(i + 1)*W + j] == 255 &&
					Image[i*W + (j - 1)] == 255 &&
					Image[i*W + (j + 1)] == 255)) {
					Output[i*W + j] = 0;
				}
				else
					Output[i*W + j] = 255;
			}
			else
				Output[i*W + j] = 0;
		}
	}
}
```

![](https://velog.velcdn.com/images/kansun12/post/50abfb9e-6d46-41c8-8178-4b27d843924b/image.png)

4전경화소 검사법을 활용해서 검사했을 때 하나라도 전경화소가 아닌게 있다면 그 화소는 바로 배경화소로 바뀌도록 만든다.

보일지 모르겠지만 미세하게 침식이 된 것을 확인할 수 있다.

### 완전히 동전과 동전 사이를 끊어보자.

```c
Erosion(Image, Output, W, H);
Erosion(Output, Output, W, H);
Erosion(Image, Output, W, H);
Erosion(Output, Image, W, H);
```

- 이렇게 네 번 침식을 진행해주면…

  ![](https://velog.velcdn.com/images/kansun12/post/0d22d4bc-83c7-4001-ae45-07b296ad0406/image.png)

  이렇게 떨어뜨릴 수 있게 된다.

    - 참고로 이렇게 떨어뜨리고 팽창을 다시 하면 이전처럼 다시 붙는게 아니라 동전만 원본 크기대로 돌아가게 된다. 이같이 **침식 후 팽창하는 것**을 **`열림 현상`**이라 한다.

---

# ✅팽창

```c
void Dilation(BYTE * Image, BYTE* Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i*W + j] == 0)//배경화소라면
			{
				if (!(Image[(i - 1)*W + j] == 0 &&
					Image[(i + 1)*W + j] == 0 &&
					Image[i*W + (j - 1)] == 0 &&
					Image[i*W + (j + 1)] == 0)) { //4주변화소가 모두 배경화소가 아니라면
					Output[i*W + j] = 255;
				}
				else
					Output[i*W + j] = 0;
			}
			else
				Output[i*W + j] = 255;
		}
	}
}
```

![](https://velog.velcdn.com/images/kansun12/post/89fb8126-ab9f-4876-8273-e598a7025ab6/image.png)

- 팽창을 시켜주면 왼쪽같은 원본 영상에서 노이즈를 없애준 상태의 오른쪽 영상으로 추출된다.

![](https://velog.velcdn.com/images/kansun12/post/cb49d701-db7b-4042-b5cd-6526f40d68d2/image.png)

- Dilation을 여러번 해주면 이렇게 두꺼워진 것을 확인할 수 있다.
- 근데 너무 뚱뚱하다

### 원본 크기와 똑같게 만들어주기

```c
Dilation(Image, Output, W, H);	
Dilation(Output, Image, W, H);
Dilation(Image, Output, W, H);
Erosion(Output, Image, W, H);
Erosion(Image, Output, W, H);
Erosion(Output, Image, W, H);
```

- 팽창 세번을 통해 노이즈를 없애주고 자시 침식을 세번 해줘서 크기를 같게 만들어준다.

  ![](https://velog.velcdn.com/images/kansun12/post/d44182d3-e8f1-4797-bb44-ea69d3d96bf7/image.png)

- 이렇게 **팽창을 해준 후에 침식을 시키는 것**을 **`닫힘 현상`**이라고 한다.

---

## Zangshen 기법 (Thining-세선화 하는 기법)

- 얇게 만드는 데에는
    - 골격화 (Skeletionization)
    - 세선화 (Thining)
    - **`차이점`**

  ![](https://velog.velcdn.com/images/kansun12/post/9dd7db4a-706a-47fa-b068-dcd714432f15/image.png)

    - 골격화 같은 경우에는 나중에 다시 복원할 수 있을 정도로 모양의 뼈대를 잡는 것이라면 세선화는 그저 모양을 얇게 하는 것임.

### 코드

```c
// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;
// 이진영상에서 
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
	int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

	int i, j, sum = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0)
				sum += (imageMatrix[row + i][col + j] == imagePixel);
		}
	}

	return sum;
}

int getBWTransitions(int row, int col) {
	return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
		+ (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
		+ (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
		+ (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
		+ (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
		+ (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
		+ (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
		+ (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {

	int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

	pixel* markers;

	imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

	for (i = 0; i < rows; i++) {
		imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
		for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
	}

	endRow = rows - 2;
	endCol = cols - 2;
	do {
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		if (processed == 0)
			processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
	} while (processed == 1);

	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			output[i * cols + j] = imageMatrix[i][j];
		}
	}
}
```

전역 변수를 **`blankPixel = 255, imagePixel = 0`** 로 만들어줬기 때문에 이걸 안바꿔주면 이상하게 나옴 → InverImage 함수를 쓰면 됨 즉,,,메인에서

```c
Dilation(Image, Output, W, H);	
Dilation(Output, Image, W, H);
Dilation(Image, Output, W, H);
Erosion(Output, Image, W, H);
Erosion(Image, Output, W, H);
Erosion(Output, Image, W, H);
InverseImage(Image, Image, W, H);
zhangSuen(Image, Output, H, W);
```

- 이렇게 해주면 세선화가 잘 된 채로(두께가 1px) 나온다.

![](https://velog.velcdn.com/images/kansun12/post/cd3ad7c5-5f50-4272-84a0-a4ca01b73511/image.png)

- 그**럼 이 세선화 기법은 어디서 많이 쓰일까**?
    - 지문인식에서 많이 쓰임.
        - **`분기점과 끝점`**을 활용해서 지문 인식이 되도록 하는 것임
        - **`분기점과 끝점`**이란?

          ![](https://velog.velcdn.com/images/kansun12/post/d80c9bcb-69d5-456d-a385-3a6ea30d97a5/image.png)


    ```c
    void FeatureExtractThinImage(BYTE* Image, BYTE* Output, int W, int H) {
    	for (int i = 0; i < W*H; i++) Output[i] = Image[i];
    	int cnt = 0;
    	for (int i = 1; i < H - 1; i++) {
    		for (int j = 1; j < W - 1; j++) {
    			if (Image[i*W + j] == 0)//배경화소라면
    			{
    				if (Image[(i - 1)*W + j-1] == 0 && Image[(i - 1)*W + j] == 255) cnt++;
    				if (Image[(i - 1)*W + j] == 0 && Image[(i - 1)*W + j + 1] == 255) cnt++;
    				if (Image[(i - 1)*W + j + 1] == 0 && Image[i*W + j + 1] == 255) cnt++;
    				if (Image[i*W + j + 1] == 0 && Image[(i + 1)*W + j + 1] == 255) cnt++;
    				if (Image[(i+1)*W + j + 1] == 0 && Image[(i + 1)*W + j] == 255) cnt++;
    				if (Image[(i+1)*W + j] == 0 && Image[(i + 1)*W + j - 1] == 255) cnt++;
    				if (Image[(i + 1)*W + j - 1] == 0 && Image[i*W + j - 1] == 255) cnt++;
    				if (Image[i*W + j - 1] == 0 && Image[(i - 1)*W + j - 1] == 255) cnt++;
    			}
    			// 분기점과 끝점 표시
    			if (cnt > 2 || cnt == 1) {
    				for (int m = -1; m <= 1; m++) {
    					for (int n = -1; n <= 1; n++) {
    						Output[(i + m) * W + (j + n)] = 130;
    						if (m == 0 && n == 0)
    							Output[(i + m) * W + (j + n)] = 0;
    					}
    				}
    			}
    			cnt = 0;
    		}
    	}
    }
    ```
    
    ![](https://velog.velcdn.com/images/kansun12/post/012bf597-521e-4c05-8bb6-6e254994f7da/image.png)
    
    **8전경화소를 검사해서 분기점과 끝점에 뭉텅이로 표현되도록 하였다.**