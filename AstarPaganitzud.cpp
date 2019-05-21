#include "State.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <time.h>
using namespace std;

//Time counter
clock_t tStart = clock();

/*
This function gets all data from the entry file and parses it to create
all map keys, rocks, snakes, walls, Alabama initial grid position, exit position 
and also counts rows and columns.
*/
void set_Al_initial_data(State& state, string f, tuple<int,int>& g,
							 vector<tuple<int, int>>& w, vector<tuple<int, int>>& s,
							 int& rows, int& cols){
	ifstream file(f);
	string line;
	int col = 0;

	cout << "\nLet's make Al survive!\n" << endl;

	//Reading file line by line printing each different type of element
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
		    cout << c;
		    col++;
		}
		cout << endl;
		col = 0;
		rows++;
	}
}

/*
This function compares two lists: OPEN and CLOSE. At the end of each iteration,
we need to know if the new potential node it's not in OPEN nor CLOSE. In case it's
only in OPEN, we need to calculate which f(n) is lower and, in affirmative case, 
change one node for the other. At the end, we need to sort OPEN list by f(n).
*/
void compare_open_close(vector<State>& o, vector<State>& c, State& child_state){
	//Get iterators to know child state position in both lists
	vector<State>::iterator itOpen = find(o.begin(), o.end(), child_state);
	vector<State>::iterator itClose = find(c.begin(), c.end(), child_state);

	//Checking if child state is not in OPEN nor CLOSE
	if((itOpen == o.end()) && (itClose == c.end())){
		o.push_back(child_state);
		sort(o.begin(), o.end());
	//If child state is in OPEN, check if we can replace the existing one with the new node with lower f(n)	
	} else if((itOpen != o.end()) && (o.at(distance(o.begin(), itOpen)).f > child_state.f)){
		o.erase(itOpen);
		o.push_back(child_state);
		sort(o.begin(), o.end());
	}
}

//Hoping for current state to be our goal
bool isGoal(State& s, tuple<int, int> g){
	bool goal = false;
	if(s.state_keys.empty() && (get<0>(g) == s.al_x_pos) && (get<1>(g) == s.al_y_pos)){
		goal = true;
	}
	return goal;
}

//If we are going to try to move Al, we need to create a new state and give it some data
State move_Al(State& s, int x, int y){
	State n = State();

	//Al new position
	n.al_x_pos = x;
	n.al_y_pos = y;

	//Father position
	n.father_x = s.al_x_pos;
	n.father_y = s.al_y_pos;
	//Update g path current value
	n.g = s.g;

	n.state_keys = s.state_keys;
	n.state_rocks = s.state_rocks;

	return n;
}

//Updating state keys
void pick_key(State& s){
	vector<tuple<int, int>>::iterator it = find(s.state_keys.begin(), s.state_keys.end(), make_tuple(s.al_x_pos, s.al_y_pos));
	s.state_keys.erase(it);
}

/*
When we detect a rock and prove that we can move it with help of CHECK_POSITION function, we compare Al current
and earlier position to know where the rock must be moved.
*/
void move_rock(State& s){
	//Find the rock in its own collection
	vector<tuple<int, int>>::iterator it = find(s.state_rocks.begin(), s.state_rocks.end(), make_tuple(s.al_x_pos, s.al_y_pos));
	tuple<int, int> new_rock;
	s.state_rocks.erase(it);

	//Finding new rock position based on Al current and immeadiately earlier position
	if((s.father_x < s.al_x_pos) && (s.father_y == s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos + 1, s.al_y_pos);
	} else if((s.father_x > s.al_x_pos) && (s.father_y == s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos - 1, s.al_y_pos);
	} else if((s.father_x == s.al_x_pos) && (s.father_y < s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos, s.al_y_pos + 1);
	} else if((s.father_x == s.al_x_pos) && (s.father_y > s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos, s.al_y_pos - 1);
	}else if((s.father_x < s.al_x_pos) && (s.father_y < s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos + 1, s.al_y_pos + 1);
	}else if((s.father_x < s.al_x_pos) && (s.father_y > s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos + 1, s.al_y_pos - 1);
	}else if((s.father_x > s.al_x_pos) && (s.father_y < s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos - 1, s.al_y_pos + 1);
	}else if((s.father_x > s.al_x_pos) && (s.father_y > s.al_y_pos)){
		new_rock = make_tuple(s.al_x_pos - 1, s.al_y_pos - 1);
	}

	s.state_rocks.push_back(new_rock);
}

