// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef SGPP_OPTIMIZATION_SLE_SOLVER_GMMPP_HPP
#define SGPP_OPTIMIZATION_SLE_SOLVER_GMMPP_HPP

#include <sgpp/globaldef.hpp>
#include <sgpp/optimization/sle/solver/SLESolver.hpp>

#include <vector>

namespace sgpp {
namespace optimization {
namespace sle_solver {

/**
 * Linear system solver using Gmm++ (iterative sparse solver).
 */
class Gmmpp : public SLESolver {
 public:
  /**
   * Destructor.
   */
  ~Gmmpp() override;

  /**
   * @param       system  system to be solved
   * @param       b       right-hand side
   * @param[out]  x       solution to the system
   * @return              whether all went well
   *                      (false if errors occurred)
   */
  bool solve(SLE& system, base::DataVector& b, base::DataVector& x) const override;
};
}  // namespace sle_solver
}  // namespace optimization
}  // namespace sgpp

#endif /* SGPP_OPTIMIZATION_SLE_SOLVER_GMMPP_HPP */
