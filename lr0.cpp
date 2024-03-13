#include "lr0.h"
using namespace std;

vector<modified_rule> lr0_production_rules;
map<modified_rule, int> rule_index;

modified_rule modified_rule::update_dot() {
    string str_temp(1, lhs);
    str_temp += "->";
    str_temp += rhs;
    modified_rule temp(str_temp);
    int idx = get_dot_pos();
    if (idx != (int)rhs.size() - 1) {
        swap(temp.rhs[idx], temp.rhs[idx + 1]);
    }
    return temp;
}

void lr0_item::compute_closure() {

    vector<modified_rule> new_rules = rules;

    while (1) {

        //begin
        // for (auto &r: rules) {
        //     r.print();
        // }       

        // cout << "@@@@@@\n";

        // for (auto &r: new_rules) {
        //     r.print();
        // }
        // for (int i = 0; i < 20; i++) cout << '-';
        // puts("");
        //end


        for (auto &cur_rule: rules) {
            int k = (int)cur_rule.rhs.size();
            int dot_pos = cur_rule.get_dot_pos();
            if (dot_pos < k - 1) {
                char next_symbol = cur_rule.rhs[dot_pos + 1];
                if (!non_terminals.count(next_symbol)) continue;
                for (auto &r: production_rules) {
                    if (r.lhs == next_symbol) {
                        string new_rule;
                        new_rule += r.lhs;
                        new_rule += "->.";
                        new_rule += r.rhs;
                        modified_rule to_add = modified_rule(new_rule);
                        new_rules.push_back(to_add);                       
                    }
                }

            }
            
        }

        sort(new_rules.begin(), new_rules.end());
        int distinct = unique(new_rules.begin(), new_rules.end()) - new_rules.begin();
        new_rules.resize(distinct);

        if ((int)rules.size() == (int)new_rules.size()) {
            break;
        }

        rules = new_rules;
    }

    for (auto &cur_rule: rules) {
        int k = (int)cur_rule.rhs.size();
        int dot_pos = cur_rule.get_dot_pos();
        if (dot_pos != k - 1) {
            transition_symbols.push_back(cur_rule.rhs[dot_pos + 1]);
        }
    }

    // for (auto &r: rules) {
    //     r.print();
    // }       
    // debug(transition_symbols);
    // puts("///////////");

    int distinct = unique(transition_symbols.begin(), transition_symbols.end()) - transition_symbols.begin();
    transition_symbols.resize(distinct);
}

shared_ptr<lr0_item> lr0_item::compute_goto(const char &c) {
    lr0_item goto_item;

    for (auto &cur_rule: rules) {
        int k = (int)cur_rule.rhs.size();
        int idx = cur_rule.get_dot_pos();
        if (idx != k - 1) {
            char next_symbol = cur_rule.rhs[idx + 1];
            if (next_symbol == c) {
                modified_rule new_rule = cur_rule.update_dot();
                goto_item.add_rule(new_rule);
            }
        }
    }

    return make_unique<lr0_item>(goto_item);
}

bool lr0_item::contains_reduce_rule() {
    for (auto &r: rules) {
        if (r.is_reduce_rule()) return 1;
    }
    return 0;
}

void lr0_item::print() {
    for (auto &cur_rule: rules) {
        cur_rule.print();
    }
    // for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
}


void lr0_graph::construct_outgoing_edges(shared_ptr<lr0_item> &cur_item) {

    for (const char &cur_symbol: cur_item->transition_symbols) {
        shared_ptr<lr0_item> next_item = cur_item->compute_goto(cur_symbol);

        next_item->compute_closure();
        bool is_new_state = 1;

        for (auto &s: states) {
            if (*s == *next_item) {
                next_item = s;
                is_new_state = 0;
                break;
            }
        }

        if (is_new_state) {
            next_item->id = next_available_id++;
        }

        adj[cur_item].push_back(std::make_pair(cur_symbol, next_item));
    }
}

