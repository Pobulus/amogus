#pragma once
#include "amogus.h"
const std::string impostor = 
"YOU ARE THE \n"
"         ___ __  __ ___  ___  ___ _____ ___  ___ \n"
"        |_ _|  \\/  | _ \\/ _ \\/ __|_   _/ _ \\| _ \\ \n"
"         | || |\\/| |  _/ (_) \\__ \\ | || (_) |   / \n"
"        |___|_|  |_|_|  \\___/|___/ |_| \\___/|_|_\\ \n"
"               Kill other crewmates to win.";
const std::string crewm8 = 
"YOU ARE THE\n"
"          ___ ___ _____      ____  __   _ _____ ___ \n"
"         / __| _ \\ __\\ \\    / /  \\/  | /_\\_   _| __|\n"
"        | (__|   / _| \\ \\/\\/ /| |\\/| |/ _ \\| | | _| \n"
"         \\___|_|_\\___| \\_/\\_/ |_|  |_/_/ \\_\\_| |___|\n"
"               Complete all your tasks to win.";
void displayRole(int status);
void drawSquare(int x1, int y1, int w, int h, std::string character);
void drawTaskbar(int percent);
void drawHexagon(int x, int y);
void drawTesttube(int x, int y);
void drawPipet(int x, int y);
void drawHand(int x, int y);
void drawSine(double f, double A, int t, int x);
void drawRing(const int x, const int y, const int angle, const int color);
void drawExplosion(const int x, const int y);
void drawAsteroid(const int x, const int y, const int r, std::string characters);
void drawBox(int x1, int y1, int w, int h, std::string type);
void drawCircle(const int x, const int y, const int r, std::string characters);
/**
 * Draws the map from specified perspecitve
 * @param gmap game map data
 * @param x horizontal position
 * @param y vertical position
 */
void drawMap(std::vector<std::string> &gmap, std::vector<std::string> &wallmap, int x, int y, int sabbo, bool camera=false);

/**
 * prints a file, replacing characters with specified colors. 
 * @param colA color to replace 'a' and 'A'
 * @param colB color to replace 'b' and 'B'
 * @param filename name of the text file to display
 */
void cutscene(int colA, int colB, std::string filename);
/**
 * Displays text in the center of a box
 * @param text the text to display
 * @param limX box width
 * @param limY box height
 * @param offX box offset in x cooridnate (default: 0)
 * @param offY box offset in y cooridnate (default: 0)
 */
void printCenter(std::string text, const int limX, const int limY, const int offX=0, const int offY=0);
