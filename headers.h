#include<bits/stdc++.h>

#pragma once

struct rule {
    char lhs;
    std::string rhs;

    rule(std::string production_rule) {
        lhs = production_rule[0];
        rhs = production_rule.substr(3);
    }
    
    rule(char lhs, std::string rhs) 
        : lhs(lhs), rhs(rhs)
    {}
    
    void print() {
        std::cout << lhs << "->" << rhs << '\n';
    }
};

extern char start_symbol;
extern std::set<char> terminals;
extern std::set<char> non_terminals;
extern std::vector<rule> production_rules;