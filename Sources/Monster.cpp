#include "Monster.h"
#include "Player.h"
#include<iostream>
#include<string>

/**
    Constructor for Monsters.

    @param a monster's name
    @param b monster's hitpoints
    @param c number of rolls in the monster's hitdie
    @param d number of sides in the monster's hitdie
    @param e ASCII character representing the monster
    @param g true if monster is awake
    @param h color of monster's avatar
    @param i monster's starting x position
    @param j monster's starting y position
*/
monster::monster(std::string a, int b, int c, int d,
   const CHAR * e, bool g, int h, int i, int j) {
   name = a;
   HP = b;
   dierolls = c;
   numSides = d;
   avatar = e;
   awake = g;
   color = h;
   xPosition = i;
   yPosition = j;
}

/**
    Determines how much damage the monster takes.
    If its health reaches 0 or less, return false.

    @param damage amount of damage the player deals to the monster
    @return whether or not the monster has health left
*/
bool monster::setHP(int damage) {
   HP = HP - damage;
   return HP>0;
}

int monster::get_x_position() {
   return xPosition;
}

int monster::get_y_position() {
   return yPosition;
}

/**
    Resets monster's hitdie to 1d1.
*/
void monster::set_hitdie() {
   numSides = 1;
   dierolls = 1;
}
