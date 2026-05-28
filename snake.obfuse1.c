                                                                            #include                          /*]*/\
                                                                <stdio.h>/*[j]) { */                          /*ca*/
                                #include<stdlib.h>/*se NONE: fputs(" ", stdout);*/                          /* break*/
                                #include<time.h>/*; case BODY: fputs("##", stdou*/                          /*t); br*/
                        #ifdef _WIN32/*eak; case FOOD: fputs("@@", stdout); brea*/                          /*k; } } puts(*/                                  /**/
                    #include<conio.h>/*"|"); } putchar('+'); for (int i = 0; i <*/                          /* COLS * 2; i++*/                              /*) */
                #include<windows.h>/*putchar('-'); puts("+");}void clear_screen(*/                          /*void){ for (int i */                /*= 0; i < ROW*/
                int getch_timeout(int timeout_ms){clock_t start_time=clock();while                          (clock()-start_time<                  timeout_ms){if(
              _kbhit())return _getch();Sleep(1);}return-1;}/*S; i++) for (int j = 0;*/                        /* j < COLS; j++) sc*/      /*reen[i][j] = NONE;}v*/
            #else/*oid gameover(void){ printf("\33[2J"); printf("Game over. Your score: */                    /*%d\n", score); exit(0);}void random_food(void){ */
        #include<termios.h>/*food_pos.x = rand() % COLS; food_pos.y = rand() % ROWS; for (*/                  /*struct BodyElement *part = body_head; part; */
        #include<unistd.h>/*part = part->next) if (pos_equal(part->pos, food_pos)) return */                  /*random_food(); screen[food_pos.y][food_pos.x*/
        #include<sys/select.h>/*] = FOOD;}int main(void){ srand((unsigned)time(NULL)); clear*/                    /*_screen(); int ch = "wasd"[rand() % 4]; */
      int getch_timeout(int timeout_ms){struct termios old,new;fd_set read_fds;struct timeval tv;                 int ch;tcgetattr(STDIN_FILENO,&old);new=//
    old;new.c_lflag&=~(ICANON|ECHO);new.c_cc[VMIN]=0;new.c_cc[VTIME]=0;tcsetattr(STDIN_FILENO,/*he*/                TCSANOW,&new);FD_ZERO(&read_fds);/*a*/
    FD_SET(STDIN_FILENO,&read_fds);tv.tv_sec=timeout_ms/1000;tv.tv_usec=(timeout_ms%1000)*1000;int                  ret=select(STDIN_FILENO+1,&read_fds,//
    NULL,NULL,&tv);if(ret>0)ch=getchar();else ch=-1;tcsetattr(STDIN_FILENO,TCSANOW,&old);return ch;}                  /*d_pos.x = rand() % COLS; hea*/
    #endif/*d_pos.y = rand() % ROWS; body_append(head_pos); random_food(); int pause = 0; for (;;) { int*/                /* c = getch_timeout(1*/
#define ROWS 24/*00); if (c == '\33') gameover(); if (c == 'p') pause = !pause; if (pause) continue; if (c*/              /* == 'w' || c*/
#define COLS 32/* == 's' || c == 'a' || c == 'd') ch = c; screen[head_pos.y][head_pos.x] = NONE; switch (c*/              /*h) { case 'w*/
enum/*': h*/                PixelType{NONE,BODY,FOOD};int screen[ROWS][COLS];struct Pos{int x;int y;};struct Pos          head_pos;//
struct                              Pos food_pos;int score=0;int pos_equal(struct Pos pos1,struct Pos pos2){return pos1.x==pos2.x&&//
pos1.y==                                    pos2.y;}struct BodyElement{struct Pos             pos;struct BodyElement*next;};struct/**/
/*ead_*/                                    BodyElement*body_tail;struct BodyElement          *body_head;void body_append(struct Pos//
pos){                                           struct BodyElement*new=malloc(sizeof                (struct BodyElement));if(!new)exit
(1);new                                             ->pos=pos;new->next=NULL;if(body_head){           body_tail->next=new;body_tail=//
new;}else                                               body_tail=body_head=new;}                     void body_poptail(void){if(!/**/
body_head)                                              return;struct BodyElement*                    tmp=body_head;body_head=/*pos.*/
body_head->                                                 next;if(!body_head)/*y--*/                    body_tail=NULL;free(tmp);}
void/*; br*/                                                  print_screen(void){printf("Sc"                "ore: %d\n",score);/*e*/
putchar('+')                                                    ;for(int i=0;i<COLS*2;i++)                  putchar('-');puts("+");
for(int i=0;                                                    i<ROWS;i++){putchar('|');for                (int j=0;j<COLS;j++){//
switch(screen[                                                      i][j]){case NONE:fputs(" "              " ",stdout);break;case
BODY:fputs("#"                                                      "#",stdout);break;case FOOD:fputs("@@",stdout);break;}}puts(""
    "|");}/*ak*/                                                      putchar('+');for(int i=0;i<COLS*2;i++)putchar('-');puts(
    "+");}void                                                        clear_screen(void){for(int i=0;i<ROWS;i++)for(int j=0;j<
    COLS;j++)                                                             screen[i][j]=NONE;}void gameover(void){printf("\33["
    "2J");printf("G"                                                        "ame over. Your score: %d\n",score);exit(0);}//
    void random_food                                                        (void){food_pos.x=rand()%COLS;food_pos.y=rand()%
      ROWS;for(/*;*/                                                        struct BodyElement*part=body_head;part;part=part
      ->next)if(/* c*/                                                        pos_equal(part->pos,food_pos))return/*ase */
        random_food();                                                            screen[food_pos.y][food_pos.x]=FOOD;
        }int main(void){                                                          srand((unsigned)time(NULL));/*'s':*/
        clear_screen();int                          ch="was"                      "d"[rand()%4];head_pos.x=rand()%COLS
            ;head_pos.y=rand()                        %ROWS;                        body_append(head_pos);/* head_*/
              random_food();int                       pause=0;for(;;                  ){int c=getch_timeout(100);
              if(c=='\33')/*po*/                      gameover();if(                  c=='p')pause=!pause;if(pause
              )continue;if(c=='w'||c                  =='s'||c=='a'||c                    =='d')ch=c;screen[head_pos
                .y][head_pos.x]=NONE;                   switch(ch){case'w':                 head_pos.y--;break;case's'://
                    head_pos.y++;break;case'a'      :head_pos.x--;break;case                  'd':head_pos.x++;break;default:
                    break;}if(head_pos.x<0||        head_pos.y<0||head_pos.x>=                COLS||head_pos.y>=ROWS)gameover(
                        );screen[head_pos.y][head_pos.x]=BODY;for(struct BodyElement              *part=body_head;part;part=part->
                            next){struct Pos pos=part->pos;screen[pos.y][pos.x]=BODY;if(pos_equal(    pos,head_pos))gameover()
                              ;}body_append(head_pos);if(pos_equal(head_pos,food_pos)){/*s.y*/
                              random_food();score++;}else{struct Pos pos=body_head->pos;if(//
                                    body_head){screen[pos.y][pos.x]=NONE;body_poptail();}}//
                                            print_screen();printf("\33[%dA\33[%dD",//
                                                    ROWS+3,COLS+2);}return 0
                                                    ;}/*++; break; case 'a*/
