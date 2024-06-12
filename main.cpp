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
	}


	int turn = 0;
	bool game_over = false;
	for (int i = 0; i < pool_size; i++)
	{
		for (int j = i + 1; j < pool_size; j++)
		{
			turn = i;
			while (!game_over)
			{
				if (!pool[turn]->check_ready())
				{
					if (turn == i)
						pool[turn]->set_side(0);
					else
						pool[turn]->set_side(1);
					pool[turn]->equipment();
					cout << "\"" << pool[turn]->show_name() << "\" has formed an army!" << endl;
					pool[turn]->placement();
					pool[turn]->set_EL(pool, pool_size);
				}
				else
				{
					if (turn == i)
						battlefield.crate_spawn_event();
					for (int k = 1; k < 3; k++)
					{
						system("cls");
						cout << pool[turn]->show_name() << "'s turn." << endl;
						battlefield.draw();
						pool[turn]->search_alive();
						if (pool[turn]->is_defeat())
						{
							game_over = true;
							cout << pool[turn]->show_name() << " eliminated!" << endl;
							cin.get();
							if (turn == i)
								pool[j]->won();
							else
								pool[i]->won();
							break;
						}

						pool[turn]->choose_action(k);

						if (pool[turn]->is_defeat())
						{
							game_over = true;
							cout << pool[turn]->show_name() << " eliminated!" << endl;
							if (turn == i)
								pool[j]->won();
							else
								pool[i]->won();
							break;
						}
					}
				}

				if (turn == i)
					turn = j;
				else
					turn = i;
			}

			pool[i]->refresh();
			pool[j]->refresh();
			battlefield.refresh();
			game_over = false;
		}
	}

	player* winner = nullptr;
	int max_points = 0;
	system("cls");
	for (int i = 0; i < pool_size; i++)
	{
		cout << " " << pool[i]->show_name() << ": " << pool[i]->get_wins() << " points" << endl;
		if (max_points < pool[i]->get_wins())
		{
			max_points = pool[i]->get_wins();
			winner = pool[i];
		}
	}
	cout << endl;
	cout << winner->show_name() << " won the game!" << endl;

	return 0;
}