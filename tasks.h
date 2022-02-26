#pragma once
#include "amogus.h"
bool handleVent(taskStruct tasks, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id, int &sabbo);
void pickSabbotage(char buff[MAX], int sabbo, int sabbo_countdown, keyBinds kBinds);
bool handleTask(taskStruct &tasks, std::map<char,std::vector<std::string>> &triggers, const keyBinds kBinds, std::map<int, crewmate>  &positions, std::vector<std::string> &gamemap,std::vector<std::string> &wallmap, playermodel model, int sockfd, int id, int &sample_countdown, int &sabbo);
std::map<int, int> checkLevels(const taskStruct tasks);
