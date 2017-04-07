// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/finance/basis/linearstretched/noboundary/algorithm_sweep/XPhidPhiUpBBLinearStretched.hpp>

#include <sgpp/globaldef.hpp>

namespace sgpp {
namespace finance {

XPhidPhiUpBBLinearStretched::XPhidPhiUpBBLinearStretched(sgpp::base::GridStorage* storage)
    : storage(storage), stretching(storage->getStretching()) {}

XPhidPhiUpBBLinearStretched::~XPhidPhiUpBBLinearStretched() {}

void XPhidPhiUpBBLinearStretched::operator()(sgpp::base::DataVector& source,
                                             sgpp::base::DataVector& result, grid_iterator& index,
                                             size_t dim) {
  // get boundary values
  double fl = 0.0;
  double fr = 0.0;

  rec(source, result, index, dim, fl, fr);
}

void XPhidPhiUpBBLinearStretched::rec(sgpp::base::DataVector& source,
                                      sgpp::base::DataVector& result, grid_iterator& index,
                                      size_t dim, double& fl, double& fr) {
  size_t seq = index.seq();

  fl = fr = 0.0;
  double fml = 0.0;
  double fmr = 0.0;

  sgpp::base::level_t current_level;
  sgpp::base::index_t current_index;

  if (!index.hint()) {
    index.leftChild(dim);

    if (!storage->isInvalidSequenceNumber(index.seq())) {
      rec(source, result, index, dim, fl, fml);
      //      recBB(source, result, index, dim, fl, fml, q, t);
    }

    index.stepRight(dim);

    if (!storage->isInvalidSequenceNumber(index.seq())) {
      rec(source, result, index, dim, fmr, fr);
      //      recBB(source, result, index, dim, fmr, fr, q, t);
    }

    index.up(dim);
  }

  index.get(dim, current_level, current_index);
  // get the positions of the current index as well as its left and right neighbors
  double posl = 0, posr = 0, posc = 0;
  this->stretching->getAdjacentPositions(static_cast<int>(current_level),
                                         static_cast<int>(current_index), dim, posc, posl, posr);
  double baseLength = posr - posl;
  double leftLength = posc - posl;
  double rightLength = posr - posc;

  double fm = fml + fmr;

  double alpha_value = source[seq];

  // transposed operations:
  result[seq] = fm;

  double c = -1.0 / 6.0 * (posc + posl + posr);
  fl = (c * alpha_value) + fl + fm * (rightLength / baseLength);
  fr = (-c * alpha_value) + fr + fm * (leftLength / baseLength);
}

}  // namespace finance
}  // namespace sgpp
