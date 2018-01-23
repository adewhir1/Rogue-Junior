#include "Player.h"
#include<iostream>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<windows.h>

/**
    Initializes a new Player object.

    @param hitpoints player starts game with this many hitpoints
    @param owns player's inventory slot
*/
Player::Player(int hitpoints, char owns) {
   HP = hitpoints;
   carrying = owns;
}

/**
    Called when the player hits a monster.

    @return the amount of damage the player deals that turn
*/
int Player::hitdie() {
   srand(time(0));
   return 2 * (rand() % 4 + 1);
}

/**
    Allows the user to name their character.
*/
void Player::getname() {
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
   std::cout << "\nFind and destroy the Lich King!\n" << "Enter your name to begin:    ";
   std::cin >> name;
}

/**
    Decrements the player's health when they are hit by a monster.
    Returns false when the player has no health left.

    @param damage amount of damage dealt to the player
    @return whether or not the player has any health left
*/
bool Player::setHP(int damage) {
   HP = HP - damage;
   return HP > 0;
}

int Player::getHP() {
   return HP;
}

/**
    Adds an item to the player's inventory.

    @param item the object to be added to the player's inventory
*/
void Player::setitems(char item) {
   carrying = item;
}

char Player::getitems() {
   return carrying;
}

std::string Player::showname() {
   return name;
}
