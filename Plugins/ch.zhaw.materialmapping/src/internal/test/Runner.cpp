#include <mitkLogMacros.h>

#include "Runner.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace Testing;

void Runner::runUnitTests() {
    Catch::Session().run();
}

void Runner::compareGrids(mitk::UnstructuredGrid::Pointer, mitk::UnstructuredGrid::Pointer) {
    MITK_INFO("TESTING") << "comparing unstructured grid...";
}