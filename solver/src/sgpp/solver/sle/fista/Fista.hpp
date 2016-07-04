// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef FISTA_HPP
#define FISTA_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/operation/hash/OperationMultipleEval.hpp>
#include <sgpp/solver/sle/fista/RegularizationFunction.hpp>

#include <cmath>
#include <type_traits>
#include <limits>

namespace sgpp {
namespace solver {

template <typename F>
class Fista {
  static_assert(std::is_base_of<RegularizationFunction, F>::value,
                "Template argument for fista is not a subtype of regularizationFunction!");

 public:
  Fista(F g) : g(g) {}
  void solve(base::OperationMultipleEval& op, base::DataVector& weights,
             const base::DataVector& b) {
    // Parameters for linesearch
    const double eta = 1.5;
    double L = 0.5;

    // Parameters for convergence check
    auto curMSE = std::numeric_limits<double>::max();
    auto priorMSE = 0.0;
    const double epsilon = 10e-8;
    const size_t maxIt = 1000;
    size_t curIt = 0;

    // Initial values
    base::DataVector y = weights;
    double momentum = 1;  // corresponds to t in paper

    // Variables that are reused in each iteration
    double momentumBefore = momentum;
    base::DataVector weightsBefore = base::DataVector(weights.getSize());
    auto errors = base::DataVector(b.getSize());
    auto gradient = base::DataVector(weights.getSize());

    while (isNotConverged(curIt, maxIt, priorMSE, curMSE, epsilon)) {
      const bool isFirstIteration = curIt == 0;

      priorMSE = curMSE;
      momentumBefore = momentum;
      weightsBefore = weights;

      L = L / eta;
      // Try to find the smallest L(ipschitz constant) for which our approximation works
      // First iteration is L, then L*eta.
      // Mathematically speaking, we try to find the smallest integer i \elem {1,2,...}
      // for which L*eta^i gives rise to a reasonable Lipschitz constant.
      do {
        L *= eta;
        evalErrors(op, y, b, errors);
        evalGradient(op, errors, gradient);
        curMSE = evalResidual(errors);
        weights = evalProxGrad(y, gradient, L);  // do the step!
      } while (isFirstIteration && isNotLipschitz(weights, y, op, b, L));  // F(prox) < Q_L(prox, weights)
      momentum = 0.5 * (1 + std::sqrt(1 + 4 * momentum * momentum));

      auto weightsDifference = weights;
      weightsDifference.sub(weightsBefore);
      const double multiplicator = (momentumBefore - 1) / momentum;
      weightsDifference.mult(multiplicator);
      y = weights;
      y.add(weightsDifference);

      ++curIt;
    }
  }

 private:
  F g;

  // Ax - b
  void evalErrors(base::OperationMultipleEval& op, base::DataVector weights,
                  const base::DataVector& b, base::DataVector& resultErrors) {
    op.mult(weights, resultErrors);
    resultErrors.sub(b);
  }

  // A^T(Ax-b)
  void evalGradient(base::OperationMultipleEval& op, base::DataVector& errors,
                    base::DataVector& resultGradient) {
    op.multTranspose(errors, resultGradient);
  }

  // 0.5 * || Ax -b ||^2
  double evalResidual(base::DataVector errors) {
    errors.sqr();
    return 0.5 * errors.sum();
  }

  // prox(weights - L^-1 gradf(a), lambda L^-1
  base::DataVector evalProxGrad(base::DataVector weights, base::DataVector gradient, double L) {
    const double stepsize = 1.0 / L;
    gradient.mult(stepsize);
    weights.sub(gradient);
    return g.prox(weights, stepsize);
  }

  // F(x) = f(x) + g(x)
  double evalGoal(double residual, const base::DataVector& weights) {
    const double gEval = g.eval(weights);
    return residual + gEval;
  }

  double evalUpperBound(base::OperationMultipleEval& op, const base::DataVector& x,
                        const base::DataVector& y, const base::DataVector& b, double L) {
    base::DataVector errors = base::DataVector(b.getSize());
    evalErrors(op, y, b, errors);
    const double fEval = evalResidual(errors);
    const double gEval = g.eval(x);
    auto gradient = base::DataVector(x.getSize());
    evalGradient(op, errors, gradient);
    auto x_min_y = x;
    x_min_y.sub(y);
    const double innerProd = gradient.dotProduct(x_min_y);
    const double x_min_y_l2 = (L / 2.0) * x_min_y.dotProduct(x_min_y);
    return fEval + innerProd + x_min_y_l2 + gEval;
  }

  bool isNotConverged(size_t curIteration, size_t maxIterations, double priorMSE, double curMSE,
                      double epsilon) {
    const bool isGoodEnough = std::abs(priorMSE - curMSE) < epsilon;
    const bool isFinalStep = curIteration >= maxIterations;
    return !(isGoodEnough || isFinalStep);
  }

  bool isNotLipschitz(const base::DataVector& weights, const base::DataVector& y,
                      base::OperationMultipleEval& op, const base::DataVector& b, double L) {
    base::DataVector errors = base::DataVector(b.getSize());
    evalErrors(op, weights, b, errors);
    const double residual = evalResidual(errors);
    const double goalResult = evalGoal(residual, weights);
    const double upperBoundResult = evalUpperBound(op, weights, y, b, L);
    return goalResult > upperBoundResult;
  }
};

}  // namespace solver
}  // namepsace sgpp

#endif  // FISTA_HPP
