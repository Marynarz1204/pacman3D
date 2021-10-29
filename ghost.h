#pragma once
#include "object.h"
class ghost :
    public object
{
public:
	int counter;
	int x;
	int y;
	int rotation;
	ghost();
	ghost(string, int, int);
};

