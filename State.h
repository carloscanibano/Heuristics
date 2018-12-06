#ifndef State_h
#define State_h
#include <vector>
#include <tuple>

using namespace std;

struct State{
public:
	//Al x-axis position
	int al_x_pos;
	//Al y-axis position
	int al_y_pos;
	//Father node x-axis position
	int father_x_pos;
	//Father node y-axis position
	int father_y_pos;
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
	//Default constructor
	State();
};

State::State(){
	al_x_pos = 0;
	al_y_pos = 0;
	father_x_pos = 0;
	father_y_pos = 0;
	f = 0;
	g = 0;
	h = 0;
}
#endif