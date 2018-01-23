#ifndef PLAYER_H
#define PLAYER_H

#include<iostream>
#include<string>

/**
    Contains variable names and function prototypes for the Player class.

    @version 14 Oct 2015
    @author Andrea Dewhirst
*/

class Player {
   public:
   std::string name;
   int HP;
   char carrying;
   int hitdie();
   void getname();
   bool setHP(int);
   void movePlayer(int, int, int, int);
   int getHP();
   void setitems(char);
   char getitems();
   std::string showname();
   Player(int, char);
};
#endif
