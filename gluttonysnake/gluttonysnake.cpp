/******************************************************************
		贪食蛇 Preview
		此版本贪吃蛇用蛇的方块是集合用的是数组
		蛇移动是以Sleep()阻断进程实现的
		故蛇移动看似较为卡顿，且数组插入数据效率较低，
		故操作体验十分一般
*******************************************************************/
#include<windows.h>
#include <time.h>

#pragma comment(lib,"winmm.lib")

#define LENGTH 40					//游戏场地的宽
#define WIDTH 10					//组成蛇的的正方形的边长
#define RANGE 50					//游戏场地与客户去之间间隔
#define SNAKE_COLOR RGB(255,0,0)	  //蛇的颜色
#define BK_COLOR	RGB(204,232,207)  //窗体背景色
#define NO_SNAKE 0
#define HAS_SNAKE 1
#define STEP 2
#define MAKECOOR(x) (x)*WIDTH		//把flags数组的下标映射为坐标

typedef struct
{
	int x;
	int y;
	int flag;
} GRID;//蛇的结构

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void MakePartSnake(int,int,int,GRID *);	//为蛇身增加或设置一个方块
void MoveSnake(HWND);				//移动蛇
void MakeFood(GRID *food);			//制造随机方块
void initializer();				//初始化游戏
void Manager();

TCHAR szAppName[] = TEXT("Gluttony Snake");
int flags[LENGTH][LENGTH];	//游戏区域所有方块的状态标记
RECT playground;		//游戏场地
GRID *snake = NULL;	//蛇所在位置数组
GRID *food = NULL;	//食物对象
static int MAX_LENGTH = 0;//默认蛇分配的最大长度（可变的）
int snake_len = 0;
int direct = 0;
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   PSTR szCmdLine,
				   int iCmdShow)
{
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;
	while(TRUE)
	{
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hbrBackground=CreateSolidBrush(RGB(203,202,201));
		wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
		wndclass.hInstance = hInstance;
		wndclass.lpfnWndProc = WndProc;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;
		wndclass.style = CS_VREDRAW | CS_HREDRAW;
		
		if(!RegisterClass(&wndclass))
		{
			MessageBox(NULL,TEXT("Register class failed!! Retry??"),szAppName,0);
			return 0;
		}
		break;
	}
	hwnd = CreateWindow(szAppName,
			TEXT("Gluttony Snake Preview V1.0.1"),
			WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MINIMIZEBOX ^ WS_MAXIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,NULL,
			hInstance,
			NULL);

	ShowWindow(hwnd,SW_NORMAL);
	UpdateWindow(hwnd);
	while(TRUE)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			MoveSnake(hwnd);
		}
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
		//PlaySound(TEXT("HG.wav"),NULL ,SND_ASYNC | SND_LOOP);//播放游戏音乐；
		initializer();
			MoveWindow(hwnd,
			RANGE * 2,
			RANGE * 2,
			WIDTH * LENGTH + RANGE * 3,
			WIDTH * LENGTH + RANGE * 3,
			TRUE);
		return 0;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_LEFT:
			if(direct != VK_RIGHT)
				direct = VK_LEFT;
			break;
		case VK_RIGHT:
			if(direct != VK_LEFT)
				direct = VK_RIGHT;
			break;
		case VK_UP:
			if(direct != VK_DOWN)
				direct = VK_UP;
			break;
		case VK_DOWN:
			if(direct != VK_UP)
				direct = VK_DOWN;
			break;
		default:
			break;
		}
		return 0;

	case WM_PAINT:
		for(int i = 0; i != snake_len; ++i)
		{
			flags[(snake + i)->x][(snake + i)->y] = (snake + i)->flag;
		}
		hdc = BeginPaint(hwnd,&ps);
		SetViewportOrgEx(hdc,RANGE/2,RANGE,NULL);
		hBrush = CreateSolidBrush(BK_COLOR);
		SelectObject(hdc,hBrush);
		Rectangle(hdc,
			playground.left,
			playground.top,
			playground.right,
			playground.bottom);
		DeleteObject(hBrush);
		hBrush = CreateSolidBrush(SNAKE_COLOR);
		SelectObject(hdc,hBrush);
		for(int i = 0; i != LENGTH;++i)
		{
			for(int j = 0; j != LENGTH;++j)
			{
				if(flags[i][j] == HAS_SNAKE)
				{
					Rectangle(hdc,
						MAKECOOR(i),
						MAKECOOR(j),
						MAKECOOR(i+1),
						MAKECOOR(j+1));
				}
			}
		}
		DeleteObject(hBrush);
		EndPaint(hwnd,&ps);
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}
//初始化游戏各个参数
void initializer()
{
	if(snake != NULL)
		free(snake);
	if(food != NULL)
		free(food);
	snake_len = 3;		//蛇的初始长度为3
	direct = VK_RIGHT;	//蛇的初始方向为向右
	MAX_LENGTH = 100;	//蛇默认最初分配最大长度为20
	food = (GRID *)calloc(1,sizeof(GRID));//分配储存food的内存
	snake = (GRID *)calloc(MAX_LENGTH,sizeof(GRID));//分配储存蛇的内存
	/*************初始化游戏场地*******************/
	playground.left = 0;
	playground.top = 0;
	playground.right = WIDTH * LENGTH;
	playground.bottom = WIDTH * LENGTH;
	/**************初始化游戏场地********************/

	for(int i = 0 ;i < LENGTH;++i)
	{
		for(int j = 0; j < LENGTH;++j)
		{
			flags[i][j] = NO_SNAKE;
		}
	}
	for(int i = 0; i != snake_len;++i)
	{
		MakePartSnake(LENGTH / 2 + 2 - i,LENGTH / 2,HAS_SNAKE,(snake + i));
	}///初始化蛇
	MakeFood(food);///产生food
}

