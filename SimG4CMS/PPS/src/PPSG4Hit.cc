// -*- C++ -*-
//
// Package:     PPS
// Class  :     PPSG4Hit
//
// Implementation:
//     <Notes on implementation>
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
//

// system include files

// user include files
#include "SimG4CMS/PPS/interface/PPSG4Hit.h"
#include <iostream>

//
// constructors and destructor
//

PPSG4Hit::PPSG4Hit(): MeanPosition(0), theEntryPoint(0), theExitPoint(0) {

  elem              = 0.;
  hadr              = 0.;
  theIncidentEnergy = 0.;
  theTrackID        = -1;
  theUnitID         =  0;
  theTimeSlice      = 0.;

  theX              = 0.;
  theY              = 0.;
  theZ              = 0.;
  thePabs           = 0.;
  theTof            = 0.;
  theEnergyLoss     = 0.;
  theParticleType   = 0;
  theThetaAtEntry   = 0.;
  thePhiAtEntry     = 0.;
  theParentId       = 0;
  theVx             = 0.;
  theVy             = 0.;
  theVz             = 0.;
  thePx =0;
  thePy =0;
  thePz =0;
  theVPx =0;
  theVPy =0;
  theVPz =0;


}

PPSG4Hit::~PPSG4Hit() {}

PPSG4Hit::PPSG4Hit(const PPSG4Hit &right) {

  MeanPosition            = right.MeanPosition;

  elem              = right.elem;
  hadr              = right.hadr;
  theIncidentEnergy = right.theIncidentEnergy;
  theTrackID        = right.theTrackID;
  theUnitID         = right.theUnitID;
  theTimeSlice      = right.theTimeSlice;

  theX              = right.theX;
  theY              = right.theY;
  theZ              = right.theZ;
  thePabs           = right.thePabs;
  theTof            = right.theTof;
  theEnergyLoss     = right.theEnergyLoss;
  theParticleType   = right.theParticleType;

  theThetaAtEntry   = right.theThetaAtEntry;
  thePhiAtEntry     = right.thePhiAtEntry;
  theEntryPoint     = right.theEntryPoint;
  theExitPoint      = right.theExitPoint;
  theParentId       = right.theParentId;
  theVx             = right.theVx;
  theVy             = right.theVy;
  theVz             = right.theVz;
  thePx =right.thePx;
  thePy =right.thePy;
  thePz =right.thePz;
  theVPx =right.theVPx;
  theVPy =right.theVPy;
  theVPz =right.theVPz;

}


const PPSG4Hit& PPSG4Hit::operator=(const PPSG4Hit &right) {

  MeanPosition            = right.MeanPosition;
  elem              = right.elem;
  hadr              = right.hadr;
  theIncidentEnergy = right.theIncidentEnergy;
  theTrackID        = right.theTrackID;
  theUnitID         = right.theUnitID;
  theTimeSlice      = right.theTimeSlice;
 
  theX              = right.theX;
  theY              = right.theY;
  theZ              = right.theZ;
  thePabs           = right.thePabs;
  theTof            = right.theTof ;
  theEnergyLoss     = right.theEnergyLoss   ;
  theParticleType   = right.theParticleType ;

  theThetaAtEntry   = right.theThetaAtEntry;
  thePhiAtEntry     = right.thePhiAtEntry;
  theEntryPoint     = right.theEntryPoint;
  theExitPoint      = right.theExitPoint;
  theParentId       = right.theParentId;
  theVx             = right.theVx;
  theVy             = right.theVy;
  theVz             = right.theVz;
  thePx =right.thePx;
  thePy =right.thePy;
  thePz =right.thePz;
  theVPx =right.theVPx;
  theVPy =right.theVPy;
  theVPz =right.theVPz;



  return *this;
}

void PPSG4Hit::addEnergyDeposit(const PPSG4Hit& aHit) {

  elem += aHit.getEM();
  hadr += aHit.getHadr();
}


void PPSG4Hit::Print() {
  std::cout << (*this);
}

Hep3Vector PPSG4Hit::getEntryPoint() const           {return theEntryPoint;}
void       PPSG4Hit::setEntryPoint(Hep3Vector xyz)   { theEntryPoint    = xyz; }
Hep3Vector PPSG4Hit::getExitPoint() const           {return theExitPoint;}
void       PPSG4Hit::setExitPoint(Hep3Vector xyz)   { theExitPoint    = xyz; }

double     PPSG4Hit::getEM() const              {return elem; }
void       PPSG4Hit::setEM (double e)           { elem     = e; }
      
