#include "pacman.h"

pacman::pacman()
{
	x = 4;
	y = 6;
	rotation = 3;
}

pacman::pacman(string s)
{
	model = s;
	x = 4;
	y = 6;
	rotation = 3;
}
