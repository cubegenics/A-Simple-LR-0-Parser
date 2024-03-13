#include "headers.h"

#pragma once
extern std::map<char, std::vector<char>> first_sets;
extern std::map<char, std::vector<char>> follow_sets;

std::map<char, std::vector<char>> compute_first(const std::vector<rule> &production_rules);
std::map<char, std::vector<char>> compute_follow(const std::vector<rule> &production_rules, std::map<char, std::vector<char>> &first);
void print_first_sets();
void print_follow_sets();
