#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<stdbool.h>

#define X 60
#define Y 20
#define EMPTY 0
#define PLAYER 1
#define ENEMY_ALIVE 2
#define ENEMY_DEAD 3

typedef struct PLAYERDATA{
  int point;
  int level;
  int x;
  int y;
}playerdata;
typedef struct ENEMYDATA{
  int x;
  int y;
  bool isdead;
}enemydata;
typedef struct FIELDDATA{
  int state;
}fielddata;

//data データの初期化
void data_init(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]);//layer 1
void data_disp(int num,playerdata pdata,enemydata edata[40],fielddata fdata[Y][X]);

//game ゲームの管理
int  game_play(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]);//layer 1
void rmap_disp(fielddata fdata[Y][X]);//layer 2
void player_move(int ope,playerdata **pdata,fielddata fdata[Y][X]);//layer 2
void enemys_move(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]);//layer 2
int  player_alive(playerdata **pdata,fielddata fdata[Y][X]);//layer 2
int  enemys_alive(int num,enemydata edata[40],fielddata fdata[Y][X]);//layer 2
bool itsinspace(int x,int y);

//util 手続きの関数化
int  get_valid_input(int min,int max);
int  comp(int a,int b,int c);
int  arraymin(int num,double array[]);
double dist(double x_1,double y_1,double x_2,double y_2);
void write(playerdata pdata);

//レベルに応じてループをさせる
int main(){//layer 0
  srand((unsigned)time(NULL));
  playerdata pdata;
  fielddata fdata[Y][X];
  enemydata edata[40];
  int num;
  pdata.point=0;  
  for(pdata.level=1;;pdata.point+=pdata.level*10,pdata.level++){
    num=comp(pdata.level*5,40,0);
    data_init(num,&pdata,edata,fdata);
    data_disp(num,pdata,edata,fdata);
    int cont=game_play(num,&pdata,edata,fdata);
    if(cont==1){
      printf("You died at level %d.Your score is %d.",pdata.level,pdata.point);
      write(pdata);
      return 0;
    }
  }
}

//データ初期化
void data_init(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]){
  int enemyx[num],enemyy[num];
  memset(enemyx,-1,sizeof(enemyx));
  memset(enemyy,-1,sizeof(enemyy));
  pdata->x=rand()%X;
  pdata->y=rand()%Y;
  for(int i=0;i<num;i++){
    edata[i].x=rand()%X;
    edata[i].y=rand()%Y;
    edata[i].isdead=false;
  }
  for(int i=0;i<Y;i++){
    for(int j=0;j<X;j++){
      for(int k=0;k<num;k++){
        if(edata[k].x==j&&edata[k].y==i&&edata[k].isdead==false){
          fdata[i][j].state=ENEMY_ALIVE;
          break;
        }
        else if(edata[k].x==j&&edata[k].y==i&&edata[k].isdead==true){
          fdata[i][j].state=ENEMY_DEAD;
          break;
        }
        else{
          fdata[i][j].state=EMPTY;
        }
      }
      if(pdata->x==j&&pdata->y==i){
        fdata[i][j].state=PLAYER;
      }
    }
  }
}

void data_disp(int num,playerdata pdata,enemydata edata[40],fielddata fdata[Y][X]){
  printf("point:%d\nlevel:%d\n",pdata.point,pdata.level);
  printf("player:(%d %d)\n",pdata.x,pdata.y);
  for(int i=0;i<num;i++){
    printf("enemy%d:(%d %d)\n",i,edata[i].x,edata[i].y);
  }
  printf("field:\n");
  for(int i=0;i<Y;i++){
    for(int j=0;j<X;j++){
      printf("%d ",fdata[i][j].state);
    }
    printf("\n");
  }
  printf("\n");
}

