/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/Model.h"

using namespace std;

namespace Elegent
{

string Model::CompileShortLabel() const
{
	string out = shortLabel.name;
 
  	if (!shortLabel.mode.empty() && shortLabel.mode.compare("full"))
		out += ":" + shortLabel.mode;

  	if (!shortLabel.variant.empty())
		out += " (" + shortLabel.variant + ")";

	out += " [" + shortLabel.version + "]";

	return out;
}

//----------------------------------------------------------------------------------------------------

string Model::CompileFullLabel() const
{
	string out = fullLabel.name;
 
  	if (!fullLabel.mode.empty() && fullLabel.mode.compare("full"))
		out += ":" + fullLabel.mode;

  	if (!fullLabel.variant.empty())
		out += " (" + fullLabel.variant + ")";

	out += " [" + fullLabel.version + "]";

	return out;
}

//----------------------------------------------------------------------------------------------------

Model *model = NULL;

}
