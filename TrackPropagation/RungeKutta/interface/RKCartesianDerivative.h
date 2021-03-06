#ifndef RKCartesianDerivative_H
#define RKCartesianDerivative_H

class CartesianState;

class RKCartesianDerivative {
public:
 
  virtual ~RKCartesianDerivative() {}

  virtual CartesianState operator()( double step, const CartesianState& start) const = 0;

};

#endif
