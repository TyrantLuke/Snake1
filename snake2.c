#include<stdio.h>
#include<stdlib.h>        
#include<unistd.h>
#include<termios.h>
#include<time.h>           
#include<fcntl.h>
#define up 'w'
#define down 's'
#define left 'a'                         //定义四个方向的对应字符
#define right 'd'

#define SN sizeof(struct snake)

struct snake                              //@@@@@@@@@@@@@@@@@@@@@@@@@@@@2
{
    int s_y;
    int s_x;
    struct snake * next;
};
struct snake *head,*p1,*p2; 

char dir;                               //决定蛇头接下来的方向
char formal_dir;                         //记忆蛇头原来的方向
int li,num,f1,f2,b,a,y,x;                     

int bgd[20][20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,                    
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//地图（这样可以轻易修改，加墙啥的） 


int main()
{
    void food();
    int kbhit(); //响应键盘敲击（在win中有头文件conio.h，但在Linux中没有，故此处为自行定义）
    int agetch();
    int usleep();
    void direction();
    head=p1=p2=(struct snake*)malloc(SN);            //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2
    head->s_y=4;
    head->s_x=4;
    head->next=NULL;

    formal_dir=dir='s',li=1;

    b=4,a=4;
    num=0;
    food();
    while(1)
    {
         
        if(kbhit())
            dir=agetch();
        direction();
        
        if(bgd[b][a]==1)
            break;
        else if(bgd[b][a]==3)
            break;
        else if(bgd[b][a]==0)
            {
                bgd[head->s_y][head->s_x]=0;
                if(num==0)
                    {                           
                        head->s_y=b;
                        head->s_x=a;
                    }
               else
                    {   
                        p1=head;
                        p1->s_y=b;
                        p1->s_x=a;
                        head=head->next;
                        p1->next=NULL;
                        bgd[p2->s_y][p2->s_x]=3;
                        p2->next=p1;                       
                        p2=p1;                                  
                    }
                bgd[b][a]=2;
            }
        else if(bgd[b][a]==4)
            {
                p1=(struct snake*)malloc(SN);
                p1->s_y=b;
                p1->s_x=a;
                p2->next=p1;
                bgd[p2->s_y][p2->s_x]=3;
                p2=p1;
                bgd[b][a]=2;
                food();
                num++; 
            }

        system("clear");
                                //打印，0：空位；1：墙；2：蛇头；3：蛇身；4：食物
        for(y=0;y<20;y++)
            {                
                for(x=0;x<20;x++)
                    {
                        if(bgd[y][x]==1)
                            printf("#");
                        else if(bgd[y][x]==0)
                            printf(" ");
                        else if(bgd[y][x]==2)
                            printf("@");
                        else if(bgd[y][x]==3)
                            printf("0");
                        else if(bgd[y][x]==4)
                            printf("$");
                    }                
                printf("\n");
            }

            formal_dir=dir;
            usleep(300000);
    }
    printf("GAME OVER\nscore:%d\n",num);
}

void direction()
{   if(dir==up&&formal_dir==down)              //防止控制蛇往回走
        dir=formal_dir;
    else if(dir==down&&formal_dir==up)
        dir=formal_dir;
    else if(dir==left&&formal_dir==right)
        dir=formal_dir;
    else if(dir==right&&formal_dir==left)
        {dir=formal_dir;}
    switch(dir)                                //蛇四面运动的实现
    {   case up:b=b-1;break;
        case down:b=b+1;break;
        case left:a=a-1;break;
        case right:a=a+1;break;
        case 'p':while(1)
                    {if ('p'==agetch())
                        break;}
        default:                               //若为其他键输入，则无效化
        {dir=formal_dir;
        direction();}
    }
}

void food()
{
    f1=rand()%18+1;
    f2=rand()%18+1;
    if(bgd[f1][f2]==2||bgd[f1][f2]==3||bgd[f1][f2]==1)                           //食物是否和蛇头重合
        food(); 
    bgd[f1][f2]=4;
} 

int agetch()
{
     struct termios tm, tm_old;
     int fd = 0, ch;

     if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
          return -1;
     }

     tm_old = tm;
     cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
          return -1;
     }

     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
          return -1;
     }
     return ch;
}

int kbhit(void)                                  //抄来的。。。
{  
      struct termios oldt, newt;  
      int ch;  
      int oldf;  
      tcgetattr(STDIN_FILENO, &oldt);  
      newt = oldt;  
      newt.c_lflag &= ~(ICANON | ECHO);  
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
      oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
      fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);  
      ch = getchar();  
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
      fcntl(STDIN_FILENO, F_SETFL, oldf);  
      if(ch != EOF)  
      {  
        ungetc(ch, stdin);  
        return 1;  
      }  
      return 0; 
} 