#include<stdio.h>
#include<stdlib.h>        
#include<unistd.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<termios.h>
#include<time.h>           
#include<limits.h>
#include<fcntl.h>
#include<signal.h>

#define up 'w'
#define down 's'
#define left 'a'                         //定义四个方向的对应字符
#define right 'd'

int f1,f2,x,y,m,z1,z2,p,q,r,t,li;       //x,y,m is tool,p,q is head,f1 f2 is food,z1,z2 is memory.
char dir;                               //决定蛇头接下来的方向
char formal_dir;                        //记忆蛇头原来的方向
int body1[324];                         //body1和body2两个数组记忆蛇身坐标
int body2[324];

int main()
{
    srand((unsigned)time(NULL));        //设置随机数种子/stdlib.h&time.h
    dir='s',li=1,                       //dir：初始方向向下；li为生命
    r=0,t=0,
    p=4,q=4;                            //p，q是蛇头位置，初始位置设为（4,4）
    void food();                        //随机生成食物的函数
    int kbhit(); 
                           //响应键盘敲击（在win中有头文件conio.h，但在Linux中没有，故此处为自行定义）
    int agetch();                       //得到键盘输入字符/linux中没有，自行定义。。。
    void direction();                   //根据输入字符改变蛇头方向

    food();
    int bgd[20][20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//地图（这样可以轻易修改，加墙啥的）


    while(1)                                    //重复循环打印
    {  
        system("clear");                        //清屏/stdlib.h

        if(bgd[p][q]==1)                        //判断是否撞到墙或咬到自己
            li=0;
        else if(bgd[p][q]==3)
            {li=0;}

        if (li==0)                              //死亡
            break;

        r=0;          
        if(p==f1&&q==f2)                        //刷新食物
            {food();}
        r=0;

        bgd[f1][f2]=4;
        bgd[p][q]=2;
        z1=p,z2=q;

        x=0,y=0;                                //打印，0：空位；1：墙；2：蛇头；3：蛇身；4：食物
        for(m=1;y<20;y++)
            {x=0;
            for(m=1;x<20;x++)
                {if(bgd[y][x]==1)
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

        usleep(300000);                          //休眠函数，sleep单位为秒，usleep为微秒/因gcc:unistd.h

        if(kbhit())                              //检测有无按键输入，并传给direction（）
            {dir=agetch();                    
            }
        direction();

        bgd[z1][z2]=3;

        //将蛇身坐标依次记录到数组，并传递变化
        if(f1==p&&f2==q)                         //咬到食物，增加元素
            {body1[t]=z1;
            body2[t]=z2;
            t=t+1;
            }
        else                                     //未咬到食物，元素位置传递
            {

                if(t==0)
                    {body1[0]=z1;
                    body2[0]=z2;
                    bgd[body1[0]][body2[0]]=0;}
                else 
                {   body1[t]=z1;
                    body2[t]=z2;               
                    r=0;
                    bgd[body1[0]][body2[0]]=0;

                    while(r<t)
                        {   
                            body1[r]=body1[r+1];
                            body2[r]=body2[r+1];
                            r=r+1;
                        }
                }        
            }
            
    }
    printf("GAME OVER\nscore:%d\n",t);
}

void food()
{
    f1=rand()%18+1;
    f2=rand()%18+1;
    if(f1==p&&f2==q)                           //食物是否和蛇头重合
        {food();}

    while(t>r)                                 //食物是否和蛇身重合
        {   
            if(f1==body1[r]&&f2==body2[r])
                {food();}
            r=r+1;
        }    
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
    {   case up:p=p-1;break;
        case down:p=p+1;break;
        case left:q=q-1;break;
        case right:q=q+1;break;
        case 'p':while(1)
                    {if ('p'==agetch())
                        break;}
        default:                               //若为其他键输入，则无效化
        {dir=formal_dir;
        direction();}
    }
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




