#ifndef GEOMETRY_CALOTOPOLOGY_ECALBARRELTOPOLOGY_H
#define GEOMETRY_CALOTOPOLOGY_ECALBARRELTOPOLOGY_H 1

#include <vector>
#include <iostream>
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"

class EcalBarrelTopology : public CaloSubdetectorTopology
{

 public:
  /// create a new Topology
  EcalBarrelTopology(): theGeom_(0) {};

  /// virtual destructor
  virtual ~EcalBarrelTopology() { }  

  /// create a new Topology from geometry
  EcalBarrelTopology(edm::ESHandle<CaloGeometry> theGeom) : theGeom_(theGeom)
    {
    }
  
  /// move the Topology north (increment iphi)
  virtual std::vector<DetId> north(const DetId& id) const
    { 
      EBDetId nextId(incrementIphi(EBDetId(id)));
      std::vector<DetId> vNeighborsDetId;
      if (! (nextId==EBDetId(0)))
	vNeighborsDetId.push_back(DetId(nextId.rawId()));
      return vNeighborsDetId;
    }

  /// move the Topology south (decrement iphi)
  virtual std::vector<DetId> south(const DetId& id) const
    { 
      EBDetId nextId(decrementIphi(EBDetId(id)));
      std::vector<DetId> vNeighborsDetId;
      if (! (nextId==EBDetId(0)))
	vNeighborsDetId.push_back(DetId(nextId.rawId()));
      return vNeighborsDetId;
    }

  /// move the Topology east (negative ieta)
  virtual std::vector<DetId> east(const DetId& id) const
    { 
      EBDetId nextId(decrementIeta(EBDetId(id)));
      std::vector<DetId> vNeighborsDetId;
      if (! (nextId==EBDetId(0)))
	vNeighborsDetId.push_back(DetId(nextId.rawId()));
      return vNeighborsDetId;
    }

  /// move the Topology west (positive ieta)
  virtual std::vector<DetId> west(const DetId& id) const
    { 
      EBDetId nextId(incrementIeta(EBDetId(id)));
      std::vector<DetId> vNeighborsDetId;
      if (! (nextId==EBDetId(0)))
	vNeighborsDetId.push_back(DetId(nextId.rawId()));
      return vNeighborsDetId;
    }
  
  virtual std::vector<DetId> up(const DetId& id) const
    {
      std::cout << "EcalBarrelTopology::up() not yet implemented" << std::endl; 
      std::vector<DetId> vNeighborsDetId;
      return  vNeighborsDetId;
    }
  
  virtual std::vector<DetId> down(const DetId& id) const
    {
      std::cout << "EcalBarrelTopology::down() not yet implemented" << std::endl; 
      std::vector<DetId> vNeighborsDetId;
      return  vNeighborsDetId;
    }

 private:

  /// move the nagivator to larger ieta (more positive z) (stops at end of barrel and returns null)
  EBDetId incrementIeta(const EBDetId&) const ;

  /// move the nagivator to smaller ieta (more negative z) (stops at end of barrel and returns null)
  EBDetId decrementIeta(const EBDetId&) const ;

  /// move the nagivator to larger iphi (wraps around the barrel) 
  EBDetId incrementIphi(const EBDetId&) const ;

  /// move the nagivator to smaller iphi (wraps around the barrel)
  EBDetId decrementIphi(const EBDetId&) const;

  edm::ESHandle<CaloGeometry> theGeom_;
};

#endif
