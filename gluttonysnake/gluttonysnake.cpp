/******************************************************************
		̰ʳ�� Preview
		�˰汾̰�������ߵķ����Ǽ����õ�������
		���ƶ�����Sleep()��Ͻ���ʵ�ֵ�
		�����ƶ����ƽ�Ϊ���٣��������������Ч�ʽϵͣ�
		�ʲ�������ʮ��һ��
*******************************************************************/
#include<windows.h>
#include <time.h>

#pragma comment(lib,"winmm.lib")

#define LENGTH 40					//��Ϸ���صĿ�
#define WIDTH 10					//����ߵĵ������εı߳�
#define RANGE 50					//��Ϸ������ͻ�ȥ֮����
#define SNAKE_COLOR RGB(255,0,0)	  //�ߵ���ɫ
#define BK_COLOR	RGB(204,232,207)  //���屳��ɫ
#define NO_SNAKE 0
#define HAS_SNAKE 1
#define STEP 2
#define MAKECOOR(x) (x)*WIDTH		//��flags������±�ӳ��Ϊ����

typedef struct
{
	int x;
	int y;
	int flag;
} GRID;//�ߵĽṹ

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void MakePartSnake(int,int,int,GRID *);	//Ϊ�������ӻ�����һ������
void MoveSnake(HWND);				//�ƶ���
void MakeFood(GRID *food);			//�����������
void initializer();				//��ʼ����Ϸ
void Manager();

TCHAR szAppName[] = TEXT("Gluttony Snake");
int flags[LENGTH][LENGTH];	//��Ϸ�������з����״̬���
RECT playground;		//��Ϸ����
GRID *snake = NULL;	//������λ������
GRID *food = NULL;	//ʳ�����
static int MAX_LENGTH = 0;//Ĭ���߷������󳤶ȣ��ɱ�ģ�
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
		//PlaySound(TEXT("HG.wav"),NULL ,SND_ASYNC | SND_LOOP);//������Ϸ���֣�
		initializer();
		MoveWindow(hwnd,RANGE * 2,RANGE * 2,WIDTH * LENGTH + RANGE * 3,WIDTH * LENGTH + RANGE * 3,TRUE);
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
		Rectangle(hdc,playground.left,playground.top,playground.right,playground.bottom);
		DeleteObject(hBrush);
		hBrush = CreateSolidBrush(SNAKE_COLOR);
		SelectObject(hdc,hBrush);
		for(int i = 0; i != LENGTH;++i)
		{
			for(int j = 0; j != LENGTH;++j)
			{
				if(flags[i][j] == HAS_SNAKE)
				{
					Rectangle(hdc,MAKECOOR(i),MAKECOOR(j),MAKECOOR(i+1),MAKECOOR(j+1));
				}
			}
		}
		DeleteObject(hBrush);
		EndPaint(hwnd,&ps);
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}
//////////////////////////��ʼ����Ϸ��������////////////////////////////////////////////////////////////
void initializer()
{
	if(snake != NULL)
		free(snake);
	if(food != NULL)
		free(food);
	snake_len = 3;		//�ߵĳ�ʼ����Ϊ3
	direct = VK_RIGHT;	//�ߵĳ�ʼ����Ϊ����
	MAX_LENGTH = 100;	//��Ĭ�����������󳤶�Ϊ20
	food = (GRID *)calloc(1,sizeof(GRID));//���䴢��food���ڴ�
	snake = (GRID *)calloc(MAX_LENGTH,sizeof(GRID));//���䴢���ߵ��ڴ�
	/*************��ʼ����Ϸ����*******************/
	playground.left = 0;
	playground.top = 0;
	playground.right = WIDTH * LENGTH;
	playground.bottom = WIDTH * LENGTH;
	/**************��ʼ����Ϸ����********************/

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
	}///��ʼ����
	MakeFood(food);///����food
}

void MakePartSnake(int x,int y,int flag,GRID * snake)
{
	snake->x = x;
	snake->y = y;
	snake->flag = flag;
}
////////////////////////////////////////////////////////////////////////////////



//////////////////�����ߵ��ƶ�///////////////////////
void MoveSnake(HWND hwnd)
{
	Manager();
	flags[(snake + snake_len - 1)->x][(snake + snake_len - 1)->y] = NO_SNAKE;//���ߵ�β��ȥ��������Ϊ���ƶ�һ��

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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////ͨ���������food
void MakeFood(GRID *food)
{
	srand((unsigned) time(NULL));
	food->x = rand() % LENGTH;
	food->y = rand() % LENGTH;
	food->flag = HAS_SNAKE;
	flags[food->x][food->y] = food->flag;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////��Ϸ�����ӳ�估�ڴ�Ĵ���///////////////////////////////////////////////////
void Manager()
{
	if(snake_len >= MAX_LENGTH - 2)
	{
		MAX_LENGTH += STEP;
		snake = (GRID *)realloc(snake,MAX_LENGTH * sizeof(GRID));
	}//���ߵĳ�������snake��������������ڴ���ٴ��������

	if(snake->x < 0 || snake->x >= LENGTH || snake->y < 0 || snake->y >= LENGTH)
	{
		MessageBox(NULL,TEXT("Game Over!!"),szAppName,0);
		initializer();
		return;
	}///�ж����Ƿ������߽�

	for(int i = 4;i < snake_len;++i)
	{
		if(snake->x == (snake + i)->x && snake->y == (snake + i)->y)
		{
			MessageBox(NULL,TEXT("Game Over!!"),szAppName,0);
			initializer();
			return;
		}
	}////�ж����Ƿ���������

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
	}///�ж����Ƿ�Ե�food
}
////////////////////////////////////////////////////////////////////////////////