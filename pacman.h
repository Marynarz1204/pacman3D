#pragma once
#include "object.h"
class pacman : public object
{
public:
	pacman();
	pacman(string x);
	int x;
	int y;
	int rotation;
};

