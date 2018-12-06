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
} *p1, *p2,*tail;

HANDLE hOutput, hOutBuf;//控制台屏幕缓冲区句柄
COORD coord = { 0,0 };
//双缓冲处理显示
DWORD bytes = 0;


char dir;                               //决定蛇头接下来的方向
char formal_dir;                         //记忆蛇头原来的方向
int li, num, f1, f2, b, a, y, x, i = 0, dif = 300;
int poison[5][2] = { 0 };
char map2[20][20];
char bgd[20][36] =
{ "####################--------------|",
"#                  #     Snake    |",
"#                  #--------------|",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"#                  #              |",
"####################--------------|" };//画地图，规定#为墙，O为蛇身，@为蛇头，$为食物，毒草为V，地雷为X

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
	void bomb_and_poison();//生成毒草和地雷
	void direction();//处理输入数据，转为蛇的走向
	void move();//根据蛇咬到了啥决定运动，伸长，结束游戏等。
	void print1();//用来展示缓冲区1
	void print2();//用来展示缓冲区2
	tail= p1 = p2 = (struct snake*)malloc(SN);//动态分配内存
	p1->s_y = 4;//初始位置为（4,4）
	p1->s_x = 4;
	p1->next = NULL;
	p1->formal = NULL;
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
		if (li == 0)
			break;
		print1();
		SetConsoleActiveScreenBuffer(hOutBuf);
		if (num == 20)
			break;
		Sleep(dif);

		//另一块画布（同样的操作。）
		formal_dir = dir;
		if (_kbhit())
			dir = _getch();
		direction();
		move();
		if (li == 0)
			break;
		print2();
		SetConsoleActiveScreenBuffer(hOutput);
		if (num == 20)
			break;
		Sleep(dif);
	}

	_getch();

	return 0;
}

void direction()
{
	if (dir != -32) {//有两套控制方向的键，第一套：wsad
		if (dir == up && formal_dir == down)//防止控制蛇往回走
			dir = formal_dir;
		else if (dir == down && formal_dir == up)
			dir = formal_dir;
		else if (dir == left && formal_dir == right)
			dir = formal_dir;
		else if (dir == right && formal_dir == left)
			dir = formal_dir;
		switch (dir) {//蛇四面运动的实现
		case up:b = b - 1; break;
		case down:b = b + 1; break;
		case left:a = a - 1; break;
		case right:a = a + 1; break;
		case 'p':while (1) {
			if ('p' == _getch())         //暂停的实现
				break;
		}
		default: {//若为其他键输入，则无效化
			dir = formal_dir;
			direction();
		}
		}
	}
	else {//第二套，上下左右
		switch (dir = _getch()) {
		case 72:dir = up; break;
		case 80:dir = down; break;
		case 75:dir = left; break;
		case 77:dir = right; break;
		default:break;
		}
		direction();
		}
}

void food()
{
	if (num != 0) {
		srand(f1*(int)(dir)+1);
		f1 = rand() % 18 + 1;
		srand(f2*(int)dir);
		f2 = rand() % 18 + 1;
	}
	if (bgd[f1][f2]!=' ')     //食物是否和蛇头,墙壁或蛇身或毒草，炸弹重合
		food();
	else 
		bgd[f1][f2] = '$';
}

void loop_poison(){ //控制毒草生成
		srand(f1*(int)dir);
		f1 = rand() % 18 + 1;
		srand(f2*(int)dir);
		f2 = rand() % 18 + 1;
	if (bgd[f1][f2] != ' ')
		loop_poison();
	else
		bgd[f1][f2] = 'V';
}

void loop_bomb() {//控制地雷生成
	srand(f1);
	f1 = rand() % 18 + 1;
	srand(f2);
	f2 = rand() % 18 + 1;
	if (bgd[f1][f2] != ' ')
		loop_bomb();
	else
		bgd[f1][f2] = 'X';
}

void bomb_and_poison(){
	void loop_poison();
	void loop_bomb();
	if (num >5) {//吃到的食物大于5个，达到毒草生成条件
		if (num % 3 == 0){//毒草在吃完每3个食物时刷新一次
			if(poison[0][0]!=0)
				for (i = 0; i <= 4; i++)//把之前的毒草都消掉
					if(bgd[poison[i][0]][poison[i][1]]=='V')
						bgd[poison[i][0]][poison[i][1]] = ' ';
			for (i = 0; i <= 4; i++) {
				loop_poison();
				poison[i][0] = f1;
				poison[i][1] = f2;
			}
		}
	}
	if (num > 10 && num % 3 == 2)//吃到的食物大于十个，达到地雷生成条件，每吃3个食物生成1个
		loop_bomb();
}


void move() {
	if (bgd[b][a] == '#') //判断是否撞墙
		li = 0;
	else if (bgd[b][a] == 'O')//是否咬到自己
		li = 0;
	else if (bgd[b][a] == ' ')//蛇的运动要来了！！
	{
		if (num == 0)//一开始啥都没吃到的时候。。。
		{
			bgd[p1->s_y][p1->s_x] = ' ';
			p1->s_y = b;
			p1->s_x = a;
		}
		else//有蛇身的时候
		{
			bgd[tail->s_y][tail->s_x] = ' ';//去掉蛇尾
			bgd[p2->s_y][p2->s_x] = 'O';//原来的蛇头变成蛇身

			p1 = (struct snake*)malloc(SN);//来个蛇头
			p1->s_y = b;//联结起来
			p1->s_x = a;
			p1->next = NULL;
			p1->formal = p2;
			p2->next = p1;
			p2 = p1;
			tail = tail->next;//蛇尾移动
			free(tail->formal);//原来的尾节点释放
			tail->formal = NULL;
		}
		bgd[b][a] = '@';//蛇头打印出来
	}
	else if (bgd[b][a] == '$') //吃到食物，扩展链表
	{
		p1 = (struct snake*)malloc(SN);//建立新节点并联结
		p1->s_y = b;
		p1->s_x = a;
		p1->formal = p2;
		p1->next = NULL;
		p2->next = p1;
		bgd[p2->s_y][p2->s_x] = 'O';//更改蛇身字符
		bgd[b][a] = '@';//蛇头字符
		p2 = p1;
		num++;//吃到食物计数
		if (num % 4 == 0 && dif > 0)
			dif = dif - 40;
		food();//刷新食物
		bomb_and_poison();
	}
	else if (bgd[b][a] == 'V') {
		num--;//分数减1
		if (num < 0)
			li = 0;
		else if (dif > 0){
			dif = dif - 20;
			p1 = (struct snake*)malloc(SN);//蛇头和前面蛇的移动的代码完全一样
			p1->s_y = b;
			p1->s_x = a;
			p1->next = NULL;
			p1->formal = p2;
			p2->next = p1;
			bgd[p2->s_y][p2->s_x] = 'O';//更改蛇身字符
			bgd[b][a] = '@';//蛇头字符
			p2 = p1;
			bgd[tail->s_y][tail->s_x] = ' ';//这两行将最后两个节点去掉
			bgd[(tail->next)->s_y][(tail->next)->s_x] = ' ';
			tail = (tail->next)->next;
			free((tail->formal)->formal);
			free(tail->formal);
			tail->formal = NULL;
		}
	}
}

void print1() {
	for (y = 0; y < 20; y++){
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutBuf, bgd[y], 36, coord, &bytes);
	}
}
void print2() {
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, bgd[y], 36, coord, &bytes);
	}
}
