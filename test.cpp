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

int main(int argc, char *argv[]) {
	State s1 = State();
	State s2 = State();
	State s3 = State();
	State s4 = State();

	s1.al_x_pos = 1;
	s1.al_x_pos = 3;
	s1.f = 34;
	s2.al_x_pos = 1;
	s2.al_x_pos = 3;
	s2.f = 13;
	s3.f = 24;
	s4.f = 2;

	vector<State> states;
	states.push_back(s2);
	states.push_back(s1);
	states.push_back(s3);
	//states.push_back(s4);

	vector<tuple<int, int>> vec;
	tuple<int, int> t1 = make_tuple(2, 3);
	tuple<int, int> t2 = make_tuple(3, 4);
	tuple<int, int> t3 = make_tuple(5, 6);

	vec.push_back(t1);
	vec.push_back(t2);
	vec.push_back(t3);

	if(find(vec.begin(), vec.end(), make_tuple(3, 4)) != vec.end()){
		vec.erase(vec.begin() + 1);
	}

	for(auto& t : vec) {
		cout << get<0>(t) << ", " << get<1>(t) << endl;
	}

	/*
	if(find(states.begin(), states.end(), s4) != states.end()){
		sort(states.begin(), states.end());
	}

	for(State& s: states){
		cout << s.f << endl;
	}
	*/
}