//ゲームのループ
int  game_play(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]){
  int ope=-1;
  int prevenemysalive=num;
  while(1){
    rmap_disp(fdata);
    ope=get_valid_input(0,10);
    if(ope==10){
      int enemysalive;
      int playeralive;
      while(playeralive!=0&&enemysalive!=0){
        enemys_move(num,pdata,edata,fdata);
        enemysalive=enemys_alive(num,edata,fdata);
        playeralive=player_alive(&pdata,fdata);
        for(int i=0;i<num;i++){
          printf("robots %d:(%d,%d)\n",i,edata[i].x,edata[i].y);
        }
        pdata->point+=(prevenemysalive-enemysalive)*2;
        prevenemysalive=enemysalive;
        if(playeralive==0){return 1;}
        else if(enemysalive==0){return 0;}
      }
    }
    player_move(ope,&pdata,fdata);
    enemys_move(num,pdata,edata,fdata);
    int enemysalive=enemys_alive(num,edata,fdata);
    int playeralive=player_alive(&pdata,fdata);
    for(int i=0;i<num;i++){
      printf("robots %d:(%d,%d)\n",i,edata[i].x,edata[i].y);
    }
    pdata->point+=(prevenemysalive-enemysalive);
    prevenemysalive=enemysalive;
    if(playeralive==0){return 1;}
    else if(enemysalive==0){return 0;}
  }
}

//マップ表示
void rmap_disp(fielddata fdata[Y][X]){
  printf(" ");
  for(int i=0;i<X;i++){
    printf("%d",i%10);
  }
  printf("\n");
  for(int i=0;i<Y;i++){
    printf("%d",i%10);
    for(int j=0;j<X;j++){
      if(fdata[i][j].state==0){printf(" ");}
      else if(fdata[i][j].state==1){printf("@");}
      else if(fdata[i][j].state==2){printf("+");}
      else if(fdata[i][j].state==3){printf("*");}
    }
    printf("|\n");
  }
  printf(" ");
  for(int i=0;i<X;i++){
    printf("-");
  }
  printf("\n");
}

//プレイヤーの行動
void player_move(int ope,playerdata **pdata,fielddata fdata[Y][X]){
  int randx=rand()%X;
  int randy=rand()%Y;
  switch(ope){
    case 0:
      if(itsinspace((*pdata)->x-1,(*pdata)->y-1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y-1][(*pdata)->x-1].state=PLAYER;
        (*pdata)->x--;
        (*pdata)->y--;
      }
      break;
    case 1:
      if(itsinspace((*pdata)->x,(*pdata)->y-1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y-1][(*pdata)->x].state=PLAYER;
        (*pdata)->y--;
      }
      break;
    case 2:
      if(itsinspace((*pdata)->x+1,(*pdata)->y-1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y-1][(*pdata)->x+1].state=PLAYER;
        (*pdata)->x++;
        (*pdata)->y--;
      }
      break;
    case 3:
      if(itsinspace((*pdata)->x-1,(*pdata)->y)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y][(*pdata)->x-1].state=PLAYER;
        (*pdata)->x--;
      }
      break;
    case 4:
      if(itsinspace((*pdata)->x+1,(*pdata)->y)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y][(*pdata)->x+1].state=PLAYER;
        (*pdata)->x++;
      }
      break;
    case 5:
      if(itsinspace((*pdata)->x-1,(*pdata)->y+1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y+1][(*pdata)->x-1].state=PLAYER;
        (*pdata)->x--;
        (*pdata)->y++;
      }
      break;
    case 6:
      if(itsinspace((*pdata)->x,(*pdata)->y+1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y+1][(*pdata)->x].state=PLAYER;
        (*pdata)->y++;
      }
      break;
    case 7:
      if(itsinspace((*pdata)->x+1,(*pdata)->y+1)==true){
        fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
        fdata[(*pdata)->y+1][(*pdata)->x+1].state=PLAYER;
        (*pdata)->x++;
        (*pdata)->y++;
      }
      break;
    case 8:
      while(itsinspace(randx,randy)==false){
        randx=rand()%X;
        randy=rand()%Y;
      }
      fdata[(*pdata)->y][(*pdata)->x].state=EMPTY;
      fdata[randy][randx].state=PLAYER;
      (*pdata)->x=randx;
      (*pdata)->y=randy;
      break;
    case 9:
      break;
    default:
      printf("$Input error. Try again.\n");
      break;
  }
}