/*
With this function, we check if the 4/8 different movements Al can do on each iteration are allowed based
on the different objects he can find on his way. Using a character to identify them helps us to make decisions
outside the function.
*/
char check_position(State& s, int x, int y, vector<tuple<int, int>> w, vector<tuple<int, int>> sn, tuple<int, int> goal,
					int rows, int cols){
	char current_object = ' ';

		if((find(w.begin(), w.end(), make_tuple(x,y)) != w.end()) ||
			(find(sn.begin(), sn.end(), make_tuple(x,y)) != sn.end()) ||
			(x < 0) || (y < 0) || (x >= rows) || (y >= cols)){
			//Can't move with those kind of objects or if the object is a rock and pushing it will be out of bounds
			current_object = 'F';
		} else if((find(s.state_rocks.begin(), s.state_rocks.end(), make_tuple(x,y)) != s.state_rocks.end())){
			//Rock identified
			current_object = 'R';
		} else if((find(s.state_keys.begin(), s.state_keys.end(), make_tuple(x,y)) != s.state_keys.end())){
			//Key identified
			current_object = 'K';
		} else if((get<0>(goal) == x) && (get<1>(goal) == y)){
			//Goal identified
			current_object = 'E';
		}

	return current_object;
}

/*Basic but highly informed heuristic based on Manhattan distance between Al position,
all remaining keys and exit position.
*/
int heuristic_1(State& s, vector<tuple<int, int>> k, tuple<int, int> e){
	int key_distance = 0;
	int exit_distance = 0;

	//Getting Manhattan distance of all remaining keys
	for(auto& key : k){
		key_distance += abs(s.al_x_pos - get<0>(key)) + abs(s.al_y_pos - get<1>(key));
	}

	//Getting Manhattan distance of exit position
	exit_distance = abs(s.al_x_pos - get<0>(e)) + abs(s.al_y_pos - get<1>(e));

	return key_distance + exit_distance;
}

/*
Variation of the first heuristic function. Now, we use the euclidean distance to all the
elements Al needs to end the map succesfully.
*/
double heuristic_2(State& s, vector<tuple<int, int>> k, tuple<int, int> e){
	double first_key = 0;
	double exit_distance = 0;

	if(!k.empty()){
		//Getting all keys euclidean distances
		for(auto& key : k){
			first_key += sqrt(pow(s.al_x_pos - get<0>(k[0]), 2) + pow(s.al_y_pos - get<1>(k[0]), 2));
		}
	}
	//Getting exit euclidean distance
	exit_distance = sqrt(pow(s.al_x_pos - get<0>(e), 2) + pow(s.al_y_pos - get<1>(e), 2));

	return first_key + exit_distance;
}

/*
Other heuristic variation. This time, we use the minimal function to try yo move Al, always, to the
nearest key.
*/
double heuristic_3(State& s, vector<tuple<int, int>> k, tuple<int, int> e){
	double first_key = 0;
	double second_key = 0;
	double exit_distance = 0;

	if(!k.empty()){
		//Initial key distance
		first_key = sqrt(pow(s.al_x_pos - get<0>(k[0]), 2) + pow(s.al_y_pos - get<1>(k[0]), 2));
		//Comparing keys by pairs to finally get the minimal euclidean distance from current Al position
		for(auto& key : k){
			second_key = sqrt(pow(s.al_x_pos - get<0>(key), 2) + pow(s.al_y_pos - get<1>(key), 2));
			first_key = min(first_key, second_key);
		}		
	}
	//Getting standard euclidean distance to exit
	exit_distance = sqrt(pow(s.al_x_pos - get<0>(e), 2) + pow(s.al_y_pos - get<1>(e), 2));

	return first_key + exit_distance;
}

