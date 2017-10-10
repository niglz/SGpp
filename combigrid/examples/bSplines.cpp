// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/operation/CombigridMultiOperation.hpp>
#include <sgpp/combigrid/operation/CombigridOperation.hpp>
#include <sgpp/combigrid/operation/Configurations.hpp>
#include <sgpp/combigrid/operation/multidim/AveragingLevelManager.hpp>
#include <sgpp/combigrid/operation/multidim/WeightedRatioLevelManager.hpp>
#include <sgpp/combigrid/storage/FunctionLookupTable.hpp>
#include <sgpp/combigrid/storage/tree/CombigridTreeStorage.hpp>
#include <sgpp/combigrid/utils/Stopwatch.hpp>
#include <sgpp/combigrid/utils/Utils.hpp>

#include <sgpp/combigrid/operation/multidim/fullgrid/SLECoefficientsStorage.hpp>
#include <sgpp/optimization/sle/solver/Auto.hpp>
#include <sgpp/optimization/sle/system/FullSLE.hpp>
#include <sgpp/optimization/tools/Printer.hpp>
#include <sgpp/quadrature/sampling/NaiveSampleGenerator.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

// double f(sgpp::base::DataVector const &v) {
//  //  return v[0] * v[0] * v[0] + v[0] * v[0] * v[1] + v[0] * v[1] * v[1] + v[1] * v[1] * v[1];
//  return sin(1. / (1. + v[0] * v[0])) * v[1];
//}

double f(sgpp::base::DataVector const &x) {
  return std::atan(50 * (x[0] - .35)) + M_PI / 2 + 4 * std::pow(x[1], 3) +
         std::exp(x[0] * x[1] - 1);
}

double interpolate(size_t maxlevel) {
  size_t numDimensions = 2;
  sgpp::combigrid::MultiFunction func(f);

  auto operation = sgpp::combigrid::CombigridOperation::createExpUniformBsplineInterpolation(
      numDimensions, func);

  //  auto operation =
  //      sgpp::combigrid::CombigridOperation::createExpClenshawCurtisPolynomialInterpolation(
  //          numDimensions, func);

  double diff = 0;
  double max_err = 0;
  // generator generates num_points random points in [0,1]^dim
  size_t num_points = 1000;
  sgpp::quadrature::NaiveSampleGenerator generator(numDimensions);
  sgpp::base::DataVector p(numDimensions, 0);

  for (size_t i = 0; i < num_points; i++) {
    generator.getSample(p);
    diff = fabs(operation->evaluate(maxlevel, p) - f(p));
    max_err = (diff > max_err) ? diff : max_err;
  }

  std::cout << "# grid points: " << operation->numGridPoints() << " ";

  return max_err;
}

int main() {
  sgpp::base::SGppStopwatch watch;
  watch.start();
  size_t maxLevel = 8;
  std::vector<double> err(maxLevel + 1, 0);
  for (size_t l = 0; l < maxLevel + 1; l++) {
    err[l] = interpolate(l);
    std::cout << l << " " << err[l] << std::endl;
  }
  std::cout << " Total Runtime: " << watch.stop() << " s" << std::endl;
  return 0;
}