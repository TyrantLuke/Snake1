#include<stdio.h>
#include<stdlib.h>        
#include<conio.h>      
#include<windows.h>
#include<mmsystem.h>//导入声音头文件
#pragma comment(lib, "winmm.lib")
//定义四个方向的对应字符
#define up 'w'
#define down 's'
#define left 'a'                         
#define right 'd'

#define SN sizeof(struct snake)
struct snake
{
	int s_y;
	int s_x;
	struct snake * next;
	struct snake * formal;
};

struct snake *p1, *p2, *tail;//结构体,内含蛇节点的坐标和其前、后节点的指针；

HANDLE hOutput, hOutBuf;//控制台屏幕缓冲区句柄
COORD coord = { 0,0 };
//双缓冲处理显示
DWORD bytes = 0;

FILE * level=NULL;//储存关卡等级
FILE * list = NULL;//储存通关人名（排行榜）
FILE * temporary = NULL;//暂时储存人名（便于下次继续游戏）
FILE * tem_score = NULL;// 临时 储存分数
FILE * file_score = NULL;//储存 最终 分数
char name[13];
char dir;                               //决定蛇头接下来的方向
char formal_dir;                         //记忆蛇头原来的方向g
int li, num, f1, f2, b, a, y, x=1, z,q, c = 0, i = 0, dif = 300;//dif为运动间隔时间
int lv=1;//难度等级初始设为1
int loop_num=0;//轮数，用于计算分数
int i_bomb = 0;//计算吃到的地雷数
int i_poison = 0;//计算吃到的毒草数
int score=0;//分数
int poison[5][2] = { 0 };
char map2[20][20];

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
   "*           RANKING LIST          *",
   "*                                 *",
   "*                                 *",
   "*           M O R E :)            *",
   "*                                 *",
   "*                      (press P)  *",
   "***********************************"
};
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
	system("mode con cols=35 lines=20");//调整窗口大小
	system("color 6F");//改变颜色
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
	void puts_name();//输入名字
	void lv1();//分别表示第1,2,3关的结束处理
	void lv2();
	void lv3();

	choise();//用户选择界面

