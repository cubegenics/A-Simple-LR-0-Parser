#include "headers.h"

#pragma once

struct modified_rule;
struct lr0_item;
class lr0_graph;
class lr0_table;

extern std::vector<modified_rule> lr0_production_rules;
extern std::map<modified_rule, int> rule_index;

lr0_item initialize_lr0_rules();

struct modified_rule {
    //rule but it contains a "dot" to
    //denote how much of the std::string we've seen so far
    char lhs;
    std::string rhs;
    int id;

    modified_rule() {}

    modified_rule(std::string rule_) {
        lhs = rule_[0];
        rhs = rule_.substr(3);
    }


    modified_rule(char lhs, std::string rhs) 
        : lhs(lhs), rhs(rhs)
    {}

    int get_dot_pos() {
        int idx = find(rhs.begin(), rhs.end(), '.') - rhs.begin();
        return idx;
    }

    void print() {
        std::cout << lhs << "->" << rhs << '\n';
    }

    bool operator<(const modified_rule& other) const {
        if ((int)rhs.size() == (int)other.rhs.size()) {
            return lhs < other.lhs;
        } else {
            return (int)rhs.size() < (int)other.rhs.size();
        }
    }

    bool operator>(const modified_rule& other) const {
        if ((int)rhs.size() == (int)other.rhs.size()) {
            return lhs > other.lhs;
        } else {
            return (int)rhs.size() > (int)other.rhs.size();
        }
    }

    bool operator==(const modified_rule& other) const {
        return (lhs == other.lhs && rhs == other.rhs);
    }

    bool operator!=(const modified_rule& other) const {
        return !(lhs == other.lhs && rhs == other.rhs);
    }

    modified_rule update_dot();

    bool is_reduce_rule() {
        int k = (int)rhs.size();
        int dot_pos = get_dot_pos();
        return dot_pos == k - 1;
    }

};

struct lr0_item {
    std::vector<modified_rule> rules;
    std::vector<char> transition_symbols; //filled in complete_closure()
    int id;

    lr0_item() {}

    lr0_item(int id) 
        : id(id)
    {}

    bool operator<(const lr0_item &rhs) const {
        return id < rhs.id;
    }

    bool operator==(const lr0_item &rhs) const {
        return (rules == rhs.rules);
    }


    void add_rule(const modified_rule &new_rule) {
        rules.push_back(new_rule);
    }

    void compute_closure(); 

    std::shared_ptr<lr0_item> compute_goto(const char &c);

    bool contains_reduce_rule();

    void print();
};


class lr0_graph {
  private:
    std::shared_ptr<lr0_item> start_state; //the start state must have the minimum necessary rules, 
                                      //it need not have its closure computed before passing 
                                      //to the constructor
    int next_available_id; //to assign numbers to states
    std::map<std::shared_ptr<lr0_item>, std::vector<std::pair<char, std::shared_ptr<lr0_item>>>> adj;
    std::vector<std::shared_ptr<lr0_item>> states;

    void construct_outgoing_edges(std::shared_ptr<lr0_item> &cur_item);

  public:
    lr0_graph(lr0_item start)
        : start_state(std::make_shared<lr0_item>(start)), next_available_id(1)
    {
        start_state->compute_closure();
        start_state->id = 0;
        states.push_back(start_state);
    }

    void construct_graph();
    void print();

    friend class lr0_table;
};


class lr0_table {
  private:
    std::map<std::pair<int, char>, std::vector<std::string>> table_action;//(cur_state, transition_symbol) -> (action + new_state) e.x. S2, R3
    std::map<std::pair<int, char>, std::vector<int>> table_goto;//(cur_state, transition_symbol) -> (new_state)

  public:
    lr0_table(lr0_graph graph) { 
        construct_table(graph);
    }

    void construct_table(lr0_graph graph);

    void print();
};
