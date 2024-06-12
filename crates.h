#ifndef CRATE
#define CRATE

#include <iostream>
#include <string>

class crate
{
private:
	int pos_x;
	int pos_y;

protected:
	std::string type;

public:
	crate(int loclen, int locwid);
	virtual ~crate() = 0 {}

	void get_pos(int& port_x, int& port_y) { port_x = pos_x; port_y = pos_y; }
	void info() { std::cout << type; };
};

class coins_crate : virtual public crate
{
private:
	int value;
public:
	coins_crate(int loclen, int locwid);
	virtual ~coins_crate() {}

	int get_coins() { return value; };
};

class medkit_crate : virtual public crate
{
private:
	int level;

public:
	medkit_crate(int loclen, int locwid);
	virtual ~medkit_crate() {}

	int get_level() { return level; };
};

class optics_crate : virtual public crate
{
private:
	int level;

public:
	optics_crate(int loclen, int locwid);
	virtual ~optics_crate() {}

	int get_level() { return level; };
};

class arsenal_crate : virtual public crate
{
private:
	int level;
public:
	arsenal_crate(int loclen, int locwid);
	virtual ~arsenal_crate() {}

	int get_level() { return level; };
};

#endif