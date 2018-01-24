/**
    Runs Rogue Junior, a short roguelike adventure game.

    @version 1.0 14 Oct 2015
    @author Andrea Dewhirst
*/
#include<iostream>
#include<conio.h>
#include<cstdlib>
#include<windows.h>
#include<string>
#include<ctime>
#include<cmath>
#include "Player.h"
#include "Monster.h"
#define SCREEN_WIDTH  57
#define SCREEN_HEIGHT 20

using namespace std ;



/**
    These variable help avoid redrawing the entire map every time the player moves.
*/
COORD level_title = {0, 0} ;
COORD HP_count = {1 , 20};
COORD action_text = {1, 21};
CHAR_INFO screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
CHAR_INFO screenBuffer2[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};

/**
    These variables store information about how the game should look.
    Roguelike games originally used ASCII characters instead of illustrated tiles.
*/
const CHAR* me = "@" ;
DWORD  dwResult=0;
int Player_X_Location;
int Player_Y_Location;
const WORD colorBuf[1] = {FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};
const CHAR* dungeonfloor = ".";
const WORD monster_color[1] = {FOREGROUND_RED} ;
const WORD player_color[1] = {FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE} ;

/**
    These functions redraw the game screen.
*/
void drawMap() ;
void fillBuffer() ;
void fillBuffer2();
void drawMap2();
void fillStartBuffer() ;
void drawStartScreen() ;
int blocked_by_monster(int, int);

/**
    Adjusts player location based on arrow key input.
    @FIXME when multiple arrow keys are held at once, a false copy of player char appears.
*/
void Player::movePlayer(int xPosition, int yPosition, int oldX, int oldY) {
     COORD oldPosition = {oldX, oldY} ;
     WriteConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorBuf , 1, oldPosition,&dwResult );
     WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), dungeonfloor, 1, oldPosition, &dwResult);
     COORD playerPosition = {xPosition, yPosition} ;
     WriteConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), player_color , 1, playerPosition,&dwResult );
     WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), me, 1, playerPosition, &dwResult);
}

/**
    If the player is within 1 square of the monster, hit the player
    and return the amount of damage done. Else do no damage to player.

    @return amount of damage done to player
*/
int monster::hitdie() {
   srand(time(0));
   if ((xPosition - 1 == Player_X_Location && yPosition == Player_Y_Location) ||
      (xPosition + 1 == Player_X_Location && yPosition == Player_Y_Location) ||
      (yPosition - 1 == Player_Y_Location && xPosition == Player_X_Location) ||
      (yPosition + 1 == Player_Y_Location && xPosition == Player_X_Location)
   ) {
      return dierolls * (rand() % numSides + 1);
   } else return 0;
}

/**
    Monsters don't move until the player wakes them up by coming too close.
    Then they begin to follow the player and hit them if they are within range.
    @TODO: better pathfinding for the monsters. They don't track the player very well.
*/
void monster::monster_movement() {

   if ((Player_X_Location < xPosition) && screenBuffer[(xPosition - 1) + yPosition * SCREEN_WIDTH].Char.AsciiChar == '.') {
      oldX = xPosition;
      oldY = yPosition;
      xPosition--;
      if ((xPosition == Player_X_Location) && (yPosition == Player_Y_Location)) xPosition++;
   } else if ((Player_Y_Location < yPosition) && screenBuffer[xPosition + (yPosition - 1) * SCREEN_WIDTH].Char.AsciiChar == '.') {
      oldX = xPosition;
      oldY = yPosition;
      yPosition--;
      if ((yPosition == Player_Y_Location) && (xPosition == Player_X_Location)) yPosition++;
   } else if ((Player_X_Location > xPosition) && screenBuffer[(xPosition + 1) + yPosition * SCREEN_WIDTH].Char.AsciiChar == '.') {
      oldX = xPosition;
      oldY = yPosition;
      xPosition++;
      if ((xPosition == Player_X_Location) && (yPosition == Player_Y_Location)) xPosition--;
   } else if ((Player_Y_Location > yPosition) && (screenBuffer[xPosition + (yPosition + 1) * SCREEN_WIDTH].Char.AsciiChar == '.')) {
      oldX = xPosition;
      oldY = yPosition;
      yPosition++;
      if ((xPosition == Player_X_Location) && (yPosition == Player_Y_Location)) yPosition--;
   }

   COORD oldPosition = {
      oldX,
      oldY
   };
   WriteConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorBuf, 1, oldPosition, & dwResult);
   WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), dungeonfloor, 1, oldPosition, & dwResult);
   COORD monsterPosition = {
      xPosition,
      yPosition
   };
   WriteConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), monster_color, 1, monsterPosition, & dwResult);
   WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), avatar, 1, monsterPosition, & dwResult);
}

