                                                        #include<stdio.h>
                                                    #include<stdlib.h>/*; he*/
                                            #include<time.h>/*ad_pos.x = rand() % CO*/
                                          #ifdef _WIN32/*LS; head_pos.y = rand() % R*/
                                        #include<conio.h>/*OWS; body_append(head_pos);*/
                                      #include<windows.h>             /* random_food(); */
                                  int getch_timeout(                          int timeout_ms){clock_t start_time=
                                  clock();while(                              clock()-start_time<timeout_ms){if(_kbhit
                                  ())return/*i*/                              _getch();Sleep(1);}return-1;}/*nt pause */
                                #else/*= 0; */                              /*for (;;) { int c = getch_timeout(100); if */
                                #include/**/                            <termios.h>/*(c == '\33') gameover(); if (c == 'p'*/
                              #include/**/                          <unistd.h>/*) pause */                    /*= !pause; if (*/
                            #include/*pa*/                      <sys/select.h>/*use)*/                          /* continue; if */
                        int/*(c == 'w'*/                    getch_timeout(int/* */                                timeout_ms){/**/
                    struct termios old,                   new;fd_set read_fds;                                      struct timeval
                tv;int ch;tcgetattr(                STDIN_FILENO,&old);new=                                             old;new./**/
            c_lflag&=~(ICANON|ECHO);                new.c_cc[VMIN]=0;new                                                  .c_cc[/**/
            VTIME]=0;tcsetattr(/*|| */            STDIN_FILENO,TCSANOW                      ,&                            new);FD_ZERO
          (&read_fds);FD_SET(/*c == */            STDIN_FILENO,&                        read_fds);tv                      .tv_sec=/**/
        timeout_ms/       1000;tv./**/            tv_usec=(                       timeout_ms%1000)*1000;                    int ret=/**/
      select(/*'s'*/      STDIN_FILENO            +1,&/**/                      read_fds,NULL,NULL,&tv);if(                 ret>0)ch=//
      getchar();          else ch=-1;             /* || */                    tcsetattr(STDIN_FILENO,TCSANOW,&              old);return
    ch;}/*c == '*/        /*a' || c */            /*== '*/                /*d') ch = c; */  /*screen[head_pos.*/              /*y][hea*/
    #endif/*d_*/            /*pos.x]*/            /* = N*/            /*ONE; switch */          /*(ch) { case 'w':*/          /* head_*/
  #define ROWS              24/*pos.*/            /*y--;*/        /* break; ca*/                    /*se 's': head_pos.y*/  /*++; brea*/
#define COLS                32/*k; c*/            /*ase */    /*'a': head_pos.x-*/                      /*-; break; case 'd': head_pos*/
enum/*.x++*/                PixelType{            NONE,     BODY,FOOD};int screen[ROWS                    ][COLS];struct Pos{int x;int y
;};struct                   Pos/*; b*/            head_pos;struct Pos food_pos;int score                      =0;int pos_equal(struct//
Pos pos1,                   struct Pos            pos2){return pos1.          x==pos2.x&&pos1.                    y==pos2.y;}struct/*r*/
/*eak; d*/                  /*efault*/            BodyElement{                  struct Pos pos;                       struct BodyElement
*next;};                    struct/**/            /*: brea*/                        BodyElement*                          body_tail;/**/
struct/**/                  /*k; } i*/            /*f (hea*/                          BodyElement*/**/                    body_head;void//
body_append(                struct Pos            pos){                               struct BodyElement*                     new=malloc(
sizeof(/*d*/                struct/**/            /*_pos*/                            BodyElement));if(!new)                    exit(1);new
->pos=pos;                  new->next=            NULL;if(                            body_head){body_tail->next                  =new;/*.*/
  body_tail=                new;}else             /*x < */                            body_tail=    body_head=new;                }void/*0 |*/
  body_poptail              (void){if(!/*|*/      /* hea*/                            body_head)        return;/*d*/              struct/*_p*/
    BodyElement*                  tmp=body_head;body_head=                            body_head           ->next;if(                !body_head)
    body_tail=NULL                ;free(tmp);}void/*os.y*/                            /* < 0 |*/          /*| head*/                print_screen
      (void){/*_*/                    printf("Score: %d\n"                            ,score);            putchar(                  '+');for(int
      i=0;i<COLS*2;i                      ++)putchar('-');                            puts("+");          for(int i=                0;i<ROWS;i++
        ){putchar('|');for                    (int j=0;j<COLS;                    j++){switch(            screen[i][                j]){case/**/
        NONE:fputs("  ",/**/                      stdout);break;              case BODY:fputs(""          "##",/*p*/                stdout);/**/
          break;case FOOD:fputs(                    "@@",stdout);             break;}}puts("|");          }putchar(                 '+');for(int
        i=0;i<COLS*2;i++)putchar(                     '-');puts("+");}    void clear_screen(void          ){for(int                 i=0;i<ROWS;i
        ++)for(int j=0;j<COLS;j++)/**/                      screen[i][j]=NONE;}void   gameover(           void){/**/              printf("\33"
        "[2J");printf("Game over. Your scor"                  "e: %d\n",score);       exit(0);}           void/*os*/              random_food(
        void){/**/      food_pos.x=rand()%                    COLS;food_pos.y=        rand()%             ROWS;for(               struct/*.x*/
        /* >= CO*/        BodyElement*part=/*L*/            body_head;part            ;part=part          ->next)if(              pos_equal(
        part->pos,          food_pos))return/*S */      random_food();                screen[             food_pos.y            ][food_pos.x
        ]=FOOD;}                  int main(void){srand((unsigned)                     time(NULL)          );/*|| h*/          clear_screen
        ();int ch=""                "wasd"[rand()%4];head_pos.x=                      rand()%             COLS;/*e*/        head_pos.y=//
        rand()%ROWS;                    body_append(head_pos                        );/*ad_pos*/          /*.y >= */      random_food();//
        int pause=0;                        for(;;){int c=                      getch_timeout(            100);if(c     =='\33')gameover
          ();if(c=='p'                                                    )pause=!pause;if(               pause)continue;if(c=='w'||
          c=='s'||c=='a'                                                ||c=='d')ch=c;screen[           head_pos.y][head_pos.x]=//
            NONE;switch(ch                                            ){case'w':head_pos.y              --;break;case's':head_pos.
            y++;break;case                                        'a':head_pos.x--;                     break;case'd':head_pos
                .x++;break;                                 default:break;}if(/**/                      head_pos.x<0||//
                head_pos.y<0||/**/                        head_pos.x>=COLS||                            head_pos.y
                  >=ROWS)gameover();                  screen[head_pos.y][/*R*/                        head_pos.x]=
                  BODY;for(struct BodyElement*part=body_head;part;part=                               part->next
                    ){struct Pos pos=part->pos;screen[pos.y][pos.x]=                                BODY;if(/**/
                          pos_equal(pos,head_pos))gameover();}/**/                              body_append(/**/
                            head_pos);if(pos_equal(head_pos,/*OW*/                              food_pos)){//
                                random_food();score++;}else{struct                          Pos pos=/*S) g*/
                                      body_head->pos;if(body_head){/*a*/              screen[pos.y][pos.x]
                                                        =NONE;body_poptail();}}print_screen();printf(""
                                                          "\33[%dA\33[%dD",ROWS+3,COLS+2);}return 0;}
                                                            /*meover(); screen[head_pos.y][head_po*/
                                                              /*s.x] = BODY; for (struct BodyE*/
                                                                        /*lement *pa*/
