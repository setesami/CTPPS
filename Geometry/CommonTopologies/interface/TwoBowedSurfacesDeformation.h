#ifndef GeometryCommonTopologiesTwoBowedSurfacesDeformation_H
#define GeometryCommonTopologiesTwoBowedSurfacesDeformation_H

/// TwoBowedSurfacesAlignmentParameters
///
/// Class to apply corrections to local positions resulting
/// from two surfaces chained in local y. Possible bows are
/// parametrised using Legendre polynomials up to second order,
/// excluding 0th and 1st order that are already treated by
/// local w shift and rotations around local u and v axes.
/// In addition store relative shifts and rotations of the
/// two surfaces.
///
///  \author    : Gero Flucke
///  date       : October 2010
///  $Revision: 1.1 $
///  $Date: 2010/10/26 19:00:00 $
///  (last update by $Author: flucke $)

#include "Geometry/CommonTopologies/interface/SurfaceDeformation.h"

// already included in the above:
// #include <vector>

class TwoBowedSurfacesDeformation : public SurfaceDeformation
{
 public:
  /// Constructor from vector of parameters, its size() must be
  /// between minParameterSize() and maxParameterSize().
  /// The parameters are (in that order)
  /// - mean sagittaX  of both surfaces
  /// - mean sagittaXY 
  /// - mean sagittaY  
  /// - 'delta' u of both surfaces (shift surface at lower/higher y by +u/-u) 
  /// - 'delta' v of both surfaces
  /// - 'delta' w of both surfaces
  /// - 'delta' alpha of both surfaces (rotate around local x)
  /// - 'delta' beta
  /// - 'delta' gamma
  /// - 'delta' sagittaX  of both surfaces (add/subtract for surfaces at lower/higher y)
  /// - 'delta' sagittaXY
  /// - 'delta' sagittaY
  /// - ySplit: y-value where surfaces are split
  TwoBowedSurfacesDeformation(const std::vector<double> &parameters);

  virtual TwoBowedSurfacesDeformation* clone() const;

  /// specific type, i.e. SurfaceDeformationFactory::kTwoBowedSurfaces
  virtual int type() const;

  /// correction to add to local position depending on 
  /// - track parameters in local frame (from LocalTrajectoryParameters):
  ///   * track position as Local2DPoint(x,y)
  ///   * track angles   as LocalTrackAngles(dxdz, dydz)
  /// - length of surface (local y-coordinate)
  /// - width of surface (local x-coordinate)
  virtual Local2DVector positionCorrection(const Local2DPoint &localPos,
                                           const LocalTrackAngles &localAngles,
                                           double length, double width) const;

  /// update information with parameters of 'other',
  /// false in case the type or some parameters do not match and thus
  /// the information cannot be used (then no changes are done),
  /// true if merge was successful
  virtual bool add(const SurfaceDeformation &other);
  
  /// parameters - see constructor for meaning
  virtual std::vector<double> parameters() const;

  /// minimum size of vector that is accepted by constructor from vector
  static unsigned int minParameterSize() { return 13;}
  /// maximum size of vector that is accepted by constructor from vector
  static unsigned int maxParameterSize() { return 13;}


 private:
  std::vector<double> theParameters;
};

#endif
