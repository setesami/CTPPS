#ifndef PPSTrackerDetId_h
#define PPSTrackerDetId_h


#include <DataFormats/DetId/interface/DetId.h>
#include <FWCore/Utilities/interface/Exception.h>

#include <iosfwd>
#include <iostream>

class PPSTrackerDetId :public DetId {
  
 public:
      
  PPSTrackerDetId();

  /// Construct from a packed id. It is required that the Detector part of
  /// id is Totem and the SubDet part is T1, otherwise an exception is thrown.
  explicit PPSTrackerDetId(uint32_t id);


  /// Construct from fully qualified identifier.
  PPSTrackerDetId(unsigned int Arm, 
		  unsigned int Station,
	  unsigned int RP,
	  unsigned int Plane);
    


 /// Bit 24 = Arm: 0=z>0 1=z<0
  /// Bits [22:23] Station (0 = 210 or ex 147) 
  /// Bits [19:21] RP
  /// Bits [16:18] Si Plane
/// 
 int Arm() const{
    return int((id_>>startArmBit) & 0X1);
  }
  inline int Station() const
    {
      return int((id_>>startStationBit) & 0x3);
    }
  int RP() const{
    return int((id_>>startRPBit) & 0X7);
  }

  int Plane() const{
    return int((id_>>startPlaneBit) & 0X7);
  }

  int trIndex() const{
    return trind;
  }

  void set(unsigned int a, unsigned int b, unsigned int c,unsigned int d ){
//    unsigned int d=0;
    this->init(a,b,c,d);
  }

  // void setLayer(unsigned int a, unsigned int b, unsigned int c, unsigned int d){
  //  this->init(a,b,c,d);
  //}

  static const int startArmBit = 24;
  static const int startStationBit = 22;
  static const int startRPBit = 19;
  static const int startPlaneBit = 16;
//  static const int startLayerBit = 16;
 
 private:
  void init(unsigned int Arm, unsigned int Station,unsigned int RP, unsigned int Plane); 
  int trind;

}; // PPSTrackerDetId

std::ostream& operator<<( std::ostream& os, const PPSTrackerDetId& id );

#endif
