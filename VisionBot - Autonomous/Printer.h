#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <strstream> 
#include <conio.h>
#include <windows.h>
#include "serial_com.h"


using namespace std;
class Printer
{
	HANDLE h1;
	char buffer[64];
	char s1[1000];
	char ch;
	int n, s_index = 0;
	
	double xr = 0.0;
public:
	Printer(char *PortName);
	void setHome();
	void move(int x, int y, int z);
	void currentPosition(string &Position);
	bool isReady=false;
	int xP , yP, zP ;
	~Printer();
};

