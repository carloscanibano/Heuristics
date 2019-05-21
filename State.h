#ifndef State_h
#define State_h
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

using namespace std;

struct State{
public:
	//Reference
	int id;
	//Father reference
	int father_id;
	//Al x-axis position
	int al_x_pos;
	//Al y-axis position
	int al_y_pos;
	//Father x-axis position
	int father_x;
	//Father y-axis position
	int father_y;
	//Key state in this moment
	vector<tuple<int, int>> state_keys;
	//Rock state in this moment
	vector<tuple<int, int>> state_rocks;
	//f(n) = g(n) + h(n)
	double f;
	//g(n) cost to move
	int g;
	//h(n) heuristic cost
	double h;
	//Custom operator for sorting by f(n) value
	bool operator < (const State &other) const {
    	return f < other.f;
    }
    //Custom operator for find
    bool operator == (const State& other) const {
    	bool equals = false;
    	int counter = 0;
    	tuple<int, int> rock;
    	tuple<int, int> key;

    	//First, we check for the same x and y positions
    	if((al_x_pos == other.al_x_pos) && (al_y_pos == other.al_y_pos)){
    		equals = true;
    		//If the keys vector has te same size then we continue
	    	if((state_keys.size() == other.state_keys.size())){
	    		//Comparing keys one by one
	    		while((counter < state_keys.size()) && (equals)){
	    			key = other.state_keys.at(counter);
	    			if(find(state_keys.begin(), state_keys.end(), key) == state_keys.end()){
	    				equals = false;
	    			}
	    			counter++;
	    		}
	    	} else {
	    		equals = false;
	    	}
	    	//We only need to check rocks in case keys check was successful
    		if(equals){
	    		counter = 0;
	    		//Comparing rocks one by one
	    		while((counter < state_rocks.size()) && (equals)){
	    			rock = other.state_rocks.at(counter);
	    			if(find(state_rocks.begin(), state_rocks.end(), rock) == state_rocks.end()){
	    				equals = false;
	    			}
	    			counter++;
	    		}
    		}
    	}

    	return equals;
    }
	//Default constructor
	State();
};

State::State(){
	id = 0;
	father_id = 0;
	father_x = -1;
	father_y = -1;
	al_x_pos = 0;
	al_y_pos = 0;
	f = 0.0;
	g = 0;
	h = 0.0;
}
#endif