/*
In every movement Al does, he needs to make sure there's no snakes in the same row/column 
if he wants to survive. To achieve that, we iterate through the snakes and rocks 
collection to compare positions with Al's one.
*/
bool care_with_snakes(State& s, vector<tuple<int, int>> snakes, vector<tuple<int, int>> walls){
	bool safe = false;
	int snake_col = -1;

	if(snakes.empty()){
		safe = true;
	} else {
		for(auto& sn : snakes){
			if(s.al_x_pos == get<0>(sn)) snake_col = get<1>(sn);
		}

		if(snake_col != -1){
			for(auto& key : s.state_keys){
				if(s.al_x_pos == get<0>(key)){
					if(((s.al_y_pos < get<1>(key)) && (get<1>(key) < snake_col)) ||
						((snake_col < get<1>(key)) && (get<1>(key) < s.al_y_pos))){
						safe = true;
					}
				}
			}
			for(auto& rock : s.state_rocks){
				if(s.al_x_pos == get<0>(rock)){
					if(((s.al_y_pos < get<1>(rock)) && (get<1>(rock) < snake_col)) ||
						((snake_col < get<1>(rock)) && (get<1>(rock) < s.al_y_pos))){
						safe = true;
					}
				}
			}
			//
			for(auto& wall : walls){
				if(s.al_x_pos == get<0>(wall)){
					if(((s.al_y_pos < get<1>(wall)) && (get<1>(wall) < snake_col)) ||
						((snake_col < get<1>(wall)) && (get<1>(wall) < s.al_y_pos))){
						safe = true;
					}
				}
			}			
		} else {
			safe = true;
		}
	}

	return safe;
}

/*
With this function we are able to do lots of things with help of other functions. We check if all the new state
movement conditions are correct. At the same time, we can know which object Al can have on his way. Always checking
for Al's snake safeness.
*/
void apply_movement(vector<State>& open, vector<State>& close, State& state, vector<tuple<int, int>>& walls,
					vector<tuple<int, int>>& snakes, tuple<int, int> goal, int heuristic_function,
					int rows, int cols, int x, int y, int& global_id){
	char c;
	State child_state;
	
	//Is there any object that makes my movement not possible?
	c = check_position(state, state.al_x_pos + x, state.al_y_pos + y, walls, snakes, goal, rows, cols);
	//If my movement is possible, lets see if we have some kind of object in front of us or nothing
	if((c != 'F') && care_with_snakes(state, snakes, walls)){
		child_state = move_Al(state, state.al_x_pos + x, state.al_y_pos + y);
		//Rock in front of us
		if(c == 'R'){
			//Can we push the rock?
			if((check_position(child_state, child_state.al_x_pos + x, child_state.al_y_pos + y, walls, snakes, goal, rows, cols) == ' ') &&
				care_with_snakes(child_state, snakes, walls)){
				//Move the rock
				move_rock(child_state);
				//Moving a rock costs 4 units
				child_state.g += 4;
				if(heuristic_function == 1){
					child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
				} else if(heuristic_function == 2) {
					child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
				} else {
					child_state.h = heuristic_3(child_state, child_state.state_keys, goal);
				}
				//Helps us to know which way we traveled for
				child_state.father_id = state.id;
				global_id++;
				child_state.id = global_id;
				child_state.f = child_state.g + child_state.h;
				//Maybe we need to update our lists...
				compare_open_close(open, close, child_state);
			}
		//Key in front of us	
		} else if((c == 'K') && care_with_snakes(child_state, snakes, walls)){
			//Pick key
			pick_key(child_state);
			//Pick a key costs 2 units
			child_state.g += 2;
			if(heuristic_function == 1){
				child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
			} else if(heuristic_function == 2) {
				child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
			} else {
				child_state.h = heuristic_3(child_state, child_state.state_keys, goal);
			}
			//Helps us to know which way we traveled for
			child_state.father_id = state.id;
			global_id++;
			child_state.id = global_id;
			child_state.f = child_state.g + child_state.h;
			//Maybe we need to update our lists...
			compare_open_close(open, close, child_state);
		//Clear movement, but... Safe? Still need to check it	
		} else if(care_with_snakes(child_state, snakes, walls)){
			child_state.g += 2;
			if(heuristic_function == 1){
				child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
			} else if(heuristic_function == 2) {
				child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
			} else {
				child_state.h = heuristic_3(child_state, child_state.state_keys, goal);
			}
			//Helps us to know which way we traveled for
			child_state.father_id = state.id;
			global_id++;
			child_state.id = global_id;
			child_state.f = child_state.g + child_state.h;
			//Maybe we need to update our lists...
			compare_open_close(open, close, child_state);
		}
	}
}

