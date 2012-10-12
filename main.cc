// Copyright 2009 Isis Innovation Limited
// This is the main extry point for PTAMM
#include <stdlib.h>
#include <iostream>
#include <gvars3/instances.h>
#include "System.h"

using namespace std;
using namespace GVars3;

int main()
{
	cout << "  Parsing settings.cfg ...." << endl;
	GUI.LoadFile("settings.cfg");

	GUI.StartParserThread(); // Start parsing of the console input
	atexit(GUI.StopParserThread);

	try{
		PTAMM::System s;
		s.Run();
	}
	catch(CVD::Exceptions::All e){
		cout << endl;
		cout << "!! Failed to run system; got exception. " << endl;
		cout << "   Exception was: " << endl;
		cout << e.what << endl;
	}

	return 0;
}