//敵の行動
void enemys_move(int num,playerdata *pdata,enemydata edata[40],fielddata fdata[Y][X]){
  for(int i=0;i<num;i++){
    if(edata[i].isdead==false){
      double distance[8];
      distance[0]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x-1),(double)(edata[i].y-1));
      distance[1]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x),(double)(edata[i].y-1));
      distance[2]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x+1),(double)(edata[i].y-1));
      distance[3]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x-1),(double)(edata[i].y));
      distance[4]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x+1),(double)(edata[i].y));
      distance[5]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x-1),(double)(edata[i].y+1));
      distance[6]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x),(double)(edata[i].y+1));
      distance[7]=dist((double)pdata->x,(double)pdata->y,(double)(edata[i].x+1),(double)(edata[i].y+1));
      int distmin=arraymin(8,distance);
      switch(distmin){
        case 0:
          if(itsinspace(edata[i].x-1,edata[i].y-1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y-1][edata[i].x-1].state=ENEMY_ALIVE;
            edata[i].x--;
            edata[i].y--;
          }
          break;
        case 1:
          if(itsinspace(edata[i].x,edata[i].y-1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y-1][edata[i].x].state=ENEMY_ALIVE;
            edata[i].y--;
          }
          break;
        case 2:
          if(itsinspace(edata[i].x+1,edata[i].y-1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y-1][edata[i].x+1].state=ENEMY_ALIVE;
            edata[i].x++;
            edata[i].y--;
          }
          break;
        case 3:
          if(itsinspace(edata[i].x-1,edata[i].y)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y][edata[i].x-1].state=ENEMY_ALIVE;
            edata[i].x--;
          }
          break;
        case 4:
          if(itsinspace(edata[i].x+1,edata[i].y)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y][edata[i].x+1].state=ENEMY_ALIVE;
            edata[i].x++;
          }
          break;
        case 5:
          if(itsinspace(edata[i].x-1,edata[i].y+1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y+1][edata[i].x-1].state=ENEMY_ALIVE;
            edata[i].x--;
            edata[i].y++;
          }
          break;
        case 6:
          if(itsinspace(edata[i].x,edata[i].y+1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y+1][edata[i].x].state=ENEMY_ALIVE;
            edata[i].y++;
          }
          break;
        case 7:
          if(itsinspace(edata[i].x+1,edata[i].y+1)==true){
            fdata[edata[i].y][edata[i].x].state=EMPTY;
            fdata[edata[i].y+1][edata[i].x+1].state=ENEMY_ALIVE;
            edata[i].x++;
            edata[i].y++;
          }
          break;
      }
    }
  }
}

//プレイヤー生存確認
int player_alive(playerdata **pdata,fielddata fdata[Y][X]){
  if(fdata[(*pdata)->y][(*pdata)->x].state==EMPTY||fdata[(*pdata)->y][(*pdata)->x].state==PLAYER){
    return 1;
  }
  return 0;
}

//敵生存確認
int enemys_alive(int num,enemydata edata[40],fielddata fdata[Y][X]){
  int alive=num;
  for (int i=0;i<num;i++){
    for(int j=i+1;j<num;j++){
      if(edata[j].x==edata[i].x&&edata[j].y==edata[i].y){
        edata[i].isdead=true;
        edata[j].isdead=true;
        fdata[edata[i].y][edata[i].x].state=3;
        fdata[edata[j].y][edata[j].x].state=3;
      }
    }
    if(edata[i].isdead==true){
      alive--;
    }
  }
  return alive;
}

//要素のはみ出し監視
bool itsinspace(int x,int y){
  if(x<X&&x>=0&&y<Y&&y>=0){
    return true;
  }
  return false;
}

//入力の受付
int get_valid_input(int min,int max){
  int input;
  printf("0 1 2\n3 @ 4\n5 6 7\n");
  printf("$Enter a number between %d and %d:>", min, max);
  while(1){
    if(scanf("%d",&input)==1&&input>=min&&input<=max) {
      break;
    }
    else{
      printf("$Invalid input. Please enter a number between %d and %d:>", min, max);
      while (getchar() != '\n'); // Clear input buffer
    }
  }
  return input;
}

//ユークリッド距離計算
double dist(double x_1,double y_1,double x_2,double y_2){
  double dist=sqrt(pow((x_1-x_2),2)+pow((y_1-y_2),2));
  return dist;
}

//大小比較
int comp(int a,int b,int c){
  if(c==0){//c==0 小さいほうを返す
    if(a<b){return a;}
    else{return b;}
  }
  if(c==1){//c==1 大きい方を返す
    if(a>b){return a;}
    else{return b;}
  }
}

//最小の配列要素を返す
int arraymin(int num,double array[]){
  double min=array[0];
  int min_arg=0;
  for(int i=1;i<num;i++){
    if(min>array[i]){
      min=array[i];
      min_arg=i;
    }
  }
  return min_arg;
}

//ファイル書き込み
void write(playerdata pdata){
  FILE *fp;
  fp=fopen("score.txt","a"); 
  fprintf(fp,"%d\n",pdata.point);
}