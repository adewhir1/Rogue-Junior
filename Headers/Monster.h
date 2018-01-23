#ifndef MONSTER_H
#define MONSTER_H

#include<iostream>
#include<string>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<windows.h>

class monster {
      public:
      std::string name ;
      int HP, dierolls, numSides, xPosition, yPosition, oldX, oldY ;
      const CHAR* avatar ;
      int color ;
      bool awake ;
      monster(std::string, int, int, int, const CHAR*, bool, int, int, int) ;
      int hitdie() ;
      bool setHP(int);
      void erase();
      void monster_movement() ;
      bool isAwake();
      int get_x_position();
      int get_y_position();
      void set_hitdie() ;
} ;

#endif
