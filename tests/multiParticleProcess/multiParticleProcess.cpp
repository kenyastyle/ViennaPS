#include <geometries/psMakeTrench.hpp>
#include <models/psMultiParticleProcess.hpp>

#include <lsTestAsserts.hpp>
#include <psDomain.hpp>
#include <psProcess.hpp>
#include <vcTestAsserts.hpp>

namespace viennacore {

using namespace viennaps;

template <class NumericType, int D> void RunTest() {
  Logger::setLogLevel(LogLevel::WARNING);

  {
    auto domain = SmartPointer<Domain<NumericType, D>>::New();
    MakeTrench<NumericType, D>(domain, 1., 10., 10., 2.5, 5., 10., 1., false,
                               true, Material::Si)
        .apply();
    auto model = SmartPointer<MultiParticleProcess<NumericType, D>>::New();

    VC_TEST_ASSERT(model->getSurfaceModel());
    VC_TEST_ASSERT(model->getVelocityField());
    VC_TEST_ASSERT(model->getVelocityField()->getTranslationFieldOptions() ==
                   2);

    model->addParticle(1., 1., false, "particleFlux");
    VC_TEST_ASSERT(model->getParticleTypes().size() == 1);

    // Process<NumericType, D>(domain, model, 2.).apply();

    // VC_TEST_ASSERT(domain->getLevelSets().size() == 2);
    // VC_TEST_ASSERT(domain->getMaterialMap());
    // VC_TEST_ASSERT(domain->getMaterialMap()->size() == 2);
    // LSTEST_ASSERT_VALID_LS(domain->getLevelSets().back(), NumericType, D);
  }
}

} // namespace viennacore

int main() { VC_RUN_ALL_TESTS }
