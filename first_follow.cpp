#include "first_follow.h"
using namespace std;

map<char, vector<char>> first_sets;
map<char, vector<char>> follow_sets;

map<char, vector<char>> compute_first(const vector<rule> &production_rules) {
    
    map<char, vector<char>> first, new_first;
    while (1) {
        new_first.clear();

        new_first = first;

        for (const rule &r: production_rules) {
            char lhs = r.lhs;
            string rhs = r.rhs;

            int epsilon_count = 0;
            for (const char &c: rhs) {
                if (non_terminals.count(c)) {
                    bool epsilon_present = 0;
                    for (const char& c1: first[c]) {
                        if (c1 != '@') {
                            new_first[lhs].push_back(c1);
                        } else {
                            epsilon_present = 1;
                        }
                    }
                    if (!epsilon_present) {
                        break;
                    } else {
                        epsilon_count++;
                    }
                } else {
                    new_first[lhs].push_back(c);
                    break;
                }
            }

            if (epsilon_count == (int)rhs.size()) {
                new_first[lhs].push_back('@');
            }
        }

        //modify the first sets to only contain distinct elements
        for (auto &p: new_first) {
            sort(p.second.begin(), p.second.end());
            int distinct = unique(p.second.begin(), p.second.end()) - p.second.begin();
            p.second.resize(distinct);
        }

        if (first == new_first) {
            break;
        }

        first = new_first;
    }

    return first;
}

map<char, vector<char>> compute_follow(const vector<rule> &production_rules, map<char, vector<char>> &first) {
    
    map<char, vector<char>> follow, new_follow;
    follow[start_symbol].push_back('$');

    while (1) {

        new_follow.clear();

        new_follow = follow;

        for (const rule &r: production_rules) {
            char lhs = r.lhs;
            string rhs = r.rhs;
            int k = (int)rhs.size();

            for (int i = 0; i < k; i++) {
                char &cur_symbol = rhs[i];
                if (!non_terminals.count(cur_symbol)) {
                    continue;
                }
                if (i == k - 1) {
                    for (const char&c: follow[lhs]) {
                        new_follow[cur_symbol].push_back(c);
                    }
                } else {
                    int epsilon_count = 0;
                    for (int j = i + 1; j < k; j++) {
                        char next_symbol = rhs[j];
                        if (non_terminals.count(rhs[j])) {
                            bool epsilon_present = 0;
                            for (const char &c: first[next_symbol]) {
                                if (c != '@') {
                                    new_follow[cur_symbol].push_back(c);
                                } else {
                                    epsilon_present = 1;
                                }
                            }
                            if (!epsilon_present) {
                                break;
                            } else {
                                epsilon_count++;
                            }
                        } else {
                            if (next_symbol != '@') {
                                new_follow[cur_symbol].push_back(next_symbol);
                                break;
                            }
                        }
                    }
                    if (epsilon_count == k - i - 1) {
                        for (const char&c: follow[lhs]) {
                            new_follow[cur_symbol].push_back(c);
                        }                       
                    }
                }
            }
        }

        //modify the follow sets to only contain distinct elements
        for (auto &p: new_follow) {
            sort(p.second.begin(), p.second.end());
            int distinct = unique(p.second.begin(), p.second.end()) - p.second.begin();
            p.second.resize(distinct);
        }

        if (follow == new_follow) {
            break;
        }

        follow = new_follow;
    }

    return follow;
}

void print_first_sets() {
    cout << "\nFIRST SETS\n";
    for (auto &p: first_sets) {
        cout << p.first << ": ";
        cout << "{";
        for (int i = 0; i < (int)p.second.size(); i++) {
            cout << p.second[i] << ",}"[i == (int)p.second.size() - 1];
        }
        cout << "\n";
    }
    for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
}

void print_follow_sets() {
    cout << "\nFOLLOW SETS\n";
    for (auto &p: follow_sets) {
        cout << p.first << ": ";
        cout << "{";
        for (int i = 0; i < (int)p.second.size(); i++) {
            cout << p.second[i] << ",}"[i == (int)p.second.size() - 1];
        }
        cout << "\n";
    }
    for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
}