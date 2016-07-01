/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_modelfactory_
#define _elegent_modelfactory_

#include "IslamModel.h"
#include "PPPModel.h"
#include "BSWModel.h"
#include "BHModel.h"
#include "JenkovszkyModel.h"
#include "ExpModel.h"

#include <string>
#include <map>

namespace Elegent
{

/**
 * \brief A class to give list of available models and to create an instance of a model specified by tag.
 **/
class ModelFactory
{
	protected:
		/// map: tag --> model instance
		std::map<std::string, Model*> model_map;

	public:
		ModelFactory();

		void PrintList() const;

		Model* MakeInstance(const std::string &tag, bool callInit = true) const;
};


} // namespace

#endif
