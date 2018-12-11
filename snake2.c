#include<stdio.h>
#include<stdlib.h>        
#include<conio.h>      
#include<windows.h>
#include<mmsystem.h>//导入声音头文件
#pragma comment(lib, "winmm.lib")
#include"clear.h"
#include"struct.h"
//定义四个方向的对应字符
#define up 'w'
#define down 's'
#define left 'a'                         
#define right 'd'

#define SN sizeof(struct snake)
struct snake *p1, *p2, *tail;//结构体,内含蛇节点的坐标和其前、后节点的指针；

HANDLE hOutput, hOutBuf;//控制台屏幕缓冲区句柄
COORD coord = { 0,0 };
//双缓冲处理显示
DWORD bytes = 0;

FILE * level=NULL;
char dir;                               //决定蛇头接下来的方向
char formal_dir;                         //记忆蛇头原来的方向
int li, num, f1, f2, b, a, y, x=1, z,q, c = 0, i = 0, dif = 300;
int lv=1;//难度等级初始设为1
int poison[5][2] = { 0 };
char map2[20][20];
char bgd[20][36] =
{ "####################--------------|",
"#                  #     Snake    |",
"#                  #--------------|",
"#                  # wsad or arrow|",
"#                  # keys to contr|",
"#                  # ol the snake;|",
"#                  #  p to pause, |",
"#                  #begin or exit.|",
"#                  #              |",
"#                  # $: food      |",
"#                  # V: poison    |",
"#                  # X: bomb      |",
"#                  #              |",
"#                  #score 00 / 20 |",
"#                  #              |",
"#                  #Lv:  1        |",
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
	void choise();//界面选择相关
	void welcome();//贪吃蛇初始欢迎界面
	void free_chain();//将链表释放
	void resornot();//一关结束后的处理
	void clear();
	void lv1();//分别表示第1,2,3关的结束处理
	void lv2();
	void lv3();

	choise();//用户选择界面

loop://以便进入下一关

	if (lv == 2) {//进入难度2
		lv1();
	}
	else if (lv == 3) {//进入难度3
		lv2();
	}
	PlaySound("01.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);//循环播放背景音乐
	tail = p1 = p2 = (struct snake*)malloc(SN);//动态分配内存
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
		bgd[13][26] = num / 10 + 48;
		bgd[13][27] = num % 10 + 48;
		bgd[15][25] = lv + 48;
		if (li == 0)
			break;
		print1();
		SetConsoleActiveScreenBuffer(hOutBuf);
		if (num == 20)
			break;
		Sleep(dif);

		if (c == 0) {//让玩家开始游戏
			while (_getch() != 'p') {
			}
			c++;
		}

		//另一块画布（同样的操作。）
		formal_dir = dir;
		if (_kbhit())
			dir = _getch();
		direction();
		move();
		bgd[13][26] = num / 10 + 48;
		bgd[13][27] = num % 10 + 48;
		if (li == 0)
			break;
		print2();
		SetConsoleActiveScreenBuffer(hOutput);
		if (num == 20)
			break;
		Sleep(dif);
	}
	if (num < 20) {//意外死亡
		PlaySound("02.wav", NULL, SND_SYNC | SND_NODEFAULT);//结束音	
		while (_getch() != 'p') {}
		free_chain();
		clear();
		x = 1, c = 0, i = 0, dif = 300,lv=1;
		choise();
		goto loop;
	}
	else {//达成任务通关
		PlaySound("03.wav", NULL, SND_SYNC | SND_NODEFAULT);//通关音
			resornot();
		if (lv == 4) {//完成最终难度
			lv3();
			while (_getch() != 'p') {}
			free_chain();
			clear();
			x = 1, c = 0, i = 0, dif = 300, lv = 1;
			choise();
			goto loop;//清洗数据并重开游戏
		}
		else
			goto loop;//重开游戏
	}

	return 0;
}

void resornot() {//清除地图，释放内存
	void clear();//清屏
	void free_chain();//将链表释放
	if (_getch() == 'p') {
		free_chain();
		clear();
		x = 1, c = 0, i = 0, dif = 300;
		lv++;
	}
	else
		resornot();
	}


void direction()
{
	char q;
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
			if ((q = _getch()) == 'p')         //暂停的实现
				break;
			else if (q == 27) {
				level = fopen("mod.txt", "w");
				putc(lv, level);
				fclose(level);
				level = NULL;
				exit(0);
			}
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
		srand(f2*(int)dir+1);
		f2 = rand() % 18 + 1;
	}
	if (bgd[f1][f2] != ' ')     //食物是否和蛇头,墙壁或蛇身或毒草，地雷重合
		food();
	else
		bgd[f1][f2] = '$';
}

void loop_poison() { //控制毒草生成
	f1 = rand() % 18 + 1;
	f2 = rand() % 18 + 1;
	if (bgd[f1][f2] != ' ')
		loop_poison();
	else
		bgd[f1][f2] = 'V';
}

