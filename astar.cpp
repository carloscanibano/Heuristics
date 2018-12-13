#include "State.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
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
	bool found = false;
	vector<State>::iterator it;
	State state;
	int count = 0;
	if(c.empty()){
		state = o.at(0);
		c.push_back(state);
		o.pop_back();
	} else {
		for(int i = 0; (i < o.size()) && (!found); i++){
			state = o.at(i);
			for(int j = 0; (j < c.size()) && (!found); j++){
				if(state == c.at(j)){
					//cout << "ESTADO ELEGIDO: (" << state.al_x_pos << ", " << state.al_y_pos << ")" << endl;
					c.push_back(state);
					o.erase(o.begin() + i);
					found = true;
				} else {
					//cout << "DENTRO DE CERRADA" << endl;
				}
			}
		}
	}
	return state;
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

void pick_key(State& s){
	cout << "LLAVE ENCONTRADA" << endl;
	vector<tuple<int, int>>::iterator it = find(s.state_keys.begin(), s.state_keys.end(), make_tuple(s.al_x_pos, s.al_y_pos));
	s.state_keys.erase(it);
	s.allow_father = true;
}

void move_rock(State& s){
	vector<tuple<int, int>>::iterator it = find(s.state_rocks.begin(), s.state_rocks.end(), make_tuple(s.al_x_pos, s.al_y_pos));
	tuple<int, int> new_rock;
	s.state_rocks.erase(it);

	if(s.father_x < s.al_x_pos){
		new_rock = make_tuple(s.al_x_pos + 1, s.al_y_pos);
	} else if(s.father_x > s.al_x_pos){
		new_rock = make_tuple(s.al_x_pos - 1, s.al_y_pos);
	} else if(s.father_y < s.al_y_pos){
		new_rock = make_tuple(s.al_x_pos, s.al_y_pos + 1);
	} else if(s.father_y > s.al_y_pos){
		new_rock = make_tuple(s.al_x_pos, s.al_y_pos - 1);
	}

	s.state_rocks.push_back(new_rock);
	s.allow_father = true;
}

char check_position(State& s, int x, int y, vector<tuple<int, int>> w, vector<tuple<int, int>> sn){
	char current_object = ' ';
	//cout << "Padre id:" << s.father_id << '\n';
		if((find(w.begin(), w.end(), make_tuple(x,y)) != w.end()) ||
			(find(sn.begin(), sn.end(), make_tuple(x,y)) != sn.end()) ||
			((x == s.father_x) && (y == s.father_y) && (!s.allow_father))){
			//Can't move with those kind of objects
			current_object = 'F';
			//cout << "BLOQUEADO" << endl;
		} else if((find(s.state_rocks.begin(), s.state_rocks.end(), make_tuple(x,y)) != s.state_rocks.end())){
			//Rock identified
			current_object = 'R';
		} else if((find(s.state_keys.begin(), s.state_keys.end(), make_tuple(x,y)) != s.state_keys.end())){
			//Key identified
			current_object = 'K';
		}

	return current_object;
}

int heuristic_1(State& s, vector<tuple<int, int>> k, tuple<int, int> e){
	int key_distance = 0;
	int exit_distance = 0;

	for(auto& key : k){
		key_distance += abs(s.al_x_pos - get<0>(key)) + abs(s.al_y_pos - get<1>(key));
	}

	exit_distance = abs(s.al_x_pos - get<0>(e)) + abs(s.al_y_pos - get<1>(e));

	return key_distance + exit_distance;
}

int heuristic_2(State& s, vector<tuple<int, int>> k, tuple<int, int> e){
	//MIN BETWEEN ALL KEYS (EUCLIDEAN) + EXIT EUCLIDEAN DISTANCE
	int first_key = 0;
	int second_key = 0;
	int exit_distance = 0;

	first_key = sqrt(pow(s.al_x_pos - get<0>(k[0]), 2) + pow(s.al_y_pos - get<1>(k[0]), 2));

	for(auto& key : k){
		second_key += sqrt(pow(s.al_x_pos - get<0>(key), 2) + pow(s.al_y_pos - get<1>(key), 2));
		first_key = min(first_key, second_key);
	}

	exit_distance = sqrt(pow(s.al_x_pos - get<0>(e), 2) + pow(s.al_y_pos - get<1>(e), 2));

	return first_key + exit_distance;
}

bool care_with_snakes(State& s, vector<tuple<int, int>> snakes){
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
		}
	}

	return safe;
}

