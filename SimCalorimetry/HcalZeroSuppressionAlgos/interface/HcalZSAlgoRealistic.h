#ifndef SIMCALORIMETRY_HCALZEROSUPPRESSIONALGOS_HCALZSALGOREALISTIC_H
#define SIMCALORIMETRY_HCALZEROSUPPRESSIONALGOS_HCALZSALGOREALISTIC_H 1

#include "SimCalorimetry/HcalZeroSuppressionAlgos/interface/HcalZeroSuppressionAlgo.h"

/** \class HcalZSAlgoRealistic
  *  
  * Simple amplitude-based zero suppression algorithm.  For each digi, add
  * up consecutive 2 samples in a slice of 10 time samples, beginning with (start) 
  * sample. If any of the sums are greater then the threshold, keep the event.
  *
  * $Date: 2009/10/29 17:28:49 $
  * $Revision: 1.5 $
  * \author S. Sengupta - Minnesota
  */
class HcalZSAlgoRealistic : public HcalZeroSuppressionAlgo {
public:
  HcalZSAlgoRealistic(bool markAndPass);
  HcalZSAlgoRealistic(bool markAndPass, int levelHB, int levelHE, int levelHO, int levelHF);
  
protected:
  virtual bool shouldKeep(const HBHEDataFrame& digi) const;
  virtual bool shouldKeep(const HODataFrame& digi) const;
  virtual bool shouldKeep(const HFDataFrame& digi) const;
private:
  bool usingDBvalues; 
  int thresholdHB_, thresholdHE_, thresholdHO_, thresholdHF_;
  bool keepMe(const HBHEDataFrame& inp, int threshold, uint32_t hbhezsmask) const;
  bool keepMe(const HODataFrame& inp, int threshold, uint32_t hozsmask) const;
  bool keepMe(const HFDataFrame& inp, int threshold, uint32_t hfzsmask) const;
};

#endif
