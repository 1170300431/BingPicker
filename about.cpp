#pragma once
#include "GUI.h"


form about((char*)"JamzumSum", (char*)"About");

void about_onCreate(form* me) {
	me->resize(392,280);
	me->move(336,112);
	Label I(me, 24, 56, 344, 216, (char*)
		"Developer:\n"
		"    JamzumSum\n\n"
		"Credit:\n"
		"    OpenCV Project\n\n"
		"Thanks for using.\n");
	I.create();
}