int main(int argc, char *argv[]) {
	string file_path = argv[1];
	int heuristic_function = atoi(argv[2]);
	State state = State();
	vector<State> open;
	vector<State> close;
	vector<tuple<int, int>> walls;
	vector<tuple<int, int>> snakes;
	tuple<int, int> goal;
	bool completed = false;
	int rows = 0;
	int cols = 0;
	int global_id;

	if(argc != 3){
		cout << "You need to supply two arguments to this program, file and heuristic. (1-3)" << endl;
	}

	//Parsing the map, etc... To start working
	set_Al_initial_data(state, file_path, goal, walls, snakes, rows, cols);
  	state.id = global_id;
  	//Stop point to later discover the path we have been through
	state.father_id = -1;
	open.push_back(state);

	while(!open.empty() && !completed){
		//Pick first node in OPEN
		state = open.at(0);
		//Erase first node in OPEN
		open.erase(open.begin());

		if(isGoal(state, goal)){
			completed = true;
		} else {
			//Put expanded node in CLOSE
			close.push_back(state);

			//Move right
			if(state.al_x_pos + 1 < rows){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, 1, 0, global_id);
			}
			//Move left
			if(state.al_x_pos - 1 >= 0){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, -1, 0, global_id);
			}
			//Move up
			if(state.al_y_pos + 1 < cols){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, 0, 1, global_id);
			}
			//Move down
			if(state.al_y_pos - 1 >= 0){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, 0, -1, global_id);
			}
			//Move up-right
			if((state.al_x_pos + 1 < rows) && (state.al_y_pos + 1 < cols)){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, 1, 1, global_id);
			}
			//Move right-down
			if((state.al_x_pos + 1 < rows) && (state.al_y_pos - 1 >= 0)){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, 1, -1, global_id);
			}
			//Move left-up
			if((state.al_x_pos - 1 >= 0) && (state.al_y_pos + 1 < cols)){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, -1, 1, global_id);
			}
			//Move left-down
			if((state.al_x_pos - 1 >= 0) && (state.al_y_pos - 1 >= 0)){
				apply_movement(open, close, state, walls, snakes, goal, heuristic_function,
					 rows, cols, -1, -1, global_id);
			}

		}
	}


	State father;
	int count = 0;
	int path_length = 0;
	vector<State> path;

	//If we found solution and the goal was reached
	if(completed && isGoal(state, goal)){
		cout << "\nSolution found!\n" << endl;

		//Generating statistics
  		ofstream out;
  		out.open(file_path + ".statistics");
		out << "Expanded nodes: " << close.size() << endl;
		out << "Total cost: " << state.g << endl;
		out << "Time taken: " << ((double)(clock() - tStart)/CLOCKS_PER_SEC) << " seconds" << endl;
		//Finding with father_id the reverse path to beginning
		while((state.father_id != -1) && (count < close.size())){
   		father = close.at(count);
			if(state.father_id == father.id){
				path.push_back(state);
				state = father;
				count = 0;
				path_length++;				
			}else{
				count++;
			}
		}
		out << "Path length: " << path_length << endl;
		out.close();

		//Generating output.txt
		ofstream out1;
		out1.open(file_path + ".output");
		out1 << "Path: " << endl;
		out1 << "(" << state.al_x_pos << ", " << state.al_y_pos << ")->";
		//Printing in correct order the path Al has gone through
		for(auto it = path.rbegin(); it != path.rend(); ++it){
			state = *it;
			out1 << "(" << state.al_x_pos << ", " << state.al_y_pos << ")->";
		}
		out1 << "END!" << endl;
		out1.close();
	} else {
		cout << "Solution not found!" << endl;
	}
}