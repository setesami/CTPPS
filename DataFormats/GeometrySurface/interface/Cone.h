#ifndef Geom_Cone_H
#define Geom_Cone_H

/** \class Cone
 *
 *  A Cone.
 *
 *  \warning Surfaces are reference counted, so only ReferenceCountingPointer
 *  should be used to point to them. For this reason, they should be 
 *  using the static build() method. 
 *  (The normal constructors will become private in the future).
 *
 *  $Date: 2007/01/17 20:58:43 $
 *  $Revision: 1.1 $
 */

#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "boost/intrusive_ptr.hpp" 

class Cone : public virtual Surface {
public:
  typedef ReferenceCountingPointer<Cone> ConePointer;
  typedef ReferenceCountingPointer<Cone> ConstConePointer;


  /// Construct a cone with the specified vertex and opening angle.
  /// The reference frame is defined by pos and rot;
  /// the cone axis is parallel to the local Z axis.
  static ConePointer build(const PositionType& pos,
			   const RotationType& rot,
			   const PositionType& vert,
			   Geom::Theta<Scalar> angle) {
    return ConePointer(new Cone(pos, rot, vert, angle));
  }


  // -- DEPRECATED CONSTRUCTOR

  /// Do not use this constructor directly; use the static build method,
  /// which returns a ReferenceCountingPointer.
  /// This constructor will soon become private
  Cone( const PositionType& pos, const RotationType& rot, 
	const PositionType& vert, Geom::Theta<Scalar> angle) :
    Surface( pos, rot), theVertex(vert), theAngle(angle) {}


  // -- Extension of Surface interface for cone

  /// Global position of the cone vertex
  GlobalPoint vertex() const {return theVertex;}

  /// Angle of the cone
  Geom::Theta<float> openingAngle() const {return theAngle;}


  // -- Implementation of Surface interface    

  virtual Side side( const LocalPoint& p, Scalar tolerance) const {return side( toGlobal(p), tolerance);}
  virtual Side side( const GlobalPoint& p, Scalar tolerance) const;

  // Tangent plane to surface from global point
  virtual ReferenceCountingPointer<TangentPlane> tangentPlane (const GlobalPoint&) const;
  // Tangent plane to surface from local point
  virtual ReferenceCountingPointer<TangentPlane> tangentPlane (const LocalPoint&) const;

protected:
  // Private constructor - use build() instead
  Cone(const PositionType& pos, const RotationType& rot,
       const PositionType& vert, Geom::Theta<Scalar> angle,
       MediumProperties* mp) : 
    Surface(pos, rot, mp), theVertex(vert), theAngle(angle) {}

private:

  GlobalPoint             theVertex;
  Geom::Theta<Scalar>     theAngle;

};

#endif
