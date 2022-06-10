# 기하변환 코드
### Translation

```c
//Translation
	int Tx = 50, Ty = 30;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Output[(i+Ty)*W+(j+Tx)] = Image[i*W + j];;

		}
	}
```

이렇게 작성하면 돌아가긴하는데 자동으로 에러가 발생해 종료된다.

→ 영상의 원래 위치에서 x방향으로 50만큼 이동 시킨다는 것은 영상의 범위를 벗어나게 되는 것 배열의 존재하지 않는 인덱스를 참조하게 되는 것이다. 그래서 에러가 나는 것

```c
//Translation
	int Tx = 50, Ty = 30;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if((i+Ty<H && i+Ty>=0)&&(j+Tx < W && j+Tx>=0)) //예외처리
				Output[(i+Ty)*W+(j+Tx)] = Image[i*W + j];

		}
	}
```

이렇게 예외처리를 넣어주면 된다.

![](https://velog.velcdn.com/images/kansun12/post/dca23227-9932-4c0a-a5ee-8bf29be7f032/image.png)

그럼 이동된 것을 잘 확인할 수 있다.

그런데 우리는 분명 양수로 30만큼 이동 시켰는데 오히려 오른쪽 위로 이동된 것을 알 수있다.
>이것은 BMP 파일이 위아래가 뒤집혀있기 때문이다. 그래서 BMP파일은 양의 방향으로 이동 시키고 싶으면 음수로 값을 주면 되는 것이다. **-30!**

### 뒤집기

```c
void VerticalFlip(BYTE * img, int W, int H) {
	for (int i = 0; i < H / 2; i++) {//y좌표
		for (int j = 0; j < W; j++) {//x좌표
			swap(&img[i*W + j], &img[(H - 1 - i)*W + j]);
		}
	}
}

void HorizontalFlip(BYTE * img, int W, int H) {
	for (int i = 0; i < W / 2; i++) {//x좌표
		for (int j = 0; j < H; j++) {//y좌표
			swap(&img[j*W + i], &img[j*W + (H - 1 - i)]);
		}
	}
}
```

### 스케일링

```c
//Scailing
	double SF_X = 1.3, SF_Y = 1.5;
	int tmpX, tmpY;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)j*SF_X; 
			tmpY = (int)i*SF_Y;
			if (tmpY<H && tmpX<W)
				Output[tmpY*W+tmpX] = Image[i*W + j];
		}
	}
```

- 이렇게 짜고 돌리면 아래 이미지 처럼 이상하게 깨져서 나오는 것을 확인할 수 있다.

![](https://velog.velcdn.com/images/kansun12/post/bb127866-496c-4868-9784-ab001e1829e6/image.png)

- 가로 방향으로 1.3배, 세로방향으로 1.5배씩 늘려놨는데 원본영상에서 계싼된 위치만 화소가 채워지다보니까 주기적으로 홀이 생겨서 그런거임.

> **즉, 이렇게 순방향 사상으로 하면 오버랩이나 홀 문제가 발생하는 것을 확인한 것임.**

#### 역방향 사상을 이용한 스케일링

- 원본 영상이 화소를 하나씩 수서대로 검사하는 것이 아니라 출력 영상의 화소 위치를 탐색하면서 나한테 와야되는 애가 원본영상에서 누구인지를 보는  것

```c
//역방향 사상
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X);
			tmpY = (int)(i / SF_Y);
			if (tmpY<H && tmpX<W)
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
```

#### 최종 스케일링 함수

```c
void Scailing(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y) {
	//Scailing
	int tmpX, tmpY;
	//순방향사상
	/*for (int i = 0; i < H; i++) {
	for (int j = 0; j < W; j++) {
	tmpX = (int)(j*SF_X);
	tmpY = (int)(i*SF_Y);
	if (tmpY<H && tmpX<W)
	Output[tmpY*W+tmpX] = Image[i*W + j];
	}
	}*/

	//역방향 사상
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X);
			tmpY = (int)(i / SF_Y);
			if (tmpY<H && tmpX<W)
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}
```

![](https://velog.velcdn.com/images/kansun12/post/33a40870-ab2d-44b1-96ee-cae2b645d750/image.png)

그럼 이렇게 잘 처리된 것을 확인할 수 있다.

- `Scailing(Image, Output, W,H, 0.7, 0.7);` 함수에 X 스케일링 요소와 Y 스케일링 요소가 같은 것을 **Uniform Scailing**이라고 불림.

---

### Rotation

![](https://velog.velcdn.com/images/kansun12/post/6cadd578-b50e-4f46-994e-18173cc82ded/image.png)

기본 수식은 이런데 회전 행렬은

`R*I=O` 지금 이런 상태니까 사실상 역방향 사상으로 처리를 하니 **역행렬**을 이용해서  `I=R^(-1)*O`가 되겠다.

```c
//Rotation
	int tmpX, tmpY;
	int Angle = 30;
	double Radian = Angle*3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian)*j-sin(Radian)*i);
			tmpY = (int)(sin(Radian)*j + cos(Radian)*i);
			if ((tmpY<H && tmpY >= 0) && (tmpX < W &&tmpX >= 0))
				Output[tmpY*W + tmpX] = Image[i*W + j];
		}
	}
```

- 일단 **순방향사상**으로 코드를 짜보면

![](https://velog.velcdn.com/images/kansun12/post/794d6f8c-0de8-4bf4-b86c-575b9aad4ee6/image.png)

- 회전은 잘 됐는데 중간중간 중첩으로 인한 홀이 생긴 것을 발견할 수 있다.
- **역방향 사상대로 바꿔주자**

```c
//Rotation
	int tmpX, tmpY;
	int Angle = 30;
	double Radian = Angle*3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian)*j+sin(Radian)*i); //역방향 사상이니까 역행렬로 넣어주기
			tmpY = (int)(-sin(Radian)*j + cos(Radian)*i);
			if ((tmpY<H && tmpY >= 0) && (tmpX < W &&tmpX >= 0))
				Output[i*W + j] = Image[tmpY*W + tmpX]; //역방향
		}
	}
```

![](https://velog.velcdn.com/images/kansun12/post/7bbbef27-5877-43f8-b369-668e3084490b/image.png)

- 그럼 잘 나오는 것을 확인할 수 있다.

#### 센터 중심으로 회전하기

```c
void Rotaion(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
	//Rotation
	int tmpX, tmpY;
	double cx = W / 2.0;  // 영상 가로 중심
	double cy = H / 2.0;  //영상의 세로 중심
	double Radian = Angle*3.141592 / 180.0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian)*(j-cx) + sin(Radian)*(i-cy));
			tmpY = (int)(-sin(Radian)*(j - cx) + cos(Radian)*(i - cy));
			if ((tmpY<H && tmpY >= 0) && (tmpX < W &&tmpX >= 0))
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}
```

- 영상의 가로 중심과 영상의 세로 중심을 구해서 이걸 i와 j에서 뺴주어 원점에서 회전하는 것이 아닌 센터를 기준으로 돌아가도록 하였다.