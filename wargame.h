#ifndef classes
#define classes

#include <climits>
#include <string>
#include <iostream>

class player;
class map;
class division;
class list;

class list
{
private:
	friend player;
	friend division;
	struct node
	{
		division* content;
		node* next;
	};
	node* head;
	node* tail;
	node* current;
	int size;

public:
	list();
	list(list& orig);
	~list();
	void add_elem(division* which);
	void rm_elem(division* which);
	bool is_empty() { if (!size) return true; }

	list& operator=(const list& orig);
	division* operator[](int index);
	friend std::ostream& operator<<(std::ostream& out, list& obj);
};



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

	int ord_num;
	std::string name;
	list army;
	division* chosen;

	map* location;

	player** enemies;
	int enemies_amt;
	bool defeat;

	int coins;

	int get_armysize() { return army.size; }

public:
	player(int number);
	~player();

	void equipment();
	void placement();
	std::string& show_name() { return name; }
	division* search_by_pos(int& srch_x, int& srch_y);
	void set_location(map* addr) { location = addr; }
	void set_EL(player** pool, int poolsize);
	void choose_action(int& step);
	void check_other(division* checking);
	bool is_defeat();
	void search_alive();
};


class division
{
private:
	friend map;
	friend player;

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
	list enemy_list;

	double health;
	double aver_dmg;
	double crit_chance;
	double precise;
	int attack_range;
	int move_range;

	int price;

	void place_division(bool which_side, map& loc);
	void reach_area_comp(map& pg);
	void move(map& pg, int destX, int destY);
	void killzone_comp(map& pg);
	void refresh_EL();
	void update_EL(player** enemies, int enemies_amt);
	void attack(int victim_number, map& loc);
	void short_info();

public:
	division();
	~division() {}

	friend std::ostream& operator<<(std::ostream& out, list& obj);
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

	friend void calc_zone(division::position& current, division::position& lcorner, division::position& rcorner, map& loc, int range);
	friend bool get_coord(division::position& inp, map& loc);
};

int get_with_lim(int bottom = INT_MIN, int top = INT_MAX);
int get_int();
void check_limits(int input, int bottom = INT_MIN, int top = INT_MAX);


#endif