void MakePartSnake(int x,int y,int flag,GRID * snake)
{
	snake->x = x;
	snake->y = y;
	snake->flag = flag;
}



//控制蛇的移动
void MoveSnake(HWND hwnd)
{
	Manager();
	//把蛇的尾部去除（表现为蛇移动一格）
	flags[(snake + snake_len - 1)->x][(snake + snake_len - 1)->y] = NO_SNAKE;

	for(int i = snake_len - 1; i > 0;--i)
	{
		(snake+i)->x = (snake + i -1)->x;
		(snake+i)->y = (snake + i -1)->y;
	}
	if(direct == VK_LEFT)
	{
		snake->x -= 1;
	}
	if(direct == VK_RIGHT)
	{
		snake->x += 1;
	}
	if(direct == VK_UP)
	{
		snake->y -= 1;
	}
	if(direct == VK_DOWN)
	{
		snake->y += 1;
	}
	InvalidateRect(hwnd,NULL,FALSE);
	Sleep(200);
}

//通过随机生成food
void MakeFood(GRID *food)
{
	srand((unsigned) time(NULL));
	food->x = rand() % LENGTH;
	food->y = rand() % LENGTH;
	food->flag = HAS_SNAKE;
	flags[food->x][food->y] = food->flag;
}

//游戏规则的映射及内存的处理
void Manager()
{
	if(snake_len >= MAX_LENGTH - 2)
	{
		MAX_LENGTH += STEP;
		snake = (GRID *)realloc(snake,MAX_LENGTH * sizeof(GRID));
	}//若蛇的长度数组snake将超过所分配的内存就再次扩充分配

	if(snake->x < 0 || snake->x >= LENGTH || snake->y < 0 || snake->y >= LENGTH)
	{
		MessageBox(NULL,TEXT("Game Over!!"),szAppName,0);
		initializer();
		return;
	}///判断蛇是否碰到边界

	for(int i = 4;i < snake_len;++i)
	{
		if(snake->x == (snake + i)->x && snake->y == (snake + i)->y)
		{
			MessageBox(NULL,TEXT("Game Over!!"),szAppName,0);
			initializer();
			return;
		}
	}////判断蛇是否碰到自身

	if(food->x == snake->x && food->y == snake->y)
	{
		MakePartSnake(
					(snake + snake_len - 1)->x
					,(snake + snake_len - 1)->y
					,HAS_SNAKE
					,snake + snake_len
					);
		++snake_len;
		MakeFood(food);
	}///判断蛇是否吃到food
}