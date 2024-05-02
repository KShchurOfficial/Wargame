#include "wargame.h"
#include <iostream>
#include <stdexcept>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

// Class "Player"


player::player(int number)
{
	using std::cout;
	using std::cin;
	using std::endl;

	cout << " Enter your name (enter to skip): ";
	std::getline(cin, name, '\n');
	if (name == "")
		name = "Player " + std::to_string(number);
	ord_num = number;

	army = new division * [armysize];
	for (int i = 0; i < armysize; i++)
	{
		army[i] = nullptr;
	}

	chosen = nullptr;
	defeat = false;
	dialog = 0;
	location = nullptr;
	enemies = nullptr;
	enemies_amt = 0;
}

player::~player()
{
	for (int i = 0; i < armysize; i++)
	{
		delete army[i];
	}
	delete[] army;
	delete[] enemies;
}

void player::placement(map& visual)
{
	using std::cout;
	using std::cin;
	using std::cerr;
	using std::endl;

	bool ready = false;
	int input = 0;

	while (!ready)
	{
		system("cls");
		cout << "\"" << name << "\" turn." << endl << endl;
		visual.draw();

		ready = true;
		for (int i = 0; i < armysize; i++)
		{
			if (army[i] == nullptr)
			{
				ready = false;
				break;
			}
		}
		if (ready)
		{
			cout << " All divisions are placed!" << endl;
			cin.get();
			system("cls");
			break;
		}

		cout << " Need to place: " << endl;
		for (int i = 0; i < armysize; i++)
		{
			if (army[i] == nullptr)
				cout << " division #" << i + 1 << endl;
		}
		cout << " Enter the number of placing division: ";
		input = get_with_lim(1, armysize);
		input--;

		if (army[input] != nullptr)
		{
			cout << " This division was already placed!" << endl;
		}
		else
		{
			army[input] = new division(ord_num % 2);
			try
			{
				check_other(army[input]);
			}
			catch (err& trouble)
			{
				cerr << " Error: " << trouble.what() << endl;
				delete army[input];
				army[input] = nullptr;
				cout << " Try again." << endl;
				cin.get();
				continue;
			}

			visual.save_div(army[input]);
			cout << " Division placed!" << endl;
		}
		cin.get();
		system("cls");
	}
}

void player::check_other(division* checking)
{
	using std::cout;
	using std::endl;

	for (int i = 0; i < armysize; i++)
	{
		if (army[i] == checking || army[i] == nullptr)
		{
			continue;
		}
		else
		{
			if (army[i]->pos.x == checking->pos.x && army[i]->pos.y == checking->pos.y)
				throw(err("This place was already taken!"));
		}
	}

	return;
}

bool player::is_defeat()
{
	return defeat;
}

void player::search_alive()
{
	defeat = true;
	for (int i = 0; i < armysize; i++)
	{
		if (!army[i]->defeated)
		{
			defeat = false;
			break;
		}
	}
}

