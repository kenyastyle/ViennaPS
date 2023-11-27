#pragma once

#include <lsMaterialMap.hpp>
#include <psSmartPointer.hpp>

enum class psMaterial : int {
  Undefined = -1,
  Mask = 0,
  Si = 1,
  SiO2 = 2,
  Si3N4 = 3,
  SiN = 4,
  SiON = 5,
  SiC = 6,
  SiGe = 7,
  PolySi = 8,
  GaN = 9,
  W = 10,
  Al2O3 = 11,
  TiN = 12,
  Cu = 13,
  Polymer = 14,
  Dielectric = 15,
  Metal = 16,
  Air = 17,
  GAS = 18
};

class psMaterialMap {
  psSmartPointer<lsMaterialMap> map;

public:
  psMaterialMap() { map = psSmartPointer<lsMaterialMap>::New(); };

  void insertNextMaterial(psMaterial material = psMaterial::Undefined) {
    map->insertNextMaterial(static_cast<int>(material));
  }

  psMaterial getMaterialAtIdx(std::size_t idx) const {
    if (idx >= map->getNumberOfLayers())
      return psMaterial::GAS;
    int matId = map->getMaterialId(idx);
    return mapToMaterial(matId);
  }

  psSmartPointer<lsMaterialMap> getMaterialMap() const { return map; }

  std::size_t size() const { return map->getNumberOfLayers(); }

  static inline psMaterial mapToMaterial(const int matId) {
    if (matId > 17 || matId < -1)
      return psMaterial::Undefined;
    return static_cast<psMaterial>(matId);
  }

  template <class T> static inline psMaterial mapToMaterial(const T matId) {
    return mapToMaterial(static_cast<int>(matId));
  }

  template <class T>
  static inline bool isMaterial(const T matId, const psMaterial material) {
    return mapToMaterial(matId) == material;
  }
};