/**
    The monster wakes up when the player gets close.

    @return true if the monster is awake
*/
bool monster::isAwake() {
   return (((Player_X_Location <= xPosition && Player_X_Location >= xPosition - 7) ||
         (Player_X_Location >= xPosition && Player_X_Location <= xPosition + 7)) &&
      ((Player_Y_Location >= yPosition && Player_Y_Location <= yPosition + 4) ||
         (Player_Y_Location <= yPosition && Player_Y_Location >= yPosition - 4)));
}

/**
    When the monster is killed, its icon gets erased.
*/
void monster::erase() {
   COORD monsterPosition = {
      xPosition,
      yPosition
   };
   WriteConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorBuf, 1, monsterPosition, & dwResult);
   WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), dungeonfloor, 1, monsterPosition, & dwResult);
   xPosition = 0;
   yPosition = 0;
}

/**
    Instantiating the three types of monsters in this game.
*/
monster bat("bat", 7, 1, 2, "b", 0, 5, 37, 6);
monster kobold("kobold", 15, 1, 2, "k", 0, 6, 26, 15);
monster lich("Lich King", 30, 5, 5, "L", 0, 12, 7, 2) ;

int main() {
    int xPosition = 5, yPosition = 5 ;
    int oldX =5, oldY=5, level=1 ;
    int damage;
    SetConsoleTitle("Rogue Junior") ;
    bool batdeathFlag = 0, kobolddeathFlag = 0, lichdeathFlag = 0;

    drawStartScreen();
    cout<<"\t\t      Junior\n\n\n";
    Player player(20, '-') ;
    player.getname() ;
    fillBuffer() ;
    fillBuffer2() ;
    drawMap() ;
    bat.monster_movement() ;
    kobold.monster_movement();
    player.movePlayer(xPosition, yPosition, oldX,oldY) ;
    while (player.setHP(0)) {
          getch();
                  if (level == 1) {
                  if(GetAsyncKeyState(VK_DOWN)&&screenBuffer[xPosition + (yPosition+1) * SCREEN_WIDTH].Char.AsciiChar != '#') {
                    oldX=xPosition;
                    oldY=yPosition;
                    yPosition++ ;

                    if (blocked_by_monster(xPosition, yPosition)==1) {yPosition--;
                    bat.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the bat.                                                            ";}

                    if (blocked_by_monster(xPosition, yPosition)==2) {yPosition--;
                    kobold.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the kobold.                                                      ";}

                    if(screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='B' &&
                       player.getitems()!='0'){
                    yPosition--;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" There is a super pretty bear here, blocking your way.                                " ;}
                    else if (screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='B' &&
                       player.getitems()=='0'){
                          yPosition--;
                          screenBuffer[7 + 7 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
                          screenBuffer[7 + 7 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
                          screenBuffer[6 + 6 * SCREEN_WIDTH].Char.AsciiChar = 'B' ;
                          screenBuffer[6 + 6 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_BLUE ;
                          screenBuffer[53 + 14 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
                          screenBuffer[53 + 14 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
                          drawMap() ;
                          SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                          cout<<" You give the mirror to the cute bear. She swoons to the northwest." ;
                          }

                    if(screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='0') {
                       SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                       cout<<" You find a small silver mirror.                                                         " ;
                       player.setitems('0');
                       }

                    if (screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='~') {
                       SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                       cout<<" You find the Sunlight Torc! Beams of light are shining from your eyes!      " ;
                       player.setitems('~');}
                    }

                 if(GetAsyncKeyState(VK_UP)&&screenBuffer[xPosition + (yPosition-1) * SCREEN_WIDTH].Char.AsciiChar !='#') {
                    oldX=xPosition;
                    oldY=yPosition;
                    yPosition-- ;

                    if (blocked_by_monster(xPosition, yPosition)==1) {yPosition++;
                    bat.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the bat.                                                                ";}

                    if (blocked_by_monster(xPosition, yPosition)==2) {yPosition++;
                    kobold.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the kobold.                                                                ";}

                    if(screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='0') {
                       SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                       cout<<" You find a small silver mirror.                                                  " ;
                       player.setitems('0');
                       }

                    }

                 if(GetAsyncKeyState(VK_LEFT)&&screenBuffer[(xPosition-1) + yPosition * SCREEN_WIDTH].Char.AsciiChar !='#') {
                    oldX=xPosition;
                    oldY=yPosition;
                    xPosition-- ;

                    if (blocked_by_monster(xPosition, yPosition)==1) {xPosition++;
                    bat.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the bat.                                                  ";}

                    if (blocked_by_monster(xPosition, yPosition)==2) {xPosition++;
                    kobold.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the kobold.                                              ";}

                    if(screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='0') {
                       SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                       cout<<" You find a small silver mirror.                                        " ;
                       player.setitems('0');
                       }

                    }

                 if(GetAsyncKeyState(VK_RIGHT)&&screenBuffer[(xPosition+1) + yPosition * SCREEN_WIDTH].Char.AsciiChar !='#') {
                    oldX=xPosition;
                    oldY=yPosition;
                    xPosition++ ;

                    if (blocked_by_monster(xPosition, yPosition)==1) {xPosition--;
                    bat.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the bat.                                                  ";}

                    if (blocked_by_monster(xPosition, yPosition)==2) {xPosition--;
                    kobold.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the kobold.                                              ";}

                    if(screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar =='0') {
                       SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                       cout<<" You find a small silver mirror.                                        " ;
                       player.setitems('0');
                       }

                    }

                    if(batdeathFlag == 0) {if(!bat.setHP(0)) {
                         bat.erase();
                         batdeathFlag = 1;
                         if (batdeathFlag) {
                            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                            cout<<" You slay the bat!                                               " ;
                                }}}

                    if(kobolddeathFlag == 0) {if(!kobold.setHP(0)) {
                         kobold.erase();
                         kobolddeathFlag = 1;
                         if (kobolddeathFlag) {
                            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                            cout<<" You slay the kobold!                                               " ;
                                }}}

                    player.movePlayer(xPosition, yPosition, oldX, oldY) ;
                    Player_X_Location = xPosition;
                    Player_Y_Location = yPosition;

                    if(bat.setHP(0)&&bat.isAwake()) {bat.monster_movement();
                    player.setHP(bat.hitdie()) ;}

                    if(kobold.setHP(0)&&kobold.isAwake()) {kobold.monster_movement();
                    damage = kobold.hitdie() ;
                    player.setHP(damage) ;}

                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), HP_count) ;
                    cout<<" HP: "<<player.getHP()<<" ";
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), level_title) ;
                    cout<<"Dungeon Level 1" ;

                    if (screenBuffer[xPosition + yPosition * SCREEN_WIDTH].Char.AsciiChar ==0) level = 2;

                    if (level == 2) {drawMap2();
                    xPosition = 6;
                    yPosition = 8;
                    lich.monster_movement();
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You fell down a hole!                                                             ";
                    }

                    } //end of level 1 if statement

                    if (level == 2) {

                    if (player.getitems() == '~')
                      lich.set_hitdie();

                    if(GetAsyncKeyState(VK_DOWN)&&screenBuffer2[xPosition + (yPosition+1) * SCREEN_WIDTH].Char.AsciiChar != '#') {
                    oldX=xPosition;
                    oldY=yPosition;
                    yPosition++ ;

                    if (blocked_by_monster(xPosition, yPosition)==3) {yPosition--;
                    lich.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the Lich, denting his rotten gray flesh.\n";
                    if (player.getitems() == '~') {
                      lich.set_hitdie();
                      cout<<"  The Lich recoils from your light.                         ";
                      }}
                    }

                 if(GetAsyncKeyState(VK_UP)&&screenBuffer2[xPosition + (yPosition-1) * SCREEN_WIDTH].Char.AsciiChar =='.') {
                    oldX=xPosition;
                    oldY=yPosition;
                    yPosition-- ;

                    if (blocked_by_monster(xPosition, yPosition)==3) {yPosition++;
                    lich.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the Lich, denting his rotten gray flesh.\n";
                    if (player.getitems() == '~') {
                      lich.set_hitdie();
                      cout<<"  The Lich recoils from your light.                       ";
                      }}
                    }

                 if(GetAsyncKeyState(VK_LEFT)&&screenBuffer2[(xPosition-1) + yPosition * SCREEN_WIDTH].Char.AsciiChar =='.') {
                    oldX=xPosition;
                    oldY=yPosition;
                    xPosition-- ;

                    if (blocked_by_monster(xPosition, yPosition)==3) {xPosition++;
                    lich.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the Lich, denting his rotten gray flesh.\n";
                    if (player.getitems() == '~') {
                      lich.set_hitdie();
                      cout<<"  The Lich recoils from your light.                         ";
                      }}
                    }

                 if(GetAsyncKeyState(VK_RIGHT)&&screenBuffer2[(xPosition+1) + yPosition * SCREEN_WIDTH].Char.AsciiChar =='.') {
                    oldX=xPosition;
                    oldY=yPosition;
                    xPosition++ ;

                    if (blocked_by_monster(xPosition, yPosition)==3) {xPosition--;
                    lich.setHP(player.hitdie());
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                    cout<<" You hit the Lich, denting his rotten gray flesh.\n";
                    if (player.getitems() == '~') {
                      lich.set_hitdie();
                      cout<<"  The Lich recoils from your light.                     ";
                      }}
                    }

                    if(lichdeathFlag == 0) {if(!lich.setHP(0)) {
                         lich.erase();
                         lichdeathFlag = 1;
                         if (lichdeathFlag) {
                            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;
                            cout<<" The Lich roars as his body disintegrates!                                  \n"
                                    <<"  Soon all that is left is a pile of dust.                             \n";

                                system("pause") ;

                                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), level_title) ;
                                cout<<"Heroic "<<player.showname()<<", you have destroyed the Lich!                \n"
                                <<"And now, your reward...                         \n" ;
                                system("pause") ;
                                cout<<"                                                                \n"
                                <<"                                                        \n"
                                <<"                                                \n"
<<"                                               .--. \n"
<<"                                               `.  \\ \n"
<<"                                                 \\  \\ \n"
<<"                                                  .  \\ \n"
<<"                                                  :   .\n"
<<"                                                  |    .\n"
<<"                                                  |    :\n"
<<"                                                  |    |\n"
<<"  ..._  ___                                       |    |\n"
<<" `.\"\".`''''\"\"--..___                              |    |\n"
<<" ,-\\  \\             \"\"-...__         _____________/    |\n"
<<" / ` \" '                    `\"\"\"\"\"\"\"\"                  .\n"
<<" \\                                                      L\n"
<<" (>                                                      \\ \n"
<<"/                                                         \\ \n"
<<"\\_    ___..---.                                            L\n"
<<"  `--'         '.                                           \\ \n"
<<"                 .                                           \\_\n"
<<"                _/`.                                           `.._\n"
<<"             .'     -.                                             `.\n"
<<"            /     __.-Y     /''''''-...___,...--------.._            |\n"
<<"           /   _.\"    |    /                ' .      \\   '---..._    |\n"
<<"          /   /      /    /                _,. '    ,/           |   |\n"
<<"          \\_,'     _.'   /              /''     _,-'            _|   |\n"
<<"                  '     /               `-----''               /     |\n"
<<"                  `...-'     dp                                !_____)\n" ;
break;



                                }}}

                    player.movePlayer(xPosition, yPosition, oldX, oldY) ;
                    Player_X_Location = xPosition;
                    Player_Y_Location = yPosition;

                    if(lich.setHP(0)&&lich.isAwake()) {lich.monster_movement();
                    damage = lich.hitdie() ;
                    player.setHP(damage) ;}

                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), HP_count) ;
                    cout<<" HP: "<<player.getHP()<<" ";
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), level_title) ;
                    cout<<"Throne room of the Lich King" ;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), action_text) ;

                    }
                    getch();}//ends while loop



                    if (!player.setHP(0))cout<<"You die...                                                   \n" ;

system("PAUSE") ;
return 0 ;
} //end of main

/*************************************************************************/
/** The rest of the functions in this program draw the game to the console.
/*************************************************************************/

void drawMap() {
     SMALL_RECT drawRect = {0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1};
	 COORD bufferSize = {SCREEN_WIDTH , SCREEN_HEIGHT};
	 COORD zeroZero = {0, 0};
	 HANDLE hOutput;
	 hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	 WriteConsoleOutput(hOutput, screenBuffer, bufferSize, zeroZero, &drawRect);
  }

void drawMap2() {
     SMALL_RECT drawRect = {0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1};
	 COORD bufferSize = {SCREEN_WIDTH , SCREEN_HEIGHT};
	 COORD zeroZero = {0, 0};
	 HANDLE hOutput;
	 hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	 WriteConsoleOutput(hOutput, screenBuffer2, bufferSize, zeroZero, &drawRect);
  }

void fillBuffer() {

/*Row 1*/
	for(int x = 1; x < 19; x++)	{
		screenBuffer[x + 1 * SCREEN_WIDTH].Char.AsciiChar = '#';
		screenBuffer[x + 1 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
		}
/*Row 2*/
for(int x = 1; x < 19; x++) {
        if (x == 1 || x == 18) {
        screenBuffer[x + 2 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 2 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else {
             screenBuffer[x + 2 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 2 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        }
/*Row 3*/
for (int x = 1; x<51; x++) {
    if (x == 1 || x == 18 ||  (x>31 && x<51)) {
        screenBuffer[x + 3 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 3 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else if (x>1 && x<18) {
             screenBuffer[x + 3 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 3 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        }
/*Row 4*/
for (int x = 1; x<51; x++) {
    if (x == 1 || x == 18 || x == 32 || x == 50) {
        screenBuffer[x + 4 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 4 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else if ((x>1 && x<18) || (x>32 && x<50)){
             screenBuffer[x + 4 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 4 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        } //y=14, x=53
/*Row 5*/
for (int x = 1; x<51; x++) {
    if (x == 1 || (x>17 && x<33) || x == 50) {
        screenBuffer[x + 5 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 5 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else if ((x>1 && x<18) || (x>32 && x<50)){
             screenBuffer[x + 5 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 5 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
             }
        }
/*Row 6*/
for (int x = 1; x<51; x++) {
    if (x == 1 || x == 50) {
        screenBuffer[x + 6 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 6 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else if (x>1 && x<50){
             screenBuffer[x + 6 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 6 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        }
/*Row 7*/
for (int x = 1; x<51; x++) {
    if ((x > 0 && x < 33) || x==50) {
        screenBuffer[x + 7 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 7 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else {
             screenBuffer[x + 7 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 7 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        }
    screenBuffer[7 + 7 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[7 + 7 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;

/*Row 8*/
screenBuffer[6 + 8 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[6 + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
screenBuffer[7 + 8 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
screenBuffer[7 + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
screenBuffer[8 + 8 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[8 + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
for(int x = 32; x<51; x++) {
        if (x==32 || x==50) {
        screenBuffer[x + 8 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
        screenBuffer[x + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
        }
        else {
             screenBuffer[x + 8 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
             screenBuffer[x + 8 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
             }
        }
/*Row 9*/
for (int x = 5; x < 10; x++) {
    if (x == 7) {
          screenBuffer[x + 9 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
          screenBuffer[x + 9 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
          }
    else {
         screenBuffer[x + 9 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
         screenBuffer[x + 9 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
         }
    }
for(int x = 32; x < 51; x++) {
        if (x == 33) {
           screenBuffer[x + 9 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
           screenBuffer[x + 9 * SCREEN_WIDTH].Attributes =FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
           }
           else {
                screenBuffer[x + 9 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
                screenBuffer[x + 9 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
                }
           }
/*Row 10*/
for (int x = 5; x < 10; x++) {
    if (x==5 || x==9) {
          screenBuffer[x + 10 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
          screenBuffer[x + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
          }
    else {
         screenBuffer[x + 10 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
         screenBuffer[x + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
         }
    }
screenBuffer[32 + 10 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[32 + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
screenBuffer[33 + 10 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
screenBuffer[33 + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
screenBuffer[34 + 10 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[34 + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
/*Row 11*/
for (int x = 5; x < 10; x++) {
          screenBuffer[x + 11 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
          screenBuffer[x + 11 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
    }
screenBuffer[32 + 11 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[32 + 11 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
screenBuffer[33 + 11 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
screenBuffer[33 + 11 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
screenBuffer[34 + 11 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
screenBuffer[34 + 11 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
/*Row 12*/
for (int x = 1; x<57; x++) {
    if (x==33) {
    screenBuffer[x + 12 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 12 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;
               }
    else {
    screenBuffer[x + 12 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 12 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY;
    }
}
/*Row 13*/
for (int x = 1; x<57; x++) {
    if (x==1 || x==56) {
    screenBuffer[x + 13 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 13 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
               }
    else {
    screenBuffer[x + 13 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 13 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
/*Row 14*/
for (int x = 1; x<57; x++) {
    if (x==1 || x==56) {
    screenBuffer[x + 14 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 14 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
               }
    else {
    screenBuffer[x + 14 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 14 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
/*Row 15*/
for (int x = 1; x<57; x++) {
    if (x==1 || x==56) {
    screenBuffer[x + 15 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 15 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
               }
    else {
    screenBuffer[x + 15 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 15 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
/*Row 16*/
for (int x = 1; x<57; x++) {
    if (x==1 || x==56) {
    screenBuffer[x + 16 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 16 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
               }
    else {
    screenBuffer[x + 16 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 16 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
/*Row 17*/
for (int x = 1; x<57; x++) {
    if (x==1 || x==56) {
    screenBuffer[x + 17 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 17 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
               }
    else {
    screenBuffer[x + 17 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer[x + 17 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
/*Row 18*/
for(int x = 1; x<57; x++) {
     screenBuffer[x + 18 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer[x + 18 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;
}

/*Special items:*/

//Super pretty bear:
screenBuffer[7 + 7 * SCREEN_WIDTH].Char.AsciiChar = 'B' ;
screenBuffer[7 + 7 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_BLUE ;
//Sunlight Torc:
screenBuffer[7 + 10 * SCREEN_WIDTH].Char.AsciiChar = '~' ;
screenBuffer[7 + 10 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN  ;
//Hole to the Lich's lair
screenBuffer[2 + 17 * SCREEN_WIDTH].Char.AsciiChar = 0 ;
//mirror
screenBuffer[53 + 14 * SCREEN_WIDTH].Char.AsciiChar = '0' ;
screenBuffer[53 + 14 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE ;

}

void drawStartScreen() {

     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),  11);

cout<<"  _____                           _    \n"
<<" |  __ \\                       /\\| |/\\ \n"
<<" | |__) |___   __ _ _   _  ___ \\ ` ' / \n"
<<" |  _  // _ \\ / _` | | | |/ _ \\_     _| \n"
<<" | | \\ \\ (_) | (_| | |_| |  __// , . \\ \n"
<<" |_|  \\_\\___/ \\__, |\\__,_|\\___|\\/|_|\\/ \n"
<<"               __/ |                   \n"
<<"              |___/                    \n" ;

  }

int blocked_by_monster (int x, int y) {
     if ((bat.get_x_position() == x) && (bat.get_y_position() == y)) return 1;
     else if ((kobold.get_x_position() == x) && (kobold.get_y_position() == y)) return 2;
     else if ((lich.get_x_position() == x) && (lich.get_y_position() == y)) return 3;
     else return 0;
     }

void fillBuffer2() {
     //Row 1, Lich's lair
     for (int x = 1; x <12; x++){
    screenBuffer2[x + 1 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
    screenBuffer2[x + 1 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;

}
    //Row 2, Lich's lair
    for (int x = 1; x < 12; x++) {
        if (x == 1 || x==11) {
              screenBuffer2[x + 2 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 2 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
              else {
    screenBuffer2[x + 2 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer2[x + 2 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
}
    //Row 3, Lich's lair
     for (int x = 1; x < 12; x++) {
        if (x == 1 || x==2 || x ==10 || x==11) {
              screenBuffer2[x + 3 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 3 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
              else {
    screenBuffer2[x + 3 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer2[x + 3 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
}
    //Row 4, Lich's Lair
    for (int x = 2; x < 11; x++) {
        if (x == 2 || x == 10) {
              screenBuffer2[x + 4 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 4 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
        else {
    screenBuffer2[x + 4 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer2[x + 4 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
}
    //Row 5, Lich's Lair
    for (int x = 2; x < 11; x++) {
        if (x == 2 || x == 3 || x == 9 || x == 10) {
              screenBuffer2[x + 5 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 5 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
        else {
    screenBuffer2[x + 5 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
    screenBuffer2[x + 5 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
}
    //Row 6, Lich's lair
    for (int x = 3; x < 10; x++) {
        if (x == 3 || x == 9) {
              screenBuffer2[x + 6 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 6 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
              else {
                   screenBuffer2[x + 6 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
                   screenBuffer2[x + 6 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
                   }
    //Row 7, Lich's lair
    for (int x = 3; x < 10; x++) {
        if (x == 3 || x == 4 || x ==8 || x == 9) {
              screenBuffer2[x + 7 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 7 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
              else {
                   screenBuffer2[x + 7 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
                   screenBuffer2[x + 7 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
                   }
    //Row 8, Lich's lair
    for (int x = 4; x < 9; x++) {
        if (x == 4 || x == 8) {
              screenBuffer2[x + 8 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
              else {
                   screenBuffer2[x + 8 * SCREEN_WIDTH].Char.AsciiChar = '.' ;
                   screenBuffer2[x + 8 * SCREEN_WIDTH].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;}
                   }
    //Row 9, lich's lair
    for (int x = 4; x < 9; x++) {
              screenBuffer2[x + 9 * SCREEN_WIDTH].Char.AsciiChar = '#' ;
              screenBuffer2[x + 9 * SCREEN_WIDTH].Attributes = FOREGROUND_INTENSITY ;}
                   }