void loop_bomb() {//控制地雷生成
	f1 = rand() % 18 + 1;
	f2 = rand() % 18 + 1;
	if (bgd[f1][f2] != ' ')
		loop_bomb();
	else
		bgd[f1][f2] = 'X';
}

void bomb_and_poison() {
	void loop_poison();
	void loop_bomb();
	if (num > 5) {//吃到的食物大于5个，达到毒草生成条件
		if (num % 3 == 0) {//毒草在吃完每3个食物时刷新一次
			if (poison[0][0] != 0)
				for (i = 0; i <= 4; i++)//把之前的毒草都消掉
					if (bgd[poison[i][0]][poison[i][1]] == 'V')
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
	if (bgd[b][a] == '#'||bgd[b][a]=='*') //判断是否撞墙
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
		dif = dif - 5;
		food();//刷新食物
		bomb_and_poison();
	}
	else if (bgd[b][a] == 'V') {//吃到毒草，减链表长度，增速；
		num--;//分数减1
		if (num < 0)
			li = 0;
		else {
			if (dif > 0) //增速
				dif = dif - 20;
			p1 = (struct snake*)malloc(SN);//蛇头和前面蛇的移动的代码完全一样
			p1->s_y = b;
			p1->s_x = a;
			p1->next = NULL;
			p1->formal = p2;
			p2->next = p1;
			bgd[p2->s_y][p2->s_x] = 'O';
			bgd[b][a] = '@';
			p2 = p1;
			bgd[tail->s_y][tail->s_x] = ' ';//这两行将最后两个节点去掉
			tail = tail->next;
			free(tail->formal);
			tail->formal = NULL;
			bgd[tail->s_y][tail->s_x] = ' ';
			tail = tail->next;
			free(tail->formal);
			tail->formal = NULL;
		}
	}
	else if (bgd[b][a] == 'X') {//踩到地雷，当长度为1时死亡。否则失去接近一半的长度，且速度稍稍减慢；
		p1 = (struct snake*)malloc(SN);//同上，先处理蛇头
		p1->s_y = b;
		p1->s_x = a;
		p1->next = NULL;
		p1->formal = p2;
		p2->next = p1;
		bgd[p2->s_y][p2->s_x] = 'O';
		bgd[b][a] = '@';
		p2 = p1;
		if (num <= 1)//当长度为1
			li = 0;
		else {//长度不为1，失去后半段蛇身
			dif = dif + 50;
			for (z = 1; z <= 2 * num / 3; z++) {
				bgd[tail->s_y][tail->s_x] = ' ';
				tail = tail->next;
				free(tail->formal);
				tail->formal = NULL;
			}
			num = num - 2 * num / 3;
		}
	}
}

void print1() {
	for (y = 0; y < 20; y++) {
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

char wel[20][36] = {
	"***********************************",
	"*                                 *",
	"*            S N A K E            *",
	"*                                 *",
	"***********************************",
	"*                                 *",
	"*                                 *",
	"*      =>   N E W  G A M E        *",
	"*                                 *",
	"*                                 *",
	"*           C O N T I N U E       *",
	"*                                 *",
	"*                                 *",
	"*           M O R E :)            *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                      (press P)  *",
	"***********************************"
	};

char more[20][36] = {
	"***********************************",
	"*                                 *",
	"*            S N A K E            *",
	"*                                 *",
	"***********************************",
	"*                                 *",
	"*   THANKS TO:                    *",
	"*   Planner : ZL                  *",
	"*   Programmer : ZL               *",
	"*   Arts : ZL                     *",
	"*   Music : ZL                    *",
	"*   Polish : ZL                   *",
	"*   Publish : ZL                  *",
	"*                                 *",
	"*   and YOU,my friend!            *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                      (press P)  *",
	"***********************************"
};

void welcome() {
	SetConsoleActiveScreenBuffer(hOutput);
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, wel[y], 36, coord, &bytes);
	}
	SetConsoleActiveScreenBuffer(hOutput);
}

void choise() {
	welcome();//打印界面
	char co;
	co = _getch();//得到输入值
	if (co== 'p'&&x == 1) {}
	else if (co== 'p'&&x == 2) {
		level = fopen("mod.txt", "r");
		lv = getc(level);
		fclose(level);
		level = NULL;
	}
	else if (co == 'p'&&x == 3) {
		char co2;
    	SetConsoleActiveScreenBuffer(hOutput);
		for (y = 0; y < 20; y++) {
			coord.Y = y;
			WriteConsoleOutputCharacterA(hOutput, more[y], 36, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);
		while((co2=_getch())!='p'){}
		choise();
	}
	else if (co == 's') {//下移
		if (x == 3) {
			x = 1;
			wel[13][7] = ' ';
			wel[13][8] = ' ';
			wel[7][7] = '=';
			wel[7][8] = '>';
		}
		else if (x == 1) {
			x = 2;
			wel[7][7] = ' ';
			wel[7][8] = ' ';
			wel[10][7] = '=';
			wel[10][8] = '>';
		}
		else if (x == 2) {
			x = 3;
			wel[10][7] = ' ';
			wel[10][8] = ' ';
			wel[13][7] = '=';
			wel[13][8] = '>';
		}
		choise();
	}
	else if (co == 'w') {//上移
		if (x == 3) {
			x = 2;
			wel[13][7] = ' ';
			wel[13][8] = ' ';
			wel[10][7] = '=';
			wel[10][8] = '>';
		}
		else if (x == 1) {
			x = 3;
			wel[7][7] = ' ';
			wel[7][8] = ' ';
			wel[13][7] = '=';
			wel[13][8] = '>';
		}
		else if (x == 2) {
			x = 1;
			wel[10][7] = ' ';
			wel[10][8] = ' ';
			wel[7][7] = '=';
			wel[7][8] = '>';
		}
		choise();
	}
	else if (co == 27) {//按esc退出游戏
		exit(0);
	}
	else if (co== -32) {
		char eo;
		eo = _getch();
		if (eo== 72) {
			if (x == 3) {
				x = 2;
				wel[13][7] = ' ';
				wel[13][8] = ' ';
				wel[10][7] = '=';
				wel[10][8] = '>';
			}
			else if (x == 1) {
				x = 3;
				wel[7][7] = ' ';
				wel[7][8] = ' ';
				wel[13][7] = '=';
				wel[13][8] = '>';
			}
			else if (x == 2) {
				x = 1;
				wel[10][7] = ' ';
				wel[10][8] = ' ';
				wel[7][7] = '=';
				wel[7][8] = '>';
			}
		}
		else if (eo == 80) {
			if (x == 3) {
				x = 1;
				wel[13][7] = ' ';
				wel[13][8] = ' ';
				wel[7][7] = '=';
				wel[7][8] = '>';
			}
			else if (x == 1) {
				x = 2;
				wel[7][7] = ' ';
				wel[7][8] = ' ';
				wel[10][7] = '=';
				wel[10][8] = '>';
			}
			else if (x == 2) {
				x = 3;
				wel[10][7] = ' ';
				wel[10][8] = ' ';
				wel[13][7] = '=';
				wel[13][8] = '>';
			}
		}
		choise();
	}
	else choise();
}

void free_chain() {
	while ((tail ->next)!= NULL) {
		tail = tail->next;
		free(tail->formal);
	}
}

void clear() {
	int c_y, c_x;
	for (c_y = 1; c_y <= 18; c_y++)
		for (c_x = 1; c_x <= 18; c_x++)
			bgd[c_y][c_x] = ' ';
}

void lv1() {
	int lt;
	for (lt = 1; lt <= 6; lt++) {
		bgd[12][lt] = '#';
		bgd[lt][7] = '#';
		bgd[7][19 - lt] = '#';
		bgd[19 - lt][12] = '#';
	}
	bgd[9][9] = '#';
	bgd[9][10] = '#';
	bgd[10][9] = '#';
	bgd[10][10] = '#';
}

void lv2() {
	char bgd3[21][21] = {
"********************",
"*                  *",
"*  ****     * * *  *",
"*  *  *       *    *",
"*           * * *  *",
"*                  *",
"*                  *",
"*       *  *       *",
"*   *   *  *   *   *",
"*       *  *       *",
"* *    *    *    * *",
"*     **    **     *",
"*       *  *       *",
"*   *   *  *   *   *",
"*      *    *      *",
"*     *      *     *",
"*    *        *    *",
"*   *          *   *",
"*   *          *   *",
"********************"
	};
	int lt,lm;
	for (lt = 1; lt <= 18; lt++)
		for (lm = 1; lm <= 18; lm++)
			bgd[lt][lm] = bgd3[lt][lm];
}

void lv3() {
	clear();
	int lt,lm;
	char bgd3[21][21] = {
"####################",
"#                  #",
"#                  #",
"#                  #",
"#                  #",
"#                  #",
"#  G R E A T !!!   #",
"#                  #",
"#                  #",
"#YOU WON THE GAME !#",
"#      __  __      #",
"#     /  \/  \       #",
"#     \      /      #",
"#      \    /       #",
"#       \  /        #",
"#        \/         #",
"#                  #",
"#    THANK YOU!    #",
"#                  #",
"####################"
	};
	for (lt = 0; lt <= 19; lt++)
		for (lm = 0; lm <= 19; lm++)
			bgd[lt][lm] = bgd3[lt][lm];
	SetConsoleActiveScreenBuffer(hOutput);
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, bgd[y], 36, coord, &bytes);
	}
	SetConsoleActiveScreenBuffer(hOutput);
}
