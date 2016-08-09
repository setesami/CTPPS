/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: ProtonTransportFunctions.h,v $: $
* $Revision: 1.1.2.2 $
* $Date: 2009/11/03 10:37:15 $
*
****************************************************************************/


#ifndef _ProtonTransportFunctions_h_
#define _ProtonTransportFunctions_h_

#include <vector>
#include <map>

class LHCOpticsApproximator;
class TFile;

/**
 *\brief List of all available optical functions.
 **/
class ProtonTransportFunctions
{
  public:
    struct FunctionPair {
      LHCOpticsApproximator *ideal;
      LHCOpticsApproximator *real;
    };

    typedef std::map<unsigned int, FunctionPair> MapType;

  protected:
    /// map RPId -> transport functions
    MapType functionMap;

    void InitFunction(unsigned int RPId, LHCOpticsApproximator *);

    friend class ProtonTransportFunctionsESSource;

  public:
    ProtonTransportFunctions();
    ~ProtonTransportFunctions();

    /// to get a real transport function by RPId
    /// throws an exception if doesn't exist
    LHCOpticsApproximator* GetFunction(unsigned int RPId) const;

    const MapType& GetFunctionMap() const { return functionMap; }
};

#endif

