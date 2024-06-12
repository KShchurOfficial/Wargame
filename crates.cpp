#include "crates.h"
#include <cstdlib>

// class crate

crate::crate(int loclen, int locwid)
{
	type = "default";
	pos_y = 1 + loclen * 0.2 + rand() % (int(loclen * 0.6));
	pos_x = 1 + locwid * 0.2 + rand() % (int(locwid * 0.6));
}


// class coins_crate

coins_crate::coins_crate(int loclen, int locwid) : crate(loclen, locwid)
{
	type = "crate of coins";
	value = 50 + rand() % 101;
}


// class medkit_crate

medkit_crate::medkit_crate(int loclen, int locwid) : crate(loclen, locwid)
{
	type = "medkit crate";

	level = 1 + rand() % 100;
	if (level > 97)
		level = 3; // 70% of division health will be recovered
	else if (level > 85)
		level = 2; // 35% of division health will be recovered
	else
		level = 1; // 10% of division health will be recovered 
}


// class optics_crate

optics_crate::optics_crate(int loclen, int locwid) : crate(loclen, locwid)
{
	type = "optics crate";

	level = 1 + rand() % 100;
	if (level > 97)
		level = 3; // X5 for critical damage chance; miss chance will be decreased in 5 times;
	else if (level > 90)
		level = 2; // X3 for critical damage chance; miss chance will be decreased in 2 times;
	else
		level = 1; // X2 for critical damage chance; miss chance will be decreased in 1.5 times;
}


// class arsenal_crate

arsenal_crate::arsenal_crate(int loclen, int locwid) : crate(loclen, locwid)
{
	type = "arsenal crate";

	level = 1 + rand() % 100;
	if (level > 97)
		level = 3; // X1.5 for division average damage
	else if (level > 75)
		level = 2; // X1.2 for division average damage
	else
		level = 1; // X1.1 for division average damage
}