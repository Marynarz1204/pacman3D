#include "ghost.h"

ghost::ghost()
{
	x = 4;
	y = 6;
	rotation = 1;
}

ghost::ghost(string s, int xx, int yy)
{
	model = s;
	x = xx;
	y = yy;
	rotation = 1;
	counter = 0;
}
