/***************************************************************************
                          testNew.cpp  -  description
                             -------------------
    begin                : Wed Feb 25 2003
    email                : case@ucdhep.ucdavis.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


#include <iostream>
#include <stdlib.h>

#include "DetectorDescription/Parser/interface/DDLParser.h"
#include "DetectorDescription/Parser/interface/FIPConfiguration.h"
#include "DetectorDescription/Algorithm/src/AlgoInit.h"
#include "DetectorDescription/Core/src/DDCheck.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Base/interface/DDException.h"

int main(int argc, char *argv[])
{
  try {
    AlgoInit();

    std::cout << "Get a hold of aDDLParser." << std::endl;
    DDCompactView cpv;
    DDLParser myP(cpv);// = DDLParser::instance();

    std::cout << "main:: initialize" << std::endl;
    //  DDInit();

    std::cout << "======================" << std::endl << std::endl;

    std::cout << "Defining my Document Provider, using the default one provided" << std::endl;

    FIPConfiguration cf(cpv);

    std::cout << "Make sure the provider is ready... you don't need this, it's just necessary for the default one." << std::endl;
    int errNumcf;
    errNumcf = cf.readConfig("DetectorDescription/Parser/test/config1.xml");
    if (!errNumcf)
      {
	std::cout << "about to call DumpFileList of configuration cf" << std::endl;
	cf.dumpFileList();
      }
    else
      {
	std::cout << "error in ReadConfig" << std::endl;
	return -1;
      }


    //  std::cout << "main::about to set configuration" << std::endl;
    //   myP->SetConfig("configuration.xml");

    //   std::cout << "main::about to start parsing" << std::endl;
 
    //   myP->StartParsing();


    // Parse the files provided by the DDLDocumentProvider above.
    std::cout << " parse all the files provided by the DDLDocumentProvider" << std::endl;
    myP.parse(cf);

    std::cout << "===================" << std::endl << std::endl;
    std::cout << " parse just one file THAT WAS ALREADY PARSED (materials.xml).  We should get a WARNING message." << std::endl;
    myP.parseOneFile("DetectorDescription/Parser/test/materials.xml");


    std::cout << "===================" << std::endl << std::endl;
    std::cout << " parse just one file that has not been parsed (specpars.xml).  This should just go through." << std::endl;
    myP.parseOneFile("DetectorDescription/Parser/test/specpars.xml");


    std::cout << "===================" << std::endl << std::endl;
    std::cout << " make a new provider and read some other configuration" << std::endl;

    FIPConfiguration cf2(cpv);
    cf2.readConfig("DetectorDescription/Parser/test/config2.xml");
    myP.parse(cf2);

    std::cout << "Done Parsing" << std::endl;
    std::cout << "===================" << std::endl << std::endl;
  
    std::cout << std::endl << std::endl << "main::Start checking!" << std::endl << std::endl;
    DDCheckMaterials(std::cout);

//     DDCompactView cpv;

//     cpv.clear();
    std::cout << "cleared DDCompactView.  " << std::endl;
  
    return EXIT_SUCCESS;
  }
  catch (DDException& e)
    {
      std::cout << "main::PROBLEM:" << std::endl 
	   << "         " << e.what() << std::endl;
    }  

}
