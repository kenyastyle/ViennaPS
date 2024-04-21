#pragma once

#include "psDomain.hpp"
#include "psSmartPointer.hpp"

template <typename NumericType, int D> class psGeometricModel {
protected:
  psSmartPointer<psDomain<NumericType, D>> domain = nullptr;

public:
  virtual ~psGeometricModel() = default;

  void setDomain(psSmartPointer<psDomain<NumericType, D>> passedDomain) {
    domain = passedDomain;
  }

  virtual void apply() {}
};