division* player::search_by_pos(int& srch_x, int& srch_y)
{
	bool found = false;
	for (int i = 0; i < armysize; i++)
	{
		chosen = army[i];
		if (chosen->pos.x == srch_x && chosen->pos.y == srch_y)
		{
			if (!chosen->defeated)
			{
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		chosen = nullptr;
		throw(err("Division with these coordinates is not found!"));
	}

	else
		return chosen;
}

void player::set_EL(player** pool, int poolsize)
{
	enemies_amt = poolsize - 1;
	enemies = new player * [enemies_amt];
	int acc = 0;
	for (int i = 0; i < poolsize; i++)
	{
		if (pool[i] == this)
			continue;

		enemies[acc] = pool[i];
		acc++;
	}

	return;
}

void player::choose_action(int& step)
{
	using std::cout;
	using std::cin;
	using std::endl;

	int dialog = 0;
	switch (step)
	{
	case 1:
	{
		cout << endl
			<< " 1) Choose some division" << endl
			<< " 2) give up" << endl
			<< " Your choice: ";
		dialog = get_with_lim(1, 2);

		switch (dialog)
		{
		case 1:
		{
			bool denial = false;
			division::position input;
			chosen = nullptr;
			while (!chosen)
			{
				if (!get_coord(input, *location))
				{
					denial = true;
					step--;
					break;
				}
				try
				{
					chosen = search_by_pos(input.x, input.y);
				}
				catch (err& trouble)
				{
					cout << " Error: " << trouble.what() << endl
						<< " Try again. ";
					cin.get();
				}
			}
			if (!denial)
			{
				chosen->reach_area_comp(*location);
				chosen->killzone_comp(*location);
				chosen->refresh_EL();
				chosen->update_EL(enemies, enemies_amt);
				cin.get();
			}

			break;
		}
		case 2:
		{
			cout << " Are you sure?" << endl
				<< " 0 - no" << endl
				<< " 1 - yes" << endl
				<< " Your input: ";
			defeat = get_with_lim(0, 1);
			break;
		}
		}

		return;
	}
	case 2:
	{
		chosen->short_info();
		cout << endl
			<< " 0) Choose other division" << endl
			<< " 1) Move division" << endl
			<< " 2) Attack enemy" << endl
			<< " Your choice: ";
		dialog = get_with_lim(0, 2);

		switch (dialog)
		{
		case 0:
		{
			step = 0;
			break;
		}
		case 1:
		{
			division::position inp;

			while (true)
			{
				if (!get_coord(inp, *location))
				{
					step--;
					break;
				}

				try
				{
					chosen->move(*location, inp.x, inp.y);
				}
				catch (err& place_not_free)
				{
					cout << " Error: " << place_not_free.what() << endl;
					cout << " Try again." << endl;
					std::cin.get();
					continue;
				}
				catch (std::range_error& trouble)
				{
					cout << " Error: " << trouble.what() << endl;
					cout << " Try again." << endl;
					std::cin.get();
					continue;
				}
				cout << " Division moved!" << endl;
				cin.get();

				break;
			}
			break;
		}
		case 2:
		{
			while (true)
			{
				cout << chosen->enemy_list << endl;
				if (!chosen->enemy_list.size)
				{
					cin.get();
					step--;
				}
				else
				{
					cout << " Your input: ";
					dialog = get_with_lim(1, chosen->enemy_list.size);
					dialog--;

					try
					{
						chosen->attack(dialog, *location);
					}
					catch (std::range_error& trouble)
					{
						cout << " Error: " << trouble.what() << endl;
						cout << " Try again." << endl;
						continue;
					}
				}

				break;
			}

			break;
		}
		}
	}
	}
}



// Class "division"



division::division(bool which_side)
{
	using std::cout;
	using std::cin;
	using std::endl;

	side = which_side;

	int place_lim = 1;
	if (side)
		place_lim = map::length - 1;

	cout << " Enter position X: ";
	pos.x = get_with_lim(1, map::width);
	cout << " Enter position Y: ";
	pos.y = get_with_lim(place_lim, place_lim + 1);

	pos.x--;
	pos.y--;

	type = "division";
	defeated = false;
	health = 100.0;
	aver_dmg = 20.0;
	crit_chance = 20.0;
	precise = 90.0;
	attack_range = 2;
	move_range = 3;
}

void division::reach_area_comp(map& pg)
{
	calc_zone(pos, move_limits[0], move_limits[1], pg, move_range);
	return;
}

void division::move(map& pg, int destX, int destY)
{
	if (pg.playground[destY][destX] != ' ')
	{
		throw(player::err("Cannot move division on this cell!"));
	}
	else if (destX < move_limits[0].x || destX > move_limits[1].x || destY < move_limits[0].y || destY > move_limits[1].y)
	{
		throw(std::range_error("Destination point is out of reach area!"));
	}
	else
	{
		pg.playground[pos.y][pos.x] = ' ';
		pos.x = destX;
		pos.y = destY;
		pg.playground[pos.y][pos.x] = 'D';
	}

	return;
}

void division::killzone_comp(map& pg)
{
	calc_zone(pos, kill_zone[0], kill_zone[1], pg, attack_range);
	return;
}

void division::refresh_EL()
{
	division* current = nullptr;
	for (int i = 0; i < enemy_list.size; i++)
	{
		current = &enemy_list[i];
		if (current->pos.x < kill_zone[0].x || current->pos.y < kill_zone[0].y ||
			current->pos.x > kill_zone[1].x || current->pos.y > kill_zone[1].y ||
			current->defeated)
		{
			enemy_list.rm_elem(current);
			i = -1;
		}
	}
	std::cin.get();
	return;
}

void division::update_EL(player** enemies, int enemies_amt)
{
	bool skip = false;
	division* current = nullptr;


	for (int i = 0; i < enemies_amt; i++)
	{
		for (int j = 0; j < enemies[i]->armysize; j++)
		{
			if (enemies[i]->army[j]->pos.x >= kill_zone[0].x && enemies[i]->army[j]->pos.y >= kill_zone[0].y &&
				enemies[i]->army[j]->pos.x <= kill_zone[1].x && enemies[i]->army[j]->pos.y <= kill_zone[1].y &&
				!enemies[i]->army[j]->defeated)
			{
				for (int k = 0; k < enemy_list.size; k++)
				{
					current = &enemy_list[k];
					if (current == enemies[i]->army[j])
					{
						skip = true;
						break;
					}
				}
				if (skip)
				{
					skip = false;
					continue;
				}

				enemy_list.add_elem(enemies[i]->army[j]);
			}
		}
	}

	return;
}

void division::attack(int victim_number, map& loc)
{
	using std::cout;
	using std::cin;
	using std::endl;
	int damage = aver_dmg * 0.9 + rand() % int(aver_dmg * 0.2 + 1);
	int _event = 1 + rand() % 100;
	if (_event <= precise)
		_event = 1 + rand() % 100;
	else
		damage = 0;

	division* target = nullptr;

	try
	{
		target = &enemy_list[victim_number];
	}
	catch (std::range_error trouble)
	{
		throw (std::range_error(trouble.what()));
	}

	if (_event <= crit_chance)
	{
		damage *= 1.5;
		cout << " Critical damage!" << endl;
	}
	else if (!damage)
		cout << " Miss!" << endl;
	else
		cout << " Enemy is hit!" << endl;

	target->health -= damage;
	cin.get();

	if (target->health <= 0)
	{
		target->health = 0;
		target->defeated = true;
		loc.playground[target->pos.y][target->pos.x] = ' ';
		target->pos.x = -1;  // Система управления дивизиями игрока должна быть переделана 
		target->pos.y = -1;  // на манер системы со списком целей для атаки.
		cout << " Enemy destroyed!" << endl;
		cin.get();
	}

	return;
}

void division::short_info()
{
	using std::cout;
	using std::endl;

	cout << type << ": (" << pos.x + 1 << ";"
		<< pos.y + 1 << ") (health: " << health << ")" << endl;

	return;
}

std::ostream& operator<<(std::ostream& out, division& div)
{
	using std::endl;

	out << " Division info: " << endl;
	out << " Average value of damage: " << div.aver_dmg << endl
		<< " Health: " << div.health << endl
		<< " Status: ";
	div.defeated ? out << "defeated" : out << "alive";
	out << endl;

	return out;
}



// Class "map"



map::map()
{
	playground = new char* [length];
	for (int y = 0; y < length; y++)
	{
		playground[y] = new char[width];
		for (int x = 0; x < width; x++)
			playground[y][x] = ' ';
	}
}

map::~map()
{
	for (int y = 0; y < length; y++)
		delete[] playground[y];
	delete[] playground;
}

void map::save_div(division* div)
{
	playground[div->pos.y][div->pos.x] = 'D';
	return;
}

void map::draw()
{
	using std::cout;
	using std::endl;

	cout << "   ";
	for (int x = 0; x < width; x++)
	{
		if ((x + 1) < 100)
			cout << " ";
		cout << x + 1;
		if ((x + 1) < 10)
			cout << " ";
	}
	cout << endl;

	for (int y = 0; y < length; y++)
	{
		if ((y + 1) < 100)
			cout << " ";
		cout << y + 1;
		if ((y + 1) < 10)
			cout << " ";

		for (int x = 0; x < width; x++)
		{
			cout << "[" << playground[y][x] << "]";
		}
		cout << endl;
	}

	cout << endl;
}

void calc_zone(division::position& current, division::position& lcorner, division::position& rcorner, map& loc, int range)
{
	if (current.x - range >= 0)
		lcorner.x = current.x - range;
	else
		lcorner.x = 0;

	if (current.y - range >= 0)
		lcorner.y = current.y - range;
	else
		lcorner.y = 0;

	if (current.x + range < loc.width)
		rcorner.x = current.x + range;
	else
		rcorner.x = loc.width - 1;

	if (current.x + range < loc.length)
		rcorner.y = current.y + range;
	else
		rcorner.y = loc.length - 1;

	return;
}


// Class "list" 



division::list::list()
{
	head = nullptr;
	tail = nullptr;
	current = nullptr;
	size = 0;
}

division::list::list(list& orig)
{
	head = new node;
	head->content = orig.head->content;
	head->next = nullptr;

	size = orig.size;
	current = head;
	orig.current = orig.head;

	for (int i = 0; i < size; i++)
	{
		current->next = new node;
		current = current->next;
		orig.current = orig.current->next;
		current->content = orig.current->content;
		current->next = nullptr;
	}
	tail = current;
	current = nullptr;
}

division::list::~list()
{
	current = head;
	while (current != nullptr)
	{
		current = current->next;
		delete head;
		head = current;
	}
}

void division::list::add_elem(division* which)
{
	if (head == nullptr)
	{
		head = new node;
		tail = head;
	}
	else
	{
		tail->next = new node;
		tail = tail->next;
	}

	tail->content = which;
	tail->next = nullptr;
	size++;
	return;
}

void division::list::rm_elem(division* which)
{
	if (size > 0)
	{
		current = head;
		node* to_rm = head;
		while (to_rm->content != which)
		{
			if (current != to_rm)
				current = current->next;
			to_rm = to_rm->next;
		}

		if (to_rm == head)
		{
			if (head->next != nullptr)
			{
				head = head->next;
			}
			else
			{
				head = nullptr;
			}
		}

		delete to_rm;
		size--;
	}
	return;
}

division& division::list::operator[](int index)
{
	current = head;
	if (index < size)
	{
		for (int i = 0; i < index; i++)
		{
			current = current->next;
		}
		return *(current->content);
	}
	else
		throw(std::range_error("index is out of range."));
}

division::list& division::list::operator=(const list& orig)
{
	if (this != &orig)
	{
		current = head;
		while (current != nullptr)
		{
			current = current->next;
			delete head;
			head = current;
		}
		tail = nullptr;

		head = new node;
		head->content = orig.head->content;
		head->next = nullptr;
		size = orig.size;

		current = head;
		node* passing = orig.head;
		for (int i = 0; i < size; i++)
		{
			current->next = new node;
			current = current->next;
			passing = passing->next;
			current->content = passing->content;
			current->next = nullptr;
		}
		tail = current;
		current = nullptr;
	}
	return *this;
}

std::ostream& operator<<(std::ostream& out, division::list& obj)
{
	using std::endl;

	out << " List of available enemies for this division:" << endl;
	obj.current = obj.head;

	int acc = 1;
	if (!obj.size)
		out << " *there are no targets*" << endl;
	else
	{
		for (int i = 0; i < obj.size; i++)
		{
			out << " #" << acc << ": ";
			obj.current->content->short_info();
			obj.current = obj.current->next;
			acc++;
		}
	}

	return out;
}




int get_with_lim(int bottom, int top)
{
	using std::cerr;
	using std::endl;

	int result;
	bool invalid = true;

	while (invalid)
	{
		try
		{
			result = get_int();
			std::cin.get();
		}
		catch (std::invalid_argument& trouble)
		{
			cerr << " Error: " << trouble.what() << endl
				<< " Try again: ";
			continue;
		}

		try
		{
			check_limits(result, bottom, top);
		}
		catch (std::range_error& trouble)
		{
			cerr << " Error: " << trouble.what() << endl
				<< " Try again: ";
			continue;
		}

		invalid = false;
	}

	return result;
}

int get_int()
{
	int result;
	std::cin >> result;

	if (!std::cin)
	{
		std::cin.ignore();
		std::cin.clear();
		throw(std::invalid_argument("You should enter the number!"));
	}

	return result;
}

void check_limits(int input, int bottom, int top)
{
	if (input < bottom || input > top)
	{
		std::string errmsg = "You should enter the number";
		if (bottom != INT_MIN)
		{
			errmsg += " more, than " + std::to_string(bottom);
			if (top != INT_MAX)
				errmsg += " and less, than " + std::to_string(top);
		}
		else if (top != INT_MAX)
			errmsg += " less, than " + std::to_string(top);
		if (top == bottom)
			errmsg = "You can only enter " + std::to_string(bottom);

		throw(std::range_error(errmsg));
	}

	return;
}

bool get_coord(division::position& inp, map& location)
{
	using std::cout;
	using std::endl;

	cout << " Enter coordinate X (0 to cancel): ";
	inp.x = get_with_lim(0, location.width);
	inp.x--;
	cout << " Enter coordinate Y (0 to cancel): ";
	inp.y = get_with_lim(0, location.length);
	inp.y--;

	if (inp.x < 0 || inp.y < 0)
		return false;
	else
		return true;
}