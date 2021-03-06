// -*- C++ -*-
//
// Package:     Framework
// Class  :     GenericObjectOwner
// 
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Thu Feb  7 17:21:22 EST 2008
//

// system include files

// user include files
#include "FWCore/Framework/interface/GenericObjectOwner.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Utilities/interface/WrappedClassName.h"

namespace edm {
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
//GenericObjectOwner::GenericObjectOwner()
//{
//}

// GenericObjectOwner::GenericObjectOwner(const GenericObjectOwner& rhs)
// {
//    // do actual copying here;
// }

GenericObjectOwner::~GenericObjectOwner()
{
   if(m_ownData) {
      m_object.Destruct();
   }
}

//
// assignment operators
//
// const GenericObjectOwner& GenericObjectOwner::operator=(const GenericObjectOwner& rhs)
// {
//   //An exception safe implementation is
//   GenericObjectOwner temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//
void 
GenericObjectOwner::swap(GenericObjectOwner& iOther) {
   Reflex::Object old(m_object);
   m_object = iOther.m_object;
   iOther.m_object = m_object;
}

//
// const member functions
//
Reflex::Object 
GenericObjectOwner::object() const {
   return m_object;
}

//
// static member functions
//

template <>
OrphanHandle<GenericObjectOwner> 
Event::put<GenericObjectOwner>(std::auto_ptr<GenericObjectOwner> product, std::string const& productInstanceName)
{
   if (product.get() == 0) {                // null pointer is illegal
      throw edm::Exception(edm::errors::NullPointerError)
      << "Event::put: A null auto_ptr was passed to 'put'.\n"
      << "The pointer is of type " << "GenericObjectOwner" << ".\n"
      << "The specified productInstanceName was '" << productInstanceName << "'.\n";
   }
   
   // The following will call post_insert if T has such a function,
   // and do nothing if T has no such function.
   /*
   typename boost::mpl::if_c<detail::has_postinsert<PROD>::value, 
   DoPostInsert<PROD>, 
   DoNotPostInsert<PROD> >::type maybe_inserter;
   maybe_inserter(product.get());
   */
   ConstBranchDescription const& desc =
   provRecorder_.getBranchDescription(TypeID(product->object().TypeOf().TypeInfo()), productInstanceName);
   
   Reflex::Type const wrapperType=Reflex::Type::ByName(wrappedClassName(desc.fullClassName()));
   if(wrapperType == Reflex::Type() ) {
      throw edm::Exception(errors::DictionaryNotFound, "NoWrapperDictionary")
      << "Event::put: the class type '" << desc.fullClassName()
      << "' was passed to put but the Reflex dictionary for the required class '"
      << wrappedClassName(desc.fullClassName()) << "' could not be found./n"
      << "Please change the C++ package which contains the description of '" << desc.fullClassName()
      << "' so that the required class also has a dictionary autogenerated.";
   }
   std::vector<void*> args;
   args.reserve(1);
   args.push_back(product->object().Address());
   //generate the signature of the function we want to call
   std::string s("void(");
   s+=desc.fullClassName();
   s+="*)";
   Reflex::Type ptrT = Reflex::Type::ByName(s);
   Reflex::Object oWrapper(wrapperType.Construct(ptrT,args));
   //ownership was transfered to the wrapper
   product.release();

   static Reflex::Type s_edproductType(Reflex::Type::ByTypeInfo(typeid(EDProduct)));
   EDProduct *wp(reinterpret_cast<EDProduct*>(oWrapper.CastObject(s_edproductType).Address()));
   putProducts().push_back(std::make_pair(wp, &desc));
   
   // product.release(); // The object has been copied into the Wrapper.
   // The old copy must be deleted, so we cannot release ownership.
   
   return(OrphanHandle<GenericObjectOwner>(oWrapper.Get("obj"), eventPrincipal().branchIDToProductID(desc.branchID())));
}
}