loop://以便进入下一关
	if (lv == 1) {//难度1
		puts_name();//输入玩家的名字
	}
	else if (lv == 2) {//进入难度2




		tem_score = fopen("tem_score.txt", "r");
		fscanf(tem_score, "%d", &score);//将临时分数文件中的分数取出粘贴
		fclose(tem_score);
		tem_score = NULL;

		lv1();
	}
	else if (lv == 3) {//进入难度3
		tem_score = fopen("tem_score.txt", "r");
		fscanf(tem_score, "%d", &score);//将临时分数文件中的分数取出粘贴
		fclose(tem_score);
		tem_score = NULL;

		lv2();
	}
	int qw = 0;
	while (name[qw] != '\0') {
		bgd[18][qw + 20] = name[qw];//加载玩家姓名
		qw++;
	}
	PlaySound("01.wav", NULL,SND_ASYNC | SND_NODEFAULT | SND_LOOP);//循环播放背景音乐
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
	num = 20;//num代表得分，也即吃到的食物个数
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
		loop_num++;
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
		loop_num++;
		if (num == 20)
			break;
		Sleep(dif);
	}
	if (num < 20) {//意外死亡
		PlaySound("02.wav", NULL, SND_SYNC | SND_NODEFAULT);//结束音	
		while (name[qw] != '\0') {
			bgd[18][qw + 20] =' ';//删掉地图上的玩家姓名
			qw++;
		}
		while (_getch() != 'p') {}
		free_chain();
		clear();
		if (x == 2) {
			x = 1;
			wel[10][7] = ' ';
			wel[10][8] = ' ';
			wel[7][7] = '=';
			wel[7][8] = '>';
		}
		c = 0, i = 0, dif = 300,lv=1;
		choise();
		goto loop;
	}
	else {//达成任务通关
		PlaySound("03.wav", NULL,  SND_SYNC | SND_NODEFAULT);//通关音
		resornot();
		score = score + (int)1000000 / (loop_num*(i_poison + 1)*(i_bomb + 1));//公式，计算分数

		tem_score = fopen("tem_score.txt", "w");//存分数到临时分数文件中
		fprintf(tem_score, "%d", score);
		fclose(tem_score);
		tem_score = NULL;

		i_poison = 0;//重新定位
		i_bomb = 0;
		loop_num = 0;
		if (lv == 4) {//如果完成的是最终难度

			file_score = fopen("file_score.txt", "a");
			fprintf(file_score, " %d", score);//存入数字到最终分数文件中（已用空格隔开）
			fclose(file_score);
			file_score = NULL;

			score = 0;//分数清零
			lv3();
			while (name[qw] != '\0') {
				bgd[18][qw + 20] = ' ';//删掉地图上的玩家姓名
				qw++;
			}
			list = fopen("list.txt", "a");//现在将玩家姓名存入排行榜list中
			fputs(name, list);
			putc('\0', list);
			fclose(list);
			list = NULL;
			while (_getch() != 'p') {}
			free_chain();
			clear();
			if (x == 2) {
				x = 1;
				wel[10][7] = ' ';
				wel[10][8] = ' ';
				wel[7][7] = '=';
				wel[7][8] = '>';
			}
			 c = 0, i = 0, dif = 300, lv = 1;
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
		case 'p':while (1) {//暂停的实现
			if ((q = _getch()) == 'p')      //再次按下p启动   
				break;
			else if (q == 27) {//按下esc退出，并储存关卡信息
				level = fopen("mod.txt", "w");
				temporary = fopen("temporary.txt", "w");
				putc(lv, level);//储存关卡等级
				fputs(name, temporary);//储存名字
				fclose(level);
				fclose(temporary);
				level = NULL;
				temporary = NULL;
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
		dif = dif - 8;
		food();//刷新食物
		bomb_and_poison();
	}
	else if (bgd[b][a] == 'V') {//吃到毒草，减链表长度，增速；
		i_poison++;
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
		i_bomb++;
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

char more[20][36] = {//第四个选项画面（更多信息）
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

char thelist[20][36] = {//第三个选项画面（排行榜）！！！！！
	"***********************************",
	"*                                 *",
	"*            S N A K E            *",
	"*                                 *",
	"***********************************",
	"*Name                Score        *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
	"*                                 *",
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
	char co;//co表示敲击的值，x表示目前在哪一个选项
	co = _getch();//得到输入值
	if (co== 'p'&&x == 1) {//（开始新游戏）
		PlaySound("04.wav", NULL,SND_SYNC | SND_NODEFAULT);//音效（下同）
	}
	else if (co== 'p'&&x == 2) {//（继续）
		PlaySound("04.wav", NULL, SND_SYNC | SND_NODEFAULT);
		level = fopen("mod.txt", "r");
		temporary = fopen("temporary.txt", "r");
		lv = getc(level);//取出关卡等级
		fgets(name, 13, temporary);//取出名字
		fclose(level);
		fclose(temporary);
		level = NULL;
		temporary = NULL;
	}
	else if (co == 'p'&&x == 3) {//（排行榜）
		PlaySound("04.wav", NULL, SND_SYNC | SND_NODEFAULT);

		//打印人名部分
		int q_y=6,q_x=1;//控制纵横，便于打印排行榜数据
		char op;
		list = fopen("list.txt", "r");
		while((op = getc(list)) != EOF) {
			if (op == '\0') {
				q_y++;
				q_x = 1;
			}
			if (q_y >= 19)
				break;
			if (op != '\0') {
				thelist[q_y][q_x] = op;
				q_x++;
			}
		}
		fclose(list);
		list = NULL;

		//打印分数部分
		q_y = 6, q_x = 20;//打印初始位置
		file_score = fopen("file_score.txt", "r");
		int qa1,qa2,qi=0;//qa存数，qi存数的位数
		while (!feof(file_score)) {
			fscanf(file_score, "%d", &qa1);
			qa2 = qa1;
			for (qi = 0; qa1 / 10 != 0;qi++) {
				qa1 /= 10;
			}
			q_x += qi;
			while (qi >= 0) {
				thelist[q_y][q_x] = (char)(qa2 % 10 + 48);
				qa2 = qa2 / 10;
				qi--;
				q_x--;
			}
			q_y++;
			q_x = 20;
		}
		fclose(file_score);
		file_score = NULL;



		SetConsoleActiveScreenBuffer(hOutput);
		for (y = 0; y < 20; y++) {
			coord.Y = y;
			WriteConsoleOutputCharacterA(hOutput, thelist[y], 36, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);
		while(_getch()!='p'){}
		PlaySound("04.wav", NULL, SND_SYNC | SND_NODEFAULT);
		choise();
	}
	else if (co == 'p'&&x == 4) {//（更多）
		PlaySound("04.wav", NULL, SND_SYNC | SND_NODEFAULT);
		char co2;
    	SetConsoleActiveScreenBuffer(hOutput);
		for (y = 0; y < 20; y++) {
			coord.Y = y;
			WriteConsoleOutputCharacterA(hOutput, more[y], 36, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);
		while((co2=_getch())!='p'){}
		PlaySound("04.wav", NULL, SND_SYNC | SND_NODEFAULT);
		choise();
	}
	else if (co == 's') {//下移
		if (x == 3) {
			x = 4;
			wel[13][7] = ' ';
			wel[13][8] = ' ';
			wel[16][7] = '=';
			wel[16][8] = '>';
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
		else if (x == 4) {
			x = 1;
			wel[16][7] = ' ';
			wel[16][8] = ' ';
			wel[7][7] = '=';
			wel[7][8] = '>';
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
			x = 4;
			wel[7][7] = ' ';
			wel[7][8] = ' ';
			wel[16][7] = '=';
			wel[16][8] = '>';
		}
		else if (x == 2) {
			x = 1;
			wel[10][7] = ' ';
			wel[10][8] = ' ';
			wel[7][7] = '=';
			wel[7][8] = '>';
		}
		else if (x == 4) {
			x = 3;
			wel[16][7] = ' ';
			wel[16][8] = ' ';
			wel[13][7] = '=';
			wel[13][8] = '>';
		}
		choise();
	}
	else if (co == 27) {//按esc退出游戏
		exit(0);
	}
	else if (co== -32) {
		char eo;
		eo = _getch();
		if (eo== 72) {//上移
			if (x == 3) {
				x = 2;
				wel[13][7] = ' ';
				wel[13][8] = ' ';
				wel[10][7] = '=';
				wel[10][8] = '>';
			}
			else if (x == 1) {
				x = 4;
				wel[7][7] = ' ';
				wel[7][8] = ' ';
				wel[16][7] = '=';
				wel[16][8] = '>';
			}
			else if (x == 2) {
				x = 1;
				wel[10][7] = ' ';
				wel[10][8] = ' ';
				wel[7][7] = '=';
				wel[7][8] = '>';
			}
			else if (x == 4) {
				x = 3;
				wel[16][7] = ' ';
				wel[16][8] = ' ';
				wel[13][7] = '=';
				wel[13][8] = '>';
			}
		}
		else if (eo == 80) {//下移
			if (x == 3) {
				x = 4;
				wel[13][7] = ' ';
				wel[13][8] = ' ';
				wel[16][7] = '=';
				wel[16][8] = '>';
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
			else if (x == 4) {
				x = 1;
				wel[16][7] = ' ';
				wel[16][8] = ' ';
				wel[7][7] = '=';
				wel[7][8] = '>';
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
	int lt=0,lm;
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
"#     ///\ ///      #",
"#     \///////      #",
"#      \/////       #",
"#       \///        #",
"#        \/         #",
"#                  #",
"#    THANK YOU!    #",
"#                  #",
"####################"
	};
	while (name[lt] != '\0') {//打印玩家姓名
		bgd3[4][lt + 4] = name[lt];
		lt++;
	};
	for (lt = 0; lt <= 19; lt++)
		for (lm = 0; lm <= 19; lm++)
			bgd[lt][lm] = bgd3[lt][lm];
	lt = 0;
	SetConsoleActiveScreenBuffer(hOutput);
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, bgd[y], 36, coord, &bytes);
	}
	SetConsoleActiveScreenBuffer(hOutput);
	while (name[lt] != '\0') {//去除玩家姓名
		bgd3[5][lt + 1] = ' ';
		lt++;
	};
}

void clear(char m[20][36]) {
	int x, y;
	for (x = 1; x <= 18; x++)
		for (y = 1; y <= 18; y++)
			m[x][y] = ' ';
}

char bgd4[5][21] = {//输入框打印
	"###|------------|###",
	"###| YOUR NAME? |###",
	"###|            |###",
	"###|            |###",
	"###|------------|###",
};

void puts_name() {//输入名字
	SetConsoleActiveScreenBuffer(hOutput);
	int lt, lm,ln=4;
	char ch;
	for (lt = 4; lt <= 8; lt++) {
		for (lm = 0; lm <= 19; lm++)
			bgd[lt][lm] = bgd4[lt-4][lm];
	}
	for (y = 0; y < 20; y++) {
		coord.Y = y;
		WriteConsoleOutputCharacterA(hOutput, bgd[y], 36, coord, &bytes);
	}
	while ((ch = _getch()) != 13) {//将输入的字符依次显示出来
		if (ln <= 15||ch==8) {
			if (ch == 8) {//敲击backspace键
				if (ln != 4) {//未到达最左侧
					name[ln-4] = '\0';
					ln--;
					bgd[7][ln] = ' ';
					ln--;
				}
			}
			else {
				bgd[7][ln] = ch;
				name[ln - 4] = ch;//将输入的字符储存进name数组中
			}
		}

		for (y = 0; y < 20; y++) {
			coord.Y = y;
			WriteConsoleOutputCharacterA(hOutput, bgd[y], 36, coord, &bytes);
		}
		SetConsoleActiveScreenBuffer(hOutput);
		if(ln<=15)
			ln++;
	}
	clear();
}
