#include "State.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
using namespace std;

void set_Al_initial_data(State& state, string f, tuple<int,int>& g,
							 vector<tuple<int, int>>& w, vector<tuple<int, int>>& s,
							 int& rows, int& cols){
	ifstream file(f);
	string line;
	int col = 0;

	while(getline(file, line)){
			if(cols == 0) cols = line.length();
		for(char& c : line){
		    if(c == 'A'){
		    	state.al_x_pos = rows;
		    	state.al_y_pos = col;
		    } else if(c == 'K'){
		    	state.state_keys.push_back(make_tuple(rows, col));
		    } else if(c == 'E'){
		    	g = make_tuple(rows, col);
		    } else if(c == '%'){
		    	w.push_back(make_tuple(rows, col));
		    } else if(c == 'O'){
		    	state.state_rocks.push_back(make_tuple(rows, col));
		    } else if(c == 'S'){
		    	s.push_back(make_tuple(rows, col));
		    }
		    col++;
		}
		col = 0;
		rows++;
	}
}

State compare_open_close(vector<State>& o, vector<State>& c){
	State state1;
	State state2;
	bool found = false;
	for(int i = 0; (i < o.size()) && (!found); i++){
		state1 = o.at(i);
		for(int j = 0; (j < c.size()) && (!found); j++){
			state2 = c.at(j);
			if((state1.al_x_pos != state2.al_x_pos) || (state1.al_y_pos != state2.al_y_pos)){
				o.erase(o.begin() + i);
				found = true;
			}
		}
	}
	return state1;
}

bool isGoal(State& s, tuple<int, int> g){
	bool goal = false;
	if(s.state_keys.empty() && (get<0>(g) == s.al_x_pos) && (get<1>(g) == s.al_y_pos)){
		goal = true;
	}
	return goal;
}

State move_Al(State& s, int x, int y){
	State n = State();

	n.state_keys = s.state_keys;

	if(find(s.state_keys.begin(), s.state_keys.end(), make_tuple(x,y)) != s.state_keys.end()){
		int it = find(s.state_keys.begin(), s.state_keys.end(), make_tuple(x,y));
		n.state_keys = n.state_keys.erase(it);
	}
}

int main(int argc, char *argv[]) {
	string file_path = argv[1];
	State state = State();
	vector<State> open;
	vector<State> close;
	vector<tuple<int, int>> walls;
	vector<tuple<int, int>> snakes;
	tuple<int, int> goal;
	bool completed = false;
	int rows = 0;
	int cols = 0;

	set_Al_initial_data(state, file_path, goal, walls, snakes, rows, cols);
	open.push_back(state);

	/*
	while(!open.empty() || completed){
		state = compare_open_close(open, close);
		if(isGoal(state)){
			completed = true;
		} else {
			
		}
	}
	*/
	
	/*PRINT TUPLE
    cout << "(" << std::get<0>(keys[0]) << ", " << std::get<1>(keys[0])
              <<")\n";
              */
}