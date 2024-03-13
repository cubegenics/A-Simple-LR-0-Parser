#include "headers.h"
#include "first_follow.h"
#include "lr0.h"
using namespace std;

char start_symbol;
set<char> terminals;
set<char> non_terminals;
vector<rule> production_rules;

void take_input() {
    //takes input from the user and does 2 additional things:
    //i) updates production_rules 
    //ii) updates start symbol, terminals and non_terminals

    int n;
    cout << "Enter number of production rules: ";
    cin >> n;

    cout << "Enter rules:\n";
    vector<string> rules(n);
    for (auto &r: rules) {
        cin >> r;
    }

    start_symbol = rules[0][0];

    for (auto &r: rules) {
        //r is of the form: A->a|b|c
        char lhs = r[0];
        non_terminals.insert(lhs);
        string buffer = "";
        for (int i = 3; i < (int)r.size(); i++) {
            if (r[i] == '|') {
                rule new_rule = rule(lhs, buffer);
                production_rules.push_back(new_rule);
                buffer.clear();
                continue;
            }
            buffer += r[i];
            if (r[i] - 'A' >= 0 && r[i] - 'A' <= 25) {
                non_terminals.insert(r[i]);
            } else {
                if (r[i] != '@') {
                    terminals.insert(r[i]);
                }
            }
        }
        rule new_rule = rule(lhs, buffer);
        production_rules.push_back(new_rule);
    }

    terminals.insert('$');
}

int main() {

    take_input();
    first_sets = compute_first(production_rules);
    follow_sets = compute_follow(production_rules, first_sets);

    print_first_sets();  
    print_follow_sets();


    lr0_item start_state = initialize_lr0_rules();

    lr0_graph graph(start_state);
    graph.construct_graph();
    graph.print();


    lr0_table table(graph);
    table.print();

    return 0;
}