bool compare_objects(vector<tuple<int, int>> obj1, vector<tuple<int, int>> obj2){
	bool equal = true;
	if(obj1.size() == obj2.size()){
		tuple<int, int> t1;
		tuple<int, int> t2;
		for(int i = 0; (i < obj1.size()) && (equal); i++){
			t1 = obj1.at(i);
			for(int j = 0; (j < obj2.size()) && (equal); j++){
				t2 = obj2.at(j);
				if((get<0>(t1) != get<0>(t2)) || (get<1>(t2) != get<1>(t2))){
					equal = false;
				}
			}
		}
	} else {
		equal = false;
	}
	return equal;
}

int main(int argc, char *argv[]) {
	string file_path = argv[1];
	int heuristic_function = atoi(argv[2]);
	State state = State();
	State child_state;
	vector<State> open;
	vector<State> close;
	vector<tuple<int, int>> walls;
	vector<tuple<int, int>> snakes;
	tuple<int, int> goal;
	bool completed = false;
	int rows = 0;
	int cols = 0;
	int global_id;
	char c;


	set_Al_initial_data(state, file_path, goal, walls, snakes, rows, cols);
  state.id = global_id;
	state.father_id = -1;
	open.push_back(state);

	while(!open.empty() && !completed){
		cout << "Open: ";
		for(State& s : open){
			cout << "(" << s.al_x_pos << ", " << s.al_y_pos << ")" << endl;
		}
		state = compare_open_close(open, close);

		cout << "Close: ";
		for(State& s : close){
			cout << "(" << s.al_x_pos << ", " << s.al_y_pos << ")" << endl;
		}
		if(isGoal(state, goal)){
			completed = true;
		} else {
			cout << "Current node: ";
			cout << state.al_x_pos << ", " << state.al_y_pos << endl;
			//MAP BOUNDS CHECKS DONE
			//COMPROBAR CASILLA, CHECK DE ROCA Y VOLVER A LLAMAR COMPROBAR CASILLA CON LA SIGUIENTE
			//LLAMAR A HEURISTICAS, ACTUALIZAR H Y F.
			//ORDENAR LOS ESTADOS DE LA LISTA ABIERTA

			if(state.al_x_pos + 1 < rows){
				c = check_position(state, state.al_x_pos + 1, state.al_y_pos, walls, snakes);
				if((c != 'F') && care_with_snakes(state, snakes)){
					child_state = move_Al(state, state.al_x_pos + 1, state.al_y_pos);
					if(c == 'R'){
						if(check_position(child_state, child_state.al_x_pos + 1, child_state.al_y_pos, walls, snakes) &&
							care_with_snakes(child_state, snakes)){

							move_rock(child_state);
							child_state.g += 4;
							if(heuristic_function == 1){
								child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
							} else {
								child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
							}
							child_state.father_id = state.id;
							global_id++;
							child_state.id = global_id;
							child_state.f = child_state.g + child_state.h;
							open.push_back(child_state);
							sort(open.begin(), open.end());
							cout << "x + 1" << endl;
							cout << "f(n): " << child_state.f << endl;
							cout << "h(n): " << child_state.h << endl;
						}
					} else if((c == 'K') && care_with_snakes(child_state, snakes)){
						pick_key(child_state);
						child_state.g += 2;
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "x + 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					} else if(care_with_snakes(child_state, snakes)){
						if((child_state.al_x_pos == get<0>(goal)) && (child_state.al_y_pos == get<1>(goal))){
							child_state.g += 2;
						} else {
							child_state.g += 2;
						}
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "x + 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					}
				}
			}
			if(state.al_x_pos - 1 >= 0){
				c = check_position(state, state.al_x_pos - 1, state.al_y_pos, walls, snakes);
				if((c != 'F') && care_with_snakes(state, snakes)){
					child_state = move_Al(state, state.al_x_pos - 1, state.al_y_pos);
					if(c == 'R'){
						if(check_position(child_state, child_state.al_x_pos - 1, child_state.al_y_pos, walls, snakes) &&
							care_with_snakes(child_state, snakes)){

							move_rock(child_state);
							child_state.g += 4;
							if(heuristic_function == 1){
								child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
							} else {
								child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
							}
							child_state.father_id = state.id;
							global_id++;
							child_state.id = global_id;
							child_state.f = child_state.g + child_state.h;
							open.push_back(child_state);
							sort(open.begin(), open.end());
							cout << "x - 1" << endl;
							cout << "f(n): " << child_state.f << endl;
							cout << "h(n): " << child_state.h << endl;
						}
					} else if((c == 'K') && care_with_snakes(child_state, snakes)){
						pick_key(child_state);
						child_state.g += 2;
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "x - 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					} else if(care_with_snakes(child_state, snakes)){
						if((child_state.al_x_pos == get<0>(goal)) && (child_state.al_y_pos == get<1>(goal))){
							child_state.g += 2;
						} else {
							child_state.g += 2;
						}
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "x - 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					}
				}
			}
			if(state.al_y_pos + 1 < cols){
				c = check_position(state, state.al_x_pos, state.al_y_pos + 1, walls, snakes);
				if((c != 'F') && care_with_snakes(state, snakes)){
					child_state = move_Al(state, state.al_x_pos, state.al_y_pos + 1);
					if(c == 'R'){
						if(check_position(child_state, child_state.al_x_pos, child_state.al_y_pos + 1, walls, snakes) &&
							care_with_snakes(child_state, snakes)){

							move_rock(child_state);
							child_state.g += 4;
							if(heuristic_function == 1){
								child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
							} else {
								child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
							}
							child_state.father_id = state.id;
							global_id++;
							child_state.id = global_id;
							child_state.f = child_state.g + child_state.h;
							open.push_back(child_state);
							sort(open.begin(), open.end());
							cout << "y + 1" << endl;
							cout << "f(n): " << child_state.f << endl;
							cout << "h(n): " << child_state.h << endl;
						}
					} else if((c == 'K') && care_with_snakes(child_state, snakes)){
						pick_key(child_state);
						child_state.g += 2;
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "y + 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					} else if(care_with_snakes(child_state, snakes)){
						if((child_state.al_x_pos == get<0>(goal)) && (child_state.al_y_pos == get<1>(goal))){
							child_state.g += 2;
						} else {
							child_state.g += 2;
						}
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "y + 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					}
				}
			}
			if(state.al_y_pos - 1 >= 0){
				c = check_position(state, state.al_x_pos, state.al_y_pos - 1, walls, snakes);
				if((c != 'F') && care_with_snakes(state, snakes)){
					child_state = move_Al(state, state.al_x_pos, state.al_y_pos - 1);
					if(c == 'R'){
						if(check_position(child_state, child_state.al_x_pos, child_state.al_y_pos - 1, walls, snakes) &&
							care_with_snakes(child_state, snakes)){

							move_rock(child_state);
							child_state.g += 4;
							if(heuristic_function == 1){
								child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
							} else {
								child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
							}
							child_state.father_id = state.id;
							global_id++;
							child_state.id = global_id;
							child_state.f = child_state.g + child_state.h;
							open.push_back(child_state);
							sort(open.begin(), open.end());
							cout << "y - 1" << endl;
							cout << "f(n): " << child_state.f << endl;
							cout << "h(n): " << child_state.h << endl;
						}
					} else if((c == 'K') && care_with_snakes(child_state, snakes)){
						pick_key(child_state);
						child_state.g += 2;
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "y - 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					} else if(care_with_snakes(child_state, snakes)){
						if((child_state.al_x_pos == get<0>(goal)) && (child_state.al_y_pos == get<1>(goal))){
							child_state.g += 2;
						} else {
							child_state.g += 2;
						}
						if(heuristic_function == 1){
							child_state.h = heuristic_1(child_state, child_state.state_keys, goal);
						} else {
							child_state.h = heuristic_2(child_state, child_state.state_keys, goal);
						}
						child_state.father_id = state.id;
						global_id++;
						child_state.id = global_id;
						child_state.f = child_state.g + child_state.h;
						open.push_back(child_state);
						sort(open.begin(), open.end());
						cout << "y - 1" << endl;
						cout << "f(n): " << child_state.f << endl;
						cout << "h(n): " << child_state.h << endl;
					}
				}
			}
		}
		cout << endl;
	}


	State father;
	int count = 0;
	//father = close.at(0);
	//cout << " id:" << father.father_id << '\n';
	if(completed){
		//cout << "Padre id :" << state.father_id << '\n';
		cout << "Solution found!" << endl;
		cout << "Total cost: " << state.g << endl;
		cout << "Path: " << endl;
		while((state.father_id != -1) && (count < close.size())){
			//cout << "Padre id :" << state.father_id << '\n';
   		father = close.at(count);
			//cout << "Padre id :" << state.father_id << '\n';
			//cout << " id:" << father.id << '\n';
			//cout << "Padre id :" << state.father_id << '\n';
			if(state.father_id == father.id){
				//cout << "Padre" << endl;
				cout << "(" << state.al_x_pos << ", " << state.al_y_pos << ")" << endl;
				state = father;
				count = 0;
			}else{
				count++;
				//cout << count << endl;
			}
		}
		cout << "(" << state.al_x_pos << ", " << state.al_y_pos << ")" << endl;
	} else {
		cout << "Solution not found!" << endl;
	}

	/*PRINT TUPLE
    cout << "(" << std::get<0>(keys[0]) << ", " << std::get<1>(keys[0])
              <<")\n";
              */
}
