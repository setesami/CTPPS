#ifndef Fireworks_FWGenericHandle_h
#define Fireworks_FWGenericHandle_h
// -*- C++ -*-
//
// Package:     Framework
// Class  :     FWGenericHandle
// 
/**\class FWGenericHandle FWGenericHandle.h Fireworks/Core/interface/FWGenericHandle.h

 Description: Allows interaction with data in the Event without actually using 
              the C++ class. Ported to work with 

 Usage:
    This is a rip-off of edm::FWGenericHandle. I extended it to work with 
    edm::EventBase as well.

    The FWGenericHandle allows one to get data back from the edm::EventBase as 
    a Reflex::Object instead of as the actual C++ class type.

    //make a handle to hold an instance of 'MyClass'
    edm::FWGenericHandle myHandle("MyClass");
    
    event.getByLabel("mine",myHandle);
    
    //call the print method of 'MyClass' instance
    myHandle->invoke("print);  
*/
//
// Original Author:  Chris Jones
//         Created:  Sat Jan  7 15:40:43 EST 2006
// $Id: FWGenericHandle.h,v 1.17 2009/11/04 15:27:19 wmtan Exp $
//

// system include files
#include <string>

// user include files
#include "Reflex/Object.h"
#include "FWCore/Common/interface/EventBase.h"
#include "FWCore/Utilities/interface/UseReflex.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/ConvertHandle.h"

// forward declarations
namespace edm {
   ///This class is just a 'tag' used to allow a specialization of edm::Handle
struct FWGenericObject
{
};

template<>
class Handle<FWGenericObject> {
public:
      ///Throws exception if iName is not a known C++ class type
      Handle(std::string const& iName) : 
        type_(Reflex::Type::ByName(iName)), prod_(), prov_(0) {
           if(type_ == Reflex::Type()) {
              Exception::throwThis(errors::NotFound,
                "Handle<FWGenericObject> told to use uknown type '",
                iName.c_str(),
                "'.\n Please check spelling or that a module uses this type in the job.");
           }
           if(type_.IsTypedef()){
              //For a 'Reflex::Typedef' the 'toType' method returns the actual type
              // this is needed since you are now allowed to 'invoke' methods of a 'Typedef'
              // only for a 'real' class
              type_ = type_.ToType();
           }
        }
   
   ///Throws exception if iType is invalid
   Handle(Reflex::Type const& iType):
      type_(iType), prod_(), prov_(0) {
         if(iType == Reflex::Type()) {
            Exception::throwThis(errors::NotFound, "Handle<FWGenericObject> given an invalid Reflex::Type");
         }
         if(type_.IsTypedef()){
            //For a 'Reflex::Typedef' the 'toType' method returns the actual type
            // this is needed since you are now allowed to 'invoke' methods of a 'Typedef'
            // only for a 'real' class
            type_ = type_.ToType();
         }
      }
   
   Handle(Handle<FWGenericObject> const& h):
   type_(h.type_),
   prod_(h.prod_),
   prov_(h.prov_),
   whyFailed_(h.whyFailed_)
   { }
   
   Handle(Reflex::Object const& prod, Provenance const* prov, ProductID const& pid):
   type_(prod.TypeOf()),
   prod_(prod),
   prov_(prov) { 
      assert(prod_);
      assert(prov_);
      // assert(prov_->productID() != ProductID());
   }
   
      //~Handle();
      
   void swap(Handle<FWGenericObject>& other)
   {
      // use unqualified swap for user defined classes
      using std::swap;
      swap(type_, other.type_);
      std::swap(prod_, other.prod_);
      swap(prov_, other.prov_);
      swap(whyFailed_, other.whyFailed_);
   }
   
   
   Handle<FWGenericObject>& operator=(Handle<FWGenericObject> const& rhs)
   {
      Handle<FWGenericObject> temp(rhs);
      this->swap(temp);
      return *this;
   }
   
   bool isValid() const {
      return prod_ && 0!= prov_;
   }

   bool failedToGet() const {
     return 0 != whyFailed_.get();
   }
   Reflex::Object const* product() const { 
     if(this->failedToGet()) { 
       whyFailed_->raise();
     } 
     return &prod_;
   }
   Reflex::Object const* operator->() const {return this->product();}
   Reflex::Object const& operator*() const {return *(this->product());}
   
   Reflex::Type const& type() const {return type_;}
   Provenance const* provenance() const {return prov_;}
   
   ProductID id() const {return prov_->productID();}

   void clear() { prov_ = 0; whyFailed_.reset();}
      
   void setWhyFailed(boost::shared_ptr<cms::Exception> const& iWhyFailed) {
    whyFailed_=iWhyFailed;
  }
private:
   Reflex::Type type_;
   Reflex::Object prod_;
   Provenance const* prov_;    
   boost::shared_ptr<cms::Exception> whyFailed_;
};

typedef Handle<FWGenericObject> FWGenericHandle;

///specialize this function forFWGenericHandle
void convert_handle(BasicHandle const& orig,
                    Handle<FWGenericObject>& result);


///Specialize the Event's getByLabel method to work with a Handle<FWGenericObject>
template <>
bool
edm::EventBase::getByLabel(edm::InputTag const& tag, Handle<FWGenericObject>& result) const;   

}
#endif
