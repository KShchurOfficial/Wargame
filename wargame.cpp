#include "wargame.h"
#include <iostream>
#include <stdexcept>
#include <Windows.h>
#include <ctime>
#include <cstdlib>
#include <typeinfo>


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

	chosen = nullptr;
	defeat = false;
	location = nullptr;
	enemies = nullptr;
	enemies_amt = 0;
	ready = false;
	side = 0;
	wins = 0;
	coins = 1000;
}

player::~player()
{
	delete[] enemies;
}

void player::equipment()
{
	using std::cout;
	using std::cin;
	using std::endl;

	ready = false;
	bool finish = false;
	int dialog = 0;
	division* recruit = nullptr;

	while (!finish)
	{
		system("cls");
		cout << "\"" << name << "\" turn." << endl << endl
			<< " You have " << coins << " coins." << endl;
		cout << " Choose the type of equiping division:" << endl
			<< " #1 Default type division (100 coins)" << endl
			<< " #0 Finish equipment" << endl
			<< " Your choice: ";
		dialog = get_with_lim(0, 1);

		switch (dialog)
		{
		case 1:
		{
			recruit = new division();
			if (coins < recruit->price)
			{
				delete recruit;
				cout << " You haven't enough coins to equip division of this type! ";
			}
			else
			{
				coins -= recruit->price;
				army.add_elem(&recruit);
				cout << recruit->type << " equiped!" << endl;
			}
			break;
		}
		case 0:
		{
			if (army.is_empty())
			{
				cout << " You should equip at least one division!" << endl;
			}
			else
			{
				finish = true;
			}
			break;
		}
		}
		cin.get();
	}

	return;
}