double     PPSG4Hit::getHadr() const            {return hadr; }
void       PPSG4Hit::setHadr (double e)         { hadr     = e; }
      
double     PPSG4Hit::getIncidentEnergy() const  {return theIncidentEnergy; }
void       PPSG4Hit::setIncidentEnergy(double e) {theIncidentEnergy  = e; }

int        PPSG4Hit::getTrackID() const         {return theTrackID; }
void       PPSG4Hit::setTrackID (int i)         { theTrackID = i; }

uint32_t   PPSG4Hit::getUnitID() const          {return theUnitID; }
void       PPSG4Hit::setUnitID (uint32_t i)     { theUnitID = i; }

double     PPSG4Hit::getTimeSlice() const       {return theTimeSlice; }
void       PPSG4Hit::setTimeSlice (double d)    { theTimeSlice = d; }
int        PPSG4Hit::getTimeSliceID() const     {return (int)theTimeSlice;}

void       PPSG4Hit::addEnergyDeposit(double em, double hd) {elem += em;  hadr += hd;}

double     PPSG4Hit::getEnergyDeposit() const   {return elem+hadr;}

float      PPSG4Hit::getPabs() const            {return thePabs;}
float      PPSG4Hit::getTof() const             {return theTof;}
float      PPSG4Hit::getEnergyLoss() const      {return theEnergyLoss;}
int        PPSG4Hit::getParticleType() const    {return theParticleType;}
float PPSG4Hit::getPx() const {return thePx;}
float PPSG4Hit::getPy() const {return thePy;}
float PPSG4Hit::getPz() const {return thePz;}
float PPSG4Hit::getVPx() const {return theVPx;}
float PPSG4Hit::getVPy() const {return theVPy;}
float PPSG4Hit::getVPz() const {return theVPz;}


void       PPSG4Hit::setPabs(float e)           {thePabs = e;}
void PPSG4Hit::setPx(float e) {thePx = e;}
void PPSG4Hit::setPy(float e) {thePy = e;}
void PPSG4Hit::setPz(float e) {thePz = e;}
void PPSG4Hit::setVPx(float e) {theVPx = e;}
void PPSG4Hit::setVPy(float e) {theVPy = e;}
void PPSG4Hit::setVPz(float e) {theVPz = e;}
void       PPSG4Hit::setTof(float e)            {theTof = e;}
void       PPSG4Hit::setEnergyLoss(float e)     {theEnergyLoss = e;}
void       PPSG4Hit::setParticleType(short i)   {theParticleType = i;}

float      PPSG4Hit::getThetaAtEntry() const    {return theThetaAtEntry;}   
float      PPSG4Hit::getPhiAtEntry() const      {return thePhiAtEntry;}

void       PPSG4Hit::setThetaAtEntry(float t)   {theThetaAtEntry = t;}
void       PPSG4Hit::setPhiAtEntry(float f)     {thePhiAtEntry = f ;}

float      PPSG4Hit::getX() const               {return theX;}
void       PPSG4Hit::setX(float t)              {theX = t;}

float      PPSG4Hit::getY() const               {return theY;}
void       PPSG4Hit::setY(float t)              {theY = t;}

float      PPSG4Hit::getZ() const               {return theZ;}
void       PPSG4Hit::setZ(float t)              {theZ = t;}

int        PPSG4Hit::getParentId() const        {return theParentId;}
void       PPSG4Hit::setParentId(int p)         {theParentId = p;}

float      PPSG4Hit::getVx() const              {return theVx;}
void       PPSG4Hit::setVx(float t)             {theVx = t;}

float      PPSG4Hit::getVy() const              {return theVy;}
void       PPSG4Hit::setVy(float t)             {theVy = t;}

float      PPSG4Hit::getVz() const              {return theVz;}
void       PPSG4Hit::setVz(float t)             {theVz = t;}

std::ostream& operator<<(std::ostream& os, const PPSG4Hit& hit) {
  os << " Data of this PPSG4Hit are:\n" 
     << " Time slice ID: " << hit.getTimeSliceID() << "\n"
     << " EnergyDeposit = " << hit.getEnergyLoss() << "\n"
     << " Energy of primary particle (ID = " << hit.getTrackID()
     << ") = " << hit.getIncidentEnergy() << " (MeV)" << "\n"
     << " Local entry and exit points in PPS unit number " << hit.getUnitID()
     << " are: " << hit.getEntryPoint() << " (mm)" << hit.getExitPoint() << " (mm)" <<"\n"
     << " Global posizion in PPS unit number " << hit.getUnitID()
     << " are: " << hit.getMeanPosition() << " (mm)" <<"\n"
     << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  return os;
}


