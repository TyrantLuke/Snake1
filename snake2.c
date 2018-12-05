#include<stdio.h>
#include<stdlib.h>        
#include<conio.h>      
#include<windows.h>

//定义四个方向的对应字符
#define up 'w'
#define down 's'
#define left 'a'                         
#define right 'd'

#define SN sizeof(struct snake)
struct snake//结构体,内含蛇节点的坐标和其前、后节点的指针；
{
	int s_y;
	int s_x;
	struct snake * next;
	struct snake * formal;
}*head, *p1, *p2;

HANDLE hOutput, hOutBuf;//控制台屏幕缓冲区句柄
COORD coord = { 0,0 };
//双缓冲处理显示
DWORD bytes = 0;


char dir;                               //决定蛇头接下来的方向
char formal_dir;                         //记忆蛇头原来的方向
int li, num, f1, f2, b, a, y, x;
char map2[20][20];
int bgd[20][20] = 
			  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };//加墙地图 




int main()
{
	//用双缓冲解决闪屏问题；
	//创建新的控制台缓冲区
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	hOutput = CreateConsoleScreenBuffer(
		GENERIC_WRITE,//定义进程可以往缓冲区写数据
		FILE_SHARE_WRITE,//定义缓冲区可共享写权限
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	//隐藏两个缓冲区的光标
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(hOutput, &cci);
	SetConsoleCursorInfo(hOutBuf, &cci);

	void food();//生成食物
	void change_map2();
	void direction();//处理输入数据，转为蛇的走向
	void move();//根据蛇咬到了啥决定运动，伸长，结束游戏等。
	void print1();//用来展示缓冲区1
	void print2();//用来展示缓冲区2
	head = p1 = p2 = (struct snake*)malloc(SN);//动态分配内存
	head->s_y = 4;//初始位置为（4,4）
	head->s_x = 4;
	head->next = NULL;
	head->formal = NULL;
	srand((unsigned int)(time_t(NULL)));//将随机数种子设为系统时间
	f1 = rand() % 18 + 1;//f代表food，f1和f2分别表示食物的纵，横坐标
	srand(f1);
	f2 = rand() % 18 + 1;

	formal_dir = dir = 's', li = 1;//初始方向向下，生命值为1
	b = 4, a = 4;
	num = 0;//num代表得分，也即吃到的食物个数
	food();
	while (1)
	{
		formal_dir = dir;//这两步用来记住 此时方向，并得到 下一个方向
		if (_kbhit())
			dir = _getch();
		direction();
		move();
		change_map2();
		if (li == 0)
			break;
		print1();
		SetConsoleActiveScreenBuffer(hOutBuf);
		Sleep(300);

		formal_dir = dir;
		if (_kbhit())
			dir = _getch();
		direction();
		move();
		change_map2();
		if (li == 0)
			break;
		print2();
		SetConsoleActiveScreenBuffer(hOutput);
		Sleep(300);


	}

	printf("GAME OVER\nscore:%d\n", num);
	_getch();

	return 0;
}




void direction()
{
	if (dir == up && formal_dir == down)              //防止控制蛇往回走
		dir = formal_dir;
	else if (dir == down && formal_dir == up)
		dir = formal_dir;
	else if (dir == left && formal_dir == right)
		dir = formal_dir;
	else if (dir == right && formal_dir == left)
		dir = formal_dir;
	switch (dir)                                //蛇四面运动的实现
	{
	case up:b = b - 1; break;
	case down:b = b + 1; break;
	case left:a = a - 1; break;
	case right:a = a + 1; break;
	case 'p':while (1)
	{
		if ('p' == _getch())         //暂停的实现
			break;
	}
	default:                               //若为其他键输入，则无效化
	{dir = formal_dir;
	direction(); }
	}
}

void food()
{
	if (num != 0) {
		srand(f1);
	}
	f1 = rand() % 18 + 1;
	if (num != 0) {
		srand(f2);
	}
	f2 = rand() % 18 + 1;
	if (bgd[f1][f2] == 2 || bgd[f1][f2] == 3 || bgd[f1][f2] == 1)                           //食物是否和蛇头重合
		food();
	bgd[f1][f2] = 4;
}

void move() {
	if (bgd[b][a] == 1)                            //判断是否撞墙以及咬到自己
		li = 0;
	else if (bgd[b][a] == 3)
		li = 0;
	else if (bgd[b][a] == 0)                        //蛇的运动
	{
		bgd[head->s_y][head->s_x] = 0;
		if (num == 0)
		{
			head->s_y = b;
			head->s_x = a;
		}
		else                                   //将蛇的末尾部分给头部，达到移动效果
		{
			p1 = head;
			p1->s_y = b;
			p1->s_x = a;
			head = head->next;
			p1->next = NULL;
			bgd[p2->s_y][p2->s_x] = 3;
			p2->next = p1;
			p2 = p1;
		}
		bgd[b][a] = 2;
	}
	else if (bgd[b][a] == 4)                                   //吃到食物，扩展链表
	{
		p1 = (struct snake*)malloc(SN);
		p1->s_y = b;
		p1->s_x = a;
		p2->next = p1;
		bgd[p2->s_y][p2->s_x] = 3;
		p2 = p1;
		bgd[b][a] = 2;
		f1 = f1 * dir;
		f2 = f2 * dir;
		food();
		num++;
	}
}

void print1() {
	//打印，0：空位；1：墙；2：蛇头；3：蛇身；4：食物
	for (y = 0; y < 20; y++){
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutBuf, map2[y], 20, coord, &bytes);
	}
}
void print2() {
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, map2[y], 20, coord, &bytes);
	}
}

void change_map2() {
	for (y = 0; y < 20; y++)
	{
		for (x = 0; x < 20; x++)
		{
			if (bgd[y][x] == 1)
				map2[y][x] = '#';
			else if (bgd[y][x] == 0)
				map2[y][x] = ' ';
			else if (bgd[y][x] == 2)
				map2[y][x] = '@';
			else if (bgd[y][x] == 3)
				map2[y][x] = '0';
			else if (bgd[y][x] == 4)
				map2[y][x] = '$';
		}
	}
}
