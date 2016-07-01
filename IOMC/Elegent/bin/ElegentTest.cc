/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include <cstdio>
#include <cstdlib>

#include "interface/Generator.h"

#include "HepMC/GenEvent.h"

using namespace Elegent;
using namespace std;
using namespace HepMC;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: ElegentTest <file name> <model path> <t_min> <t_max> <number of events>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 6)
	{
		PrintUsage();
		return 1;
	}

	Generator generator(argv[1], argv[2], atof(argv[3]), atof(argv[4]));	
	if (generator.Init() != 0)
		return 2;

	unsigned int N = atoi(argv[5]);
	for (unsigned int i = 0; i < N; i++)
	{
		GenEvent* gEv = new GenEvent();
		gEv->set_event_number(i);
		generator.Generate(gEv);
		gEv->print();
		delete gEv;
	}

	return 0;
}
