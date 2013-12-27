/***************************************************************************
                          this_demo.h  -  description
                             -------------------
    begin                : Fri May 5 2000
    copyright            : (C) 2000 by Maciej Mroz
    email                : mmroz@student.uci.agh.edu.pl
 ***************************************************************************/

#include"../demosystem.h"

class DSampleDemo:public DMDemo {
	DMInterpolator *interps[20];
	Texture* tex;
	DMScene* scene;
public:
	DSampleDemo();
	virtual ~DSampleDemo();
};