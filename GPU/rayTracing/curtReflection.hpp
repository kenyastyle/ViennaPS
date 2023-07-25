#pragma once

#include <curand.h>

#include <curtBoundary.hpp>
#include <curtPerRayData.hpp>
#include <curtSBTRecords.hpp>

#include <utGDT.hpp>

#ifdef __CUDACC__
static __device__ __forceinline__ void specularReflection(PerRayData *prd) {
  const HitSBTData *sbtData = (const HitSBTData *)optixGetSbtDataPointer();
  const gdt::vec3f geoNormal = computeNormal(sbtData, optixGetPrimitiveIndex());
  prd->pos = prd->pos + optixGetRayTmax() * prd->dir;
  prd->dir = prd->dir - (2 * gdt::dot(prd->dir, geoNormal)) * geoNormal;
}

static __device__ __forceinline__ void
specularReflection(PerRayData *prd, const gdt::vec3f &geoNormal) {
  prd->pos = prd->pos + optixGetRayTmax() * prd->dir;
  prd->dir = prd->dir - (2 * gdt::dot(prd->dir, geoNormal)) * geoNormal;
}

static __device__ void conedCosineReflection(PerRayData *prd,
                                             const float avgReflAngle,
                                             const gdt::vec3f &geomNormal) {
  // Calculate specular direction
  specularReflection(prd, geomNormal);

  float u, sqrt_1m_u;
  float angle;
  gdt::vec3f randomDir;

  //  loop until ray is reflected away from the surface normal
  //  this loop takes care of the case where part of the cone points
  //  into the geometry
  do {
    do { // generate a random angle between 0 and specular angle
      u = sqrtf(getNextRand(&prd->RNGstate));
      sqrt_1m_u = sqrtf(1. - u);
      angle = avgReflAngle * sqrt_1m_u;
    } while (getNextRand(&prd->RNGstate) * angle * u >
             cosf(PI_F / 2. * sqrt_1m_u) * sinf(angle));

    // Random Azimuthal Rotation
    float costheta = max(min(cos(angle), 1.), 0.);
    float cosphi, sinphi;
    float temp;

    do {
      cosphi = getNextRand(&prd->RNGstate) - 0.5;
      sinphi = getNextRand(&prd->RNGstate) - 0.5;
      temp = cosphi * cosphi + sinphi * sinphi;
    } while (temp >= 0.25 || temp <= 1e-6f);

    // Rotate
    float a0;
    float a1;

    if (abs(prd->dir[0]) <= abs(prd->dir[1])) {
      a0 = prd->dir[0];
      a1 = prd->dir[1];
    } else {
      a0 = prd->dir[1];
      a1 = prd->dir[0];
    }

    temp = sqrtf(max(1. - costheta * costheta, 0.) / (temp * (1. - a0 * a0)));
    sinphi *= temp;
    cosphi *= temp;
    temp = costheta + a0 * sinphi;

    randomDir[0] = a0 * costheta - (1. - a0 * a0) * sinphi;
    randomDir[1] = a1 * temp + prd->dir[2] * cosphi;
    randomDir[2] = prd->dir[2] * temp - a1 * cosphi;

    if (a0 != prd->dir[0]) {
      temp = randomDir[0];
      randomDir[0] = randomDir[1];
      randomDir[1] = temp;
    }
  } while (gdt::dot(randomDir, geomNormal) <= 0.);

  prd->dir = randomDir;
}

static __device__ gdt::vec3f PickRandomPointOnUnitSphere(curtRNGState *state) {
  float x, y, z, x2py2;
  do {
    x = 2.f * curand_uniform(state) - 1.f;
    y = 2.f * curand_uniform(state) - 1.f;
    x2py2 = x * x + y * y;
  } while (x2py2 >= 1.);
  float tmp = 2.f * sqrtf(1.f - x2py2);
  x *= tmp;
  y *= tmp;
  z = 1.f - 2.f * x2py2;
  return gdt::vec3f(x, y, z);
}

static __device__ void diffuseReflection(PerRayData *prd) {
  const gdt::vec3f randomDirection =
      PickRandomPointOnUnitSphere(&prd->RNGstate);

  const HitSBTData *sbtData = (const HitSBTData *)optixGetSbtDataPointer();
  const gdt::vec3f geoNormal = computeNormal(sbtData, optixGetPrimitiveIndex());
  prd->pos = prd->pos + optixGetRayTmax() * prd->dir;

  prd->dir = geoNormal + randomDirection;
  prd->dir = gdt::normalize(prd->dir);
}
#endif