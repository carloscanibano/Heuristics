#ifndef State_h
#define State_h
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;

struct State{
public:
	//Father state reference
	State father;
	//Al x-axis position
	int al_x_pos;
	//Al y-axis position
	int al_y_pos;
	//Key state in this moment
	vector<tuple<int, int>> state_keys;
	//Rock state in this moment
	vector<tuple<int, int>> state_rocks;
	//f(n) = g(n) + h(n)
	int f;
	//g(n) cost to move
	int g;
	//h(n) heuristic cost
	int h;
	//Allow father generation
	bool allow_father;
	//Custom operator for sorting
	bool operator < (const State &other) const {
    	return f < other.f;
    }
    //Custom operator for find
    bool operator == (const State& other) const {
    	bool foundKey = true;
    	bool foundRock = true;
    	int counter = 0;
    	tuple<int, int> rock;
    	tuple<int, int> key;

    	if((al_x_pos == other.al_x_pos) && (al_y_pos == other.al_y_pos) && (state_keys.size() == other.state_keys.size())){
    		while((counter < state_keys.size()) && (foundKey)){
    			key = other.state_keys.at(counter);
    			if(find(state_keys.begin(), state_keys.end(), key) == state_keys.end()){
    				foundKey = false;
    			}
    			counter++;
    		}

    		if(foundKey){
	    		counter = 0;

	    		while((counter < state_rocks.size()) && (foundRock)){
	    			rock = other.state_keys.at(counter);
	    			if(find(state_rocks.begin(), state_rocks.end(), rock) == state_rocks.end()){
	    				foundRock = false;
	    			}
	    			counter++;
	    		}
    		}
    	}
    	return (foundRock && foundKey);
    }
	//Default constructor
	State();
};

State::State(){
	al_x_pos = 0;
	al_y_pos = 0;
	f = 0;
	g = 0;
	h = 0;
	allow_father = false;
}
#endif