void player::placement()
{
	using std::cout;
	using std::cin;
	using std::cerr;
	using std::endl;

	int input = 0;

	while (!ready)
	{
		system("cls");
		cout << "\"" << name << "\" turn." << endl << endl;
		location->draw();

		ready = true;
		for (int i = 0; i < army.get_size(); i++)
		{
			if (army[i]->pos.x == -1 && army[i]->pos.y == -1)
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
		for (int i = 0; i < army.get_size(); i++)
		{
			if (army[i]->pos.x == -1 && army[i]->pos.y == -1)
				cout << " division #" << i + 1 << endl;
		}
		cout << " Enter the number of placing division: ";
		input = get_with_lim(1, army.get_size());
		input--;

		chosen = army[input];

		if (chosen->pos.x != -1 && chosen->pos.y != -1)
		{
			cout << " This division was already placed!" << endl;
		}
		else
		{
			chosen->place_division(side, *location);
			try
			{
				check_other(chosen);
			}
			catch (err& trouble)
			{
				cerr << " Error: " << trouble.what() << endl;
				cout << " Try again." << endl;
				chosen->pos.x = -1;
				chosen->pos.y = -1;
				cin.get();
				continue;
			}

			location->save_div(chosen);
			cout << " Division placed!" << endl;
			chosen = nullptr;
		}
		cin.get();
		system("cls");
	}
	return;
}

void player::check_other(division* checking)
{
	using std::cout;
	using std::endl;

	for (int i = 0; i < army.get_size(); i++)
	{
		if (army[i] == checking)
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

void player::refresh()
{
	coins = 1000;
	defeat = false;
	ready = false;

	division* passing = nullptr;
	for (int i = 0; i < army.get_size(); i++)
	{
		passing = army[i];
		army.rm_elem(&passing);
	}

	return;
}

void player::search_alive()
{
	using std::cout;
	using std::endl;

	defeat = true;
	for (int i = 0; i < army.get_size(); i++)
	{
		if (army[i]->defeated)
		{
			army.rm_elem(i);
			i--;
		}
	}
	if (army.get_size())
		defeat = false;

	return;
}

division* player::search_by_pos(int& srch_x, int& srch_y)
{
	bool found = false;
	for (int i = 0; i < army.get_size(); i++)
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
			<< " 2) Equip more divisions" << endl
			<< " 3) give up" << endl
			<< " Your choice: ";
		dialog = get_with_lim(1, 3);

		switch (dialog)
		{
		case 1:
		{
			bool denial = false;
			chosen = nullptr;

			cout << " #0 cancel action" << endl;
			for (int i = 0; i < army.get_size(); i++)
			{
				cout << " #" << i + 1 << ": ";
				army[i]->short_info();
			}
			cout << endl;

			while (chosen == nullptr)
			{
				cout << " Your input: ";
				dialog = get_with_lim(0, army.get_size());
				dialog--;
				if (dialog < 0)
				{
					denial = true;
					step--;
					break;
				}

				try
				{
					chosen = army[dialog];
					cin.get();
				}
				catch (std::range_error& trouble)
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
			}

			break;
		}
		case 2:
		{
			equipment();
			placement();
			step = 2;
			break;
		}
		case 3:
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

				int found_drop = 0;
				try
				{
					found_drop = chosen->move(*location, inp.x, inp.y);
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

				if (found_drop)
				{
					crate* found = location->find_crate(inp.x, inp.y);
					cout << " Got ";
					found->info();
					cout << "! " << endl;
					crate_unboxing(found);
					location->crates.rm_elem(&found);
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
				cout << " Available enemies: " << endl;
				for (int i = 0; i < chosen->enemy_list.get_size(); i++)
				{
					cout << " #" << i + 1 << ": ";
					chosen->enemy_list[i]->short_info();
				}
				cout << endl;

				if (!chosen->enemy_list.get_size())
				{
					cin.get();
					step--;
				}
				else
				{
					cout << " Your input: ";
					dialog = get_with_lim(1, chosen->enemy_list.get_size());
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
					catch (income& money)
					{
						cout << "\"" << name << "\" ";
						coins += money.msg();
						cin.get();
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

void player::crate_unboxing(crate* found)
{
	if (typeid(*found) == typeid(coins_crate))
	{
		coins_crate* adapter = dynamic_cast<coins_crate*>(found);
		coins += adapter->get_coins();
	}
	else if (typeid(*found) == typeid(medkit_crate))
	{
		medkit_crate* adapter = dynamic_cast<medkit_crate*>(found);
		int heal_multiplier = adapter->get_level();
		switch (heal_multiplier)
		{
		case 1:
		{
			chosen->health += chosen->max_health * 0.1;
			break;
		}
		case 2:
		{
			chosen->health += chosen->max_health * 0.35;
			break;
		}
		case 3:
		{
			chosen->health += chosen->max_health * 0.7;
			break;
		}
		}
		if (chosen->health > chosen->max_health)
			chosen->health = chosen->max_health;
	}
	else if (typeid(*found) == typeid(arsenal_crate))
	{
		arsenal_crate* adapter = dynamic_cast<arsenal_crate*>(found);
		int arsenal_power = adapter->get_level();
		switch (arsenal_power)
		{
		case 1:
		{
			chosen->aver_dmg *= 1.1;
			break;
		}
		case 2:
		{
			chosen->aver_dmg *= 1.2;
			break;
		}
		case 3:
		{
			chosen->aver_dmg *= 1.5;
			break;
		}
		}

	}
	else if (typeid(*found) == typeid(optics_crate))
	{
		optics_crate* adapter = dynamic_cast<optics_crate*>(found);
		int optics_power = adapter->get_level();
		switch (optics_power)
		{
		case 1:
		{
			chosen->crit_chance *= 1.15;
			chosen->precise = 100 - ((100 - chosen->precise) / 1.1);
			break;
		}
		case 2:
		{
			chosen->crit_chance *= 1.5;
			chosen->precise = 100 - ((100 - chosen->precise) / 1.25);
			break;
		}
		case 3:
		{
			chosen->crit_chance *= 2;
			chosen->precise = 100 - ((100 - chosen->precise) / 1.5);
			break;
		}
		}

		if (chosen->crit_chance > 100.0)
			chosen->crit_chance = 100.0;
	}

	return;
}



// Class "division"



division::division()
{
	using std::cout;
	using std::cin;
	using std::endl;

	side = -1;
	pos.x = -1;
	pos.y = -1;
	type = "default type division";
	defeated = false;
	max_health = 100.0;
	health = max_health;
	aver_dmg = 20.0;
	crit_chance = 20.0;
	precise = 80.0;
	attack_range = 2;
	move_range = 3;
	price = 100;
}

void division::place_division(bool which_side, map& loc)
{
	side = which_side;
	int place_lim = loc.length / 5;

	std::cout << " Enter position X: ";
	pos.x = get_with_lim(1, loc.width);
	std::cout << " Enter position Y: ";
	if (!side)
		pos.y = get_with_lim(1, place_lim);
	else
		pos.y = get_with_lim(loc.length - place_lim + 1, loc.length);

	pos.x--;
	pos.y--;
}

void division::reach_area_comp(map& pg)
{
	calc_zone(pos, move_limits[0], move_limits[1], pg, move_range);
	return;
}

int division::move(map& pg, int destX, int destY)
{
	if (pg.playground[destY][destX] != ' ' && pg.playground[destY][destX] != '+')
	{
		throw(player::err("Cannot move division on this cell!"));
	}
	else if (destX < move_limits[0].x || destX > move_limits[1].x || destY < move_limits[0].y || destY > move_limits[1].y)
	{
		throw(std::range_error("Destination point is out of reach area!"));
	}
	else
	{
		int found_drop = (pg.playground[destY][destX] == '+');

		pg.playground[pos.y][pos.x] = ' ';
		pos.x = destX;
		pos.y = destY;
		pg.playground[pos.y][pos.x] = 'D';

		return found_drop;
	}
}

void division::killzone_comp(map& pg)
{
	calc_zone(pos, kill_zone[0], kill_zone[1], pg, attack_range);
	return;
}

void division::refresh_EL()
{
	for (int i = 0; i < enemy_list.get_size(); i++)
	{
		if (enemy_list[i]->pos.x < kill_zone[0].x || enemy_list[i]->pos.y < kill_zone[0].y ||
			enemy_list[i]->pos.x > kill_zone[1].x || enemy_list[i]->pos.y > kill_zone[1].y ||
			enemy_list[i]->defeated)
		{
			enemy_list.rm_elem(i);
			i--;
		}
	}
	return;
}

void division::update_EL(player** enemies, int enemies_amt)
{
	bool skip = false;
	division* current = nullptr;

	for (int i = 0; i < enemies_amt; i++)
	{
		for (int j = 0; j < enemies[i]->army.get_size(); j++)
		{
			if (enemies[i]->army[j]->pos.x >= kill_zone[0].x && enemies[i]->army[j]->pos.y >= kill_zone[0].y &&
				enemies[i]->army[j]->pos.x <= kill_zone[1].x && enemies[i]->army[j]->pos.y <= kill_zone[1].y &&
				!enemies[i]->army[j]->defeated)
			{
				for (int k = 0; k < enemy_list.get_size(); k++)
				{
					current = enemy_list[k];
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

				enemy_list.add_elem(&enemies[i]->army[j]);
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
		target = enemy_list[victim_number];
	}
	catch (std::range_error& trouble)
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
		throw(player::income("Enemy division destroyed", 50));
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



map::map(int w_init, int l_init, int p_amt)
{
	playground = nullptr;
	playernum = p_amt;
	int mpsize = (playernum + playernum % 2) * 5;
	if (w_init < mpsize)
		width = mpsize;
	else
		width = w_init;

	if (l_init < mpsize)
		length = mpsize;
	else
		length = l_init;

	crate_spawn_timer = 1;
}

void map::settings()
{
	using std::cout;
	using std::cin;
	using std::endl;

	int dialog = 0;

	while (dialog != 4)
	{
		system("cls");
		cout << " Settings." << endl
			<< " 1) Length: " << length << endl
			<< " 2) Width: " << width << endl
			<< " 3) Amount of players: " << playernum << endl
			<< " 4) Go back to main menu" << endl
			<< " Your input: ";
		dialog = get_with_lim(1, 4);

		switch (dialog)
		{
		case 1:
		{
			cout << " Enter new value (between 10 and 50): ";
			dialog = get_with_lim(10, 50);
			set_length(dialog, playernum);
			break;
		}
		case 2:
		{
			cout << " Enter new value (between 10 and 50): ";
			dialog = get_with_lim(10, 50);
			set_width(dialog, playernum);
			break;
		}
		case 3:
		{
			cout << " Enter new value (between 2 and 4): ";
			playernum = get_with_lim(2, 4);
			break;
		}
		case 4:
			break;
		}
	}

	return;
}

void map::init()
{
	playground = new char* [length];
	for (int y = 0; y < length; y++)
	{
		playground[y] = new char[width];
		for (int x = 0; x < width; x++)
			playground[y][x] = ' ';
	}
	return;
}

void map::set_width(int w_init, int playernum)
{
	int minsize = (playernum + playernum % 2) * 5;
	if (w_init < minsize)
		width = minsize;
	else
		width = w_init;

	return;
}

void map::set_length(int l_init, int playernum)
{
	int minsize = (playernum + playernum % 2) * 5;
	if (l_init < minsize)
		length = minsize;
	else
		length = l_init;

	return;
}

map::~map()
{
	if (playground != nullptr)
	{
		for (int y = 0; y < length; y++)
			delete[] playground[y];
		delete[] playground;
	}
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

void map::refresh()
{
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			playground[y][x] = ' ';
		}
	}

	int crate_amt = crates.get_size();
	for (int i = crate_amt - 1; i >= 0; i--)
		crates.rm_elem(i);

	return;
}

void map::crate_spawn_event()
{
	if (crate_spawn_timer % 4)
		crate_spawn_timer++;
	else
	{
		crate* airdrop = nullptr;
		int type_ID = 0;
		int drop_x = 0;
		int drop_y = 0;

		while (true)
		{
			type_ID = 1 + rand() % 100;
			if (type_ID > 90)
				airdrop = new optics_crate(length, width);
			else if (type_ID > 80)
				airdrop = new arsenal_crate(length, width);
			else if (type_ID > 65)
				airdrop = new medkit_crate(length, width);
			else
				airdrop = new coins_crate(length, width);

			airdrop->get_pos(drop_x, drop_y);
			if (playground[drop_y][drop_x] == 'D')
			{
				delete airdrop;
				continue;
			}
			else if (playground[drop_y][drop_x] == '+')
			{
				crate* found = find_crate(drop_x, drop_y);
				crates.rm_elem(&found);
			}
			else if (playground[drop_y][drop_x] == ' ')
				playground[drop_y][drop_x] = '+';

			crates.add_elem(&airdrop);
			break;
		}
		crate_spawn_timer /= 4;
	}
	return;
}

crate* map::find_crate(int srch_x, int srch_y)
{
	crate* found = nullptr;
	int cmp_x = 0;
	int cmp_y = 0;

	for (int i = 0; i < crates.get_size(); i++)
	{
		crates[i]->get_pos(cmp_x, cmp_y);
		if (cmp_x == srch_x && cmp_y == srch_y)
		{
			found = crates[i];
			break;
		}
	}

	return found;
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