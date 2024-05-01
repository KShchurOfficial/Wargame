#include "wargame.h"
#include <iostream>

int main()
{
	using std::cout;
	using std::cin;
	using std::endl;
	srand(time(0));

	player** pool;
	map battlefield;
	int pool_size = 0;
	cout << " Enter the amount of players: ";
	pool_size = get_with_lim(2, 2);  // временное ограничение на кол-во игроков (пока что можно только вдвоём)
	pool = new player * [pool_size];
	for (int i = 0; i < pool_size; i++)
	{
		pool[i] = new player(i);
		pool[i]->set_location(&battlefield);
		pool[i]->placement(battlefield);
	}
	for (int i = 0; i < pool_size; i++)
		pool[i]->set_EL(pool, pool_size);

	int dialog = 0;
	int turn = 0;
	bool game_over = false;
	int step = 1;
	while (!game_over)
	{
		for (int i = 1; i < 3; i++)
		{
			system("cls");
			cout << pool[turn]->show_name() << "'s turn." << endl;
			battlefield.draw();
			pool[turn]->choose_action(i);
			if (pool[turn]->is_defeat())
			{
				game_over = true;
				cout << pool[turn]->show_name() << " lost the game!" << endl;
				break;
			}
		}


		turn++;
		if (turn == pool_size)
			turn = 0;
	}

	return 0;
}