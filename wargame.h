#ifndef classes
#define classes

#include <climits>
#include <string>
#include <iostream>

#include "unilist.h"
#include "crates.h"

class player;
class map;
class division;




class player
{
private:
	friend map;
	friend division;

	class err
	{
	private:
		std::string reason;
	public:
		err(std::string what = "something went wrong") : reason(what) {};
		~err() {};
		std::string what() { return reason; }
	};

	class income
	{
	private:
		int summ;
		std::string reason;
	public:
		income(std::string what, int amt) : reason(what), summ(amt) {}
		~income() {};
		int msg() { std::cout << " Earned " << summ << " coins! (" << reason << ")" << std::endl; return summ; }
	};

	std::string name;
	Unilist<division*> army;
	division* chosen;

	map* location;

	player** enemies;
	int enemies_amt;
	int side;
	bool ready;
	bool defeat;
	int wins;

	int coins;

	int get_armysize() { return army.get_size(); }

public:
	player(int number);
	~player();

	void equipment();
	void placement();
	std::string& show_name() { return name; }
	division* search_by_pos(int& srch_x, int& srch_y);
	void set_location(map* addr) { location = addr; }
	void set_side(int value) { side = value; }
	void set_EL(player** pool, int poolsize);
	void choose_action(int& step);
	void check_other(division* checking);
	bool is_defeat();
	bool check_ready() { return ready; }
	void won() { wins++; };
	int get_wins() { return wins; }
	void refresh();
	void search_alive();

	void crate_unboxing(crate* found);
};


class division
{
private:
	friend map;
	friend player;
	friend Unilist<division*>;

	struct position
	{
		int x;
		int y;
	};
	bool side;
	position pos;

	std::string type;
	bool defeated;
	position move_limits[2];
	position kill_zone[2];
	Unilist<division*> enemy_list;

	double max_health;
	double health;
	double aver_dmg;
	double crit_chance;
	double precise;
	int attack_range;
	int move_range;

	int price;

	void place_division(bool which_side, map& loc);
	void reach_area_comp(map& pg);
	int move(map& pg, int destX, int destY);
	void killzone_comp(map& pg);
	void refresh_EL();
	void update_EL(player** enemies, int enemies_amt);
	void attack(int victim_number, map& loc);
	void short_info();

public:
	division();
	~division() {}

	friend std::ostream& operator<<(std::ostream& out, division& div);
	friend void calc_zone(division::position& current, division::position& lcorner, division::position& rcorner, map& loc, int range);
	friend bool get_coord(position& inp, map& loc);
};



class map
{
private:
	friend player;
	friend division;

	enum { minlength = 10, minwidth = 10 };
	int length;
	int width;
	int playernum;
	char** playground;

	Unilist<crate*> crates;
	int crate_spawn_timer;

	void set_width(int w_init, int playernum = 2);
	void set_length(int l_init, int playernum = 2);

public:
	map(int w_init = minwidth, int l_init = minlength, int p_amt = 2);
	~map();
	void settings();
	void init();
	int get_playernum() { return playernum; }
	void save_div(division* div);
	void update(const player& contestant);
	void draw();
	void refresh();
	void crate_spawn_event();
	crate* find_crate(int srch_x, int srch_y);

	friend void calc_zone(division::position& current, division::position& lcorner, division::position& rcorner, map& loc, int range);
	friend bool get_coord(division::position& inp, map& loc);
};

int get_with_lim(int bottom = INT_MIN, int top = INT_MAX);
int get_int();
void check_limits(int input, int bottom = INT_MIN, int top = INT_MAX);


#endif