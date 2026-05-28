                                                #include<stdio.h>
                                        #include<stdlib.h>/*n random_foo*/
                                    #include<time.h>/*d(); screen[food_pos.y*/
                                  #ifdef _WIN32/*][food_pos.x] = FOOD;}int mai*/
                                #include<conio.h>/*n(void){ srand((unsigned)time(N*/
                              #include        <windows.h>/*ULL)); clear_screen(); */
                              int/*i*/        getch_timeout(int timeout_ms){clock_t//
                              /*nt*/          start_time=clock();while(clock()-/* ch*/
                              /* = "*/        start_time<timeout_ms){if(_kbhit())/*w*/
                              return/**/    _getch();Sleep(1);}return-1;}/*asd"[rand*/
                              #else/*() % 4]; head_pos.x = rand() % COLS; head_pos.y*/
                              #include<termios.h>/* = rand() % ROWS; body_append(hea*/
                              #include<unistd.h>/*d_pos); random_food(); int pause =*/
                                                          #include<sys/select.h>/* 0*/
                                                          int getch_timeout(int/*; f*/
              timeout_ms){struct termios old,new;fd_set read_fds;struct timeval tv;int    ch;tcgetattr(
          STDIN_FILENO,&old);new=old;new.c_lflag&=~(ICANON|ECHO);new.c_cc[VMIN]=0;new.    c_cc[VTIME]=0;/**/
        tcsetattr(STDIN_FILENO,TCSANOW,&new);FD_ZERO(&read_fds);FD_SET(STDIN_FILENO,&     read_fds);tv.tv_sec=
      timeout_ms/1000;tv.tv_usec=(timeout_ms%1000)*1000;int ret=select(STDIN_FILENO+1,    &read_fds,NULL,NULL,
    &tv);if(ret>0)ch=getchar();else ch=-1;tcsetattr(STDIN_FILENO,TCSANOW,&old);return     ch;}/*or (;;) { int */
    #endif/*c = getch_timeout(100); if (c == '\33') gameover(); if (c == 'p') pause */    /*= !pause; if (paus*/
  #define ROWS 24/*e) continue; if (c == 'w' || c == 's' || c == 'a' || c == 'd') ch*/    /* = c; screen[head_po*/
  #define COLS 32/*s.y][head_pos.x] = NONE; switch (ch) { case 'w': head_pos.y--; br*/    /*eak; case 's': head_*/
  enum PixelType{NONE,BODY,FOOD};int screen[ROWS][COLS];struct Pos{int x;int y;};       struct Pos head_pos;struct
  Pos food_pos;int score=0;int pos_equal(struct Pos pos1,struct Pos pos2){return/**/    pos1.x==pos2.x&&pos1.y==//
pos2.y;}struct BodyElement{struct Pos pos;struct BodyElement*next;};struct/*pos.*/      BodyElement*body_tail;/**/
struct BodyElement*body_head;void body_append(struct Pos pos){struct BodyElement      *new=malloc(sizeof(struct//
BodyElement));if(!new)exit(1);new->pos=pos;new->next=NULL;if(body_head){/**/          body_tail->next=new;body_tail=
new;}else body_tail=body_head=new;}                                               void body_poptail(void){if(!/*y+*/
body_head)return;struct/*+; br*/                                              BodyElement*tmp=body_head;body_head=//
body_head->next;if(!body_head)        body_tail=NULL;free(tmp);}void print_screen(void){printf("Score: %d\n",score);
putchar('+');for(int i=0;i<       COLS*2;i++)putchar('-');puts("+");for(int i=0;i<ROWS;i++){putchar('|');for(int j
=0;j<COLS;j++){switch(screen    [i][j]){case NONE:fputs("  ",stdout);break;case BODY:fputs("##",stdout);break;case
  FOOD:fputs("@@",stdout);      break;}}puts("|");}putchar('+');for(int i=0;i<COLS*2;i++)putchar('-');puts("+");}
  void clear_screen(void){    for(int i=0;i<ROWS;i++)for(int j=0;j<COLS;j++)screen[i][j]=NONE;}void gameover(void)
  {printf("\33[2J");printf    ("Game over. Your score: %d\n",score);exit(0);}void random_food(void){food_pos.x=//
  rand()%COLS;food_pos.y=     rand()%ROWS;for(struct BodyElement*part=body_head;part;part=part->next)if(/*eak;*/
    pos_equal(part->pos,      food_pos))return random_food();screen[food_pos.y][food_pos.x]=FOOD;}int main(void)
    {srand((unsigned)time(    NULL));clear_screen();int ch="wasd"[rand()%4];head_pos.x=rand()%COLS;head_pos.y=//
      rand()%ROWS;/* cas*/    body_append(head_pos);random_food();int pause=0;for(;;){int c=getch_timeout(100)
      ;if(c=='\33')/*e '*/    gameover();if(c=='p')pause=!pause;if(pause)continue;if(c=='w'||c=='s'||c=='a'
        ||c=='d')ch=c;/**/    screen[head_pos.y][head_pos.x]=NONE;switch(ch){case'w':head_pos.y--;break;//
            case's':/*a'*/    head_pos.y++;break;case'a':
                              head_pos.x--;break;case'd':
                              head_pos.x++;break;default:break;}if(head_pos.x<0||/*:*/
                              head_pos.y<0||head_pos.x>=COLS||head_pos.y>=ROWS)/* he*/
                              gameover();screen[head_pos.y][head_pos.x]=BODY;for(/*a*/
                              struct BodyElement*part=body_head;part;part=part->next){
                              struct Pos pos=part->pos;screen[pos.y][       pos.x]=//
                              BODY;if(pos_equal(pos,head_pos))/*d_*/          gameover
                              ();}body_append(head_pos);if(pos_equal          (/*pos*/
                              head_pos,food_pos)){random_food();score         ++;}else
                                {struct Pos pos=body_head->pos;if(/**/      body_head)
                                {screen[pos.y][pos.x]=NONE;body_poptail();}}/*.x--*/
                                  print_screen();printf("\33[%dA\33[%dD",ROWS+3,//
                                        COLS+2);}return 0;}/*; break; case '*/
                                              /*d': head_pos.x++; brea*/
