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
	bool game_start = false;
	int dialog = 0;

	cout << " Greetings in \"Wargame\"!" << endl;
	cin.get();
	while (!game_start && dialog != 3)
	{
		system("cls");
		cout << " Main menu." << endl
			<< " 1) Open settings" << endl
			<< " 2) start the new game" << endl
			<< " 3) quit the game" << endl
			<< " Your input: ";
		dialog = get_with_lim(1, 3);
		system("cls");

		switch (dialog)
		{
		case 1:
		{
			battlefield.settings();
			break;
		}
		case 2:
		{
			game_start = true;
			battlefield.init();
			pool_size = battlefield.get_playernum();
			break;
		}
		case 3:
		{
			cout << " Bye!" << endl;
			exit(EXIT_SUCCESS);
		}
		}
	}


	pool = new player * [pool_size];
	for (int i = 0; i < pool_size; i++)
	{
		pool[i] = new player(i + 1);
		pool[i]->set_location(&battlefield);
		pool[i]->equipment();
		pool[i]->placement();
	}
	for (int i = 0; i < pool_size; i++)
		pool[i]->set_EL(pool, pool_size);


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
			pool[turn]->search_alive();
			if (pool[turn]->is_defeat())
			{
				game_over = true;
				cout << pool[turn]->show_name() << " eliminated!" << endl;
				break;
			}

			pool[turn]->choose_action(i);

			if (pool[turn]->is_defeat())
			{
				game_over = true;
				cout << pool[turn]->show_name() << " eliminated!" << endl;
				break;
			}
		}


		turn++;
		if (turn == pool_size)
			turn = 0;
	}

	return 0;
}