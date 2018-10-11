#include<stdio.h>
#include<stdlib.h>        
#include<unistd.h>
#include<termios.h>
#include<time.h>           
#include<fcntl.h>
#include <GL/glut.h>      

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
};
struct snake *head,*p1,*p2; 


 char dir;                              
    char formal_dir;                         
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
                    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 

//////////////////////////////////////////////////////////
int agetch()
{
     struct termios tm, tm_old;
     int fd = 0, ch;

     if (tcgetattr(fd, &tm) < 0) {
          return -1;
     }

     tm_old = tm;
     cfmakeraw(&tm);
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {
          return -1;
     }

     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
          return -1;
     }
     return ch;
}

void direction()
{   if(dir==up&&formal_dir==down)              
        dir=formal_dir;
    else if(dir==down&&formal_dir==up)
        dir=formal_dir;
    else if(dir==left&&formal_dir==right)
        dir=formal_dir;
    else if(dir==right&&formal_dir==left)
        {dir=formal_dir;}
    switch(dir)                               
    {   case up:b=b-1;break;
        case down:b=b+1;break;
        case left:a=a-1;break;
        case right:a=a+1;break;
        case 'p':while(1)
                    {if ('p'==agetch())
                        break;}
        default:                              
        {dir=formal_dir;
        direction();}
    }
}

void food()
{
    f1=rand()%18+1;
    f2=rand()%18+1;
    if(bgd[f1][f2]==2||bgd[f1][f2]==3||bgd[f1][f2]==1)                         
        food(); 
    bgd[f1][f2]=4;
} 



int kbhit(void)                                
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
//////////////////////////////////////////////////////////////////
   
void fprint()
{

    glPointSize(20.0f);
    glClearColor(0.5f,0.5f,0.5f,0.0f);

    void food();
    int kbhit(); 
    int agetch();
    int usleep();
    void direction();
    head=p1=p2=(struct snake*)malloc(SN);           
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

        glClear(GL_COLOR_BUFFER_BIT);
        system("clear");
        glBegin(GL_POINTS);

                          
        for(y=0;y<20;y++)
            {                
                for(x=0;x<20;x++)
                    {
                        if(bgd[y][x]==1)
                            {glBegin(GL_POINTS);
                            glColor3f(0.0f,1.0f,1.0f);
                            glVertex2f(x*20,(20-y)*20);
                            glEnd();}
                        else if(bgd[y][x]==2)
                            {glBegin(GL_POINTS);
                            glColor3f(0.0f,1.0f,1.0f);
                            glVertex2f(x*20,(20-y)*20);
                            glEnd();}
                        else if(bgd[y][x]==3)
                            {glBegin(GL_POINTS);
                            glColor3f(1.0f,1.0f,1.0f);
                            glVertex2f(x*20,(20-y)*20);
                            glEnd();}
                        else if(bgd[y][x]==4)
                            {glBegin(GL_POINTS);
                            glColor3f(1.0f,0.0f,0.0f);
                            glVertex2f(x*20,(20-y)*20);
                            glEnd();}
                    }                
                
            }


            formal_dir=dir;
            usleep(300000);

            glFlush();
    }
    printf("score=%d",num);
}








    int main(int argc, char *argv[])
    {   
     glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(30*30,30*30);
    glutCreateWindow("SNAKE3.0");
    glutDisplayFunc(fprint);
    glutMainLoop();
    return 0;

    }