void lr0_graph::construct_graph() {
    int cur_state_idx = 0;
    int processed_states = 0;

    while (1) {

        shared_ptr<lr0_item> &cur_state = states[cur_state_idx];
        // puts("CUR STATE///////////////");
        // cur_state.print();
        // puts("ADJ LIST");

        construct_outgoing_edges(cur_state);

        // cur_state.id = cur_state_idx;
        processed_states++;

        // bool new_state_added = 0;

        for (auto &p: adj[cur_state]) {
            shared_ptr<lr0_item> &next_state = p.second;
            // if (find(states.begin(), states.end(), next_state) == states.end()) {
            //     states.push_back(next_state);
            // }

            bool is_new_state = 1;
            for (auto &s: states) {
                if (*s == *next_state) {
                    is_new_state = 0;
                    break;
                }
            }
            if (is_new_state) {
                states.push_back(next_state);
            }


            // next_state->print();

            // new_state_added = 1;
        }

        if (processed_states == (int)states.size()) {
            break;
        }

        // puts("///////////////////");
        // puts("///////////////////");
        // debug(processed_states, (int)states.size());

        cur_state_idx++;
        // if (new_state_added) {
        //     cur_state_idx++;
        // } else {
        //     break;
        // }
    }
    
    for (int i = 0; i < (int)states.size(); i++) {
        if (!adj.count(states[i])) {
            adj[states[i]] = {};
        }
    }
}

void lr0_graph::print() {
    cout << "\nLR(0) GRAPH:\n";

    cout << "STATES:\n\n";
    for (auto &state: states) {
        cout << "Item " << state->id << '\n';
        state->print();
    }


    for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
    cout << "EDGES:\n";
    for (auto &p: adj) {
        for (auto &transition: p.second) {
            cout << "(" << p.first->id << " to " << transition.second->id \
                 << ") using symbol " << transition.first << '\n';
        }
    }

    for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
}


void lr0_table::construct_table(lr0_graph graph) {
    for (auto &p: graph.adj) {
        shared_ptr<lr0_item> cur_item = p.first;

        if (cur_item->contains_reduce_rule()) {
            //need to add actions of "REDUCE" type

            for (auto &cur_rule: cur_item->rules) {
                //iterate over all reduce rules and 
                //fill the table accordingly
                if (cur_rule.is_reduce_rule()) {
                    for (auto &c: terminals) {
                        string action;
                        action += 'R';
                        action += to_string(rule_index[cur_rule]);
                        table_action[make_pair(cur_item->id, c)].push_back(action);
                    }
                }
            }
        }

        for (auto &tmp: p.second) {
            char symbol = tmp.first;
            shared_ptr<lr0_item> nxt_item = tmp.second;
            if (non_terminals.count(symbol)) {
                //transition is of "GOTO" type
                table_goto[make_pair(cur_item->id, symbol)].push_back(nxt_item->id);
            } else {
                //transition is of "ACTION" type, and that too, of "SHIFT" type
                    string action;
                    action += 'S';
                    action += to_string(nxt_item->id);
                table_action[make_pair(cur_item->id, symbol)].push_back(action);
            }
        }
    }
}

void lr0_table::print() { 

    cout << "\nLR(0) TABLE\n";

    cout << "Action\n";
    for (auto &p: table_action) {
        int i = p.first.first;
        char symbol = p.first.second;

        for (auto &p2: p.second) {
            cout << "{" << i << ", " << symbol << "}: " << p2 << '\n';
        }
    }

    cout << "\nGoto\n";
    for (auto &p: table_goto) {
        int i = p.first.first;
        char symbol = p.first.second;

        for (auto &j: p.second) {
            cout << "{" << i << ", " << symbol << "}: " << j << '\n';
        }
    }

    for (int i = 0; i < 50; i++) cout << '-';
    cout << '\n';
}

lr0_item initialize_lr0_rules() {
    //forms all the lr0_production rules, 
    //in addition to augmenting the grammar

    //also forms the base lr0 state to pass on to 
    //the lr0_graph for constructing the graph

    //finally, it assigns an index to each modified rule

    string aug_rule = "Z->.";
    aug_rule += start_symbol;

    modified_rule augmented_rule = modified_rule(aug_rule);
    lr0_production_rules.push_back(augmented_rule);

    for (auto &rule_: production_rules) {
        lr0_production_rules.push_back(modified_rule(rule_.lhs, string("." + rule_.rhs)));
    }

    lr0_item start_state(0);
    start_state.add_rule(augmented_rule);

    cout << "\nRule Indices:\n";
    int rule_idx = 0;
    for (auto &mr: lr0_production_rules) {
        cout << rule_idx << ": ";
        mr.print();
        rule_index[mr] = rule_idx++;
    }

    return start_state;
}