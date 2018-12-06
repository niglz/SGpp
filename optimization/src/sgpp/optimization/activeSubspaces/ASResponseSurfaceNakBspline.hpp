// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <sgpp/base/exception/algorithm_exception.hpp>
#include <sgpp/base/grid/generation/functors/SurplusRefinementFunctor.hpp>
#include <sgpp/base/grid/type/NakBsplineBoundaryGrid.hpp>
#include <sgpp/base/grid/type/NakBsplineExtendedGrid.hpp>
#include <sgpp/base/grid/type/NakBsplineGrid.hpp>
#include <sgpp/base/grid/type/NakBsplineModifiedGrid.hpp>
#include <sgpp/base/operation/hash/common/basis/NakBsplineBasis.hpp>
#include <sgpp/base/operation/hash/common/basis/NakBsplineBoundaryBasis.hpp>
#include <sgpp/base/operation/hash/common/basis/NakBsplineModifiedBasis.hpp>
#include <sgpp/base/tools/SGppStopwatch.hpp>
#include <sgpp/optimization/activeSubspaces/ASResponseSurface.hpp>
#include <sgpp/optimization/activeSubspaces/EigenFunctionalities.hpp>
#include <sgpp/optimization/activeSubspaces/NakBsplineScalarProducts.hpp>
#include <sgpp/optimization/activeSubspaces/ResponseSurface.hpp>
#include <sgpp/optimization/function/scalar/InterpolantScalarFunction.hpp>
#include <sgpp/optimization/sle/solver/Armadillo.hpp>
#include <sgpp/optimization/sle/solver/Auto.hpp>
#include <sgpp/optimization/sle/system/HierarchisationSLE.hpp>
#include <sgpp/optimization/tools/HaltonSequence.hpp>
#include <sgpp/optimization/tools/SobolSequence.hpp>

#include <limits>
#include <string>
#include <vector>

namespace sgpp {
namespace optimization {

/**
 * Reduced response surface on the active subspace of an objective function. The response surface
 * is based on not a knot B-spline basis functions.
 */
class ASResponseSurfaceNakBspline : public ASResponseSurface {
 public:
  /**
   * Constructor
   *
   * @param W1		eigenvectors defining the active subspace
   * @param gridType	type of the basis
   * @param degree	degree of the basis
   */
  ASResponseSurfaceNakBspline(Eigen::MatrixXd W1, sgpp::base::GridType gridType, size_t degree = 3)
      : ASResponseSurface(W1), gridType(gridType), degree(degree) {
    initialize();
  }

  /**
   * sets grid and basis according to gridType and W1
   */
  void initialize();

  /**
   * creates a regular grid of the dimension of the reduced space ( = # columns of W1)
   * then performs regression for the B-spline coefficients with the given evaluationPoints and
   * functionValues. These usually come from the detection of the active  subspace
   *
   * @param evaluationPoints	set of points
   * @param functionValues		the objective function evaluated at evaluationPoints
   * @param level				level of the regular interpolant
   */
  void createRegularReducedSurfaceFromDetectionPoints(sgpp::base::DataMatrix evaluationPoints,
                                                      sgpp::base::DataVector functionValues,
                                                      size_t level);

  /**
   * creates a regular grid of the dimension of the reduced space ( = # columns of W1)
   * then interpolates in these points. The function values are calculated by using the
   * Moore-Penrose (pseudo-) inverse of W1, pinvW1. So for a point x the right hand side is
   * f(pinvW1 x)
   *
   * @param level				level of the regular interpolant
   * @param objectiveFunction	the objective function
   */
  void createRegularReducedSurfaceWithPseudoInverse(
      size_t level, std::shared_ptr<sgpp::optimization::ScalarFunction> objectiveFunc);

  /**
   * creates a surplus adaptive grid of the dimension of the reduced space ( = # columns of W1)
   * then interpolates in these points. The function values are calculated by using the
   * Moore-Penrose (pseudo-) inverse of W1, pinvW1. So for a point x the right hand side is
   * f(pinvW1 x)
   *
   * @param maxNumGridPoints	maximum number of grid points
   * @param objectiveFunc		the objective function
   * @param initialLevel		initial regular level of the interpolant. From then on the
   * grid is created adaptively.
   */
  void createAdaptiveReducedSurfaceWithPseudoInverse(
      size_t maxNumGridPoints, std::shared_ptr<sgpp::optimization::ScalarFunction> objectiveFunc,
      size_t initialLevel = 1);

  /**
   * evalauted the reduced response surface in a point v from the original parameter space. I.e. it
   * evaluates the response surface in W1T*v
   *
   * @param v	point in the original parameter space
   * @return	evaluation at the (transformed) point
   */
  double eval(sgpp::base::DataVector v);

  /**
   * evaluates the reduced response surface's gradient in a point v from the original parameter
   * space. I.e. it evaluates the response surface's gradient in W1T*v
   *
   * @param v			point in the original parameter space
   * qparam gradient 	reference to return the gradient evaluated at the (transformed) point
   * @return			evaluation at the (transformed) point
   */
  double evalGradient(sgpp::base::DataVector v, sgpp::base::DataVector& gradient);

  /**
   * Calculates an approximation for the integral of the objective function using Monte carlo
   * points. The area orthogonal to the active subspace is approximated with a Monte Carlo Histogram
   *
   * @param numMCPoints				number of Monte Carlo points for the integration
   * @param numHistogramMCPoints	number of Monte Carlo points for the creation of the
   * histogram
   * @param pointStrategy			strategy how the Monte Carlo points are chosen. 'MC'
   * for uniform random, 'Halton' and 'Sobol' for the according Quasi Monte Carlo point sequences
   *
   * @return						the integral
   */
  double getMCIntegral(size_t numMCPoints = 1000, size_t numHistogramMCPoints = 1000000,
                       std::string pointStrategy = "MC");

  /**
   * Calculates the integral using a B-spline interpolant for the area orthogonal to the active
   * subspace. The interpolant is defined on a regular grid and uses a Monte Carlo Histogram to
   * evaluate the area orthogonal to the active subspace.
   *
   *@param level					 level of the regular grid
   *@param numHistogramPoints number of Monte Carlo points for the creation of the histogram
   *@param pointStrategy			 strategy how the Monte Carlo points are chosen.
   *'MC' for uniform random, 'Halton' and 'Sobol' for the according Quasi Monte Carlo point
   *sequences uniform uniform @return the integral
   */
  double getContinuousIntegral(size_t level, size_t numHistogramMCPoints,
                               std::string pointStrategy = "MC");

 private:
  // grid Type of the response surface
  sgpp::base::GridType gridType;
  // degree of the response surfaces basis
  size_t degree;
  // dimension of the active subspace
  size_t activeDim;
  // the grid of the response surface
  std::shared_ptr<sgpp::base::Grid> grid;
  // the coefficients of the response surface
  sgpp::base::DataVector coefficients;
  // the basis of the response surface
  std::shared_ptr<sgpp::base::SBasis> basis;
  // if the active subspace is [rightBound1D, leftBound1D]. These depend on how W1 rotates the unit
  // hypercube.
  double rightBound1D = 1.0;
  double leftBound1D = 0.0;

  // ----------------- auxiliary routines -----------

  /**
   * Creates a Histogram for uniform points (grid width = delta) and a set of random points in the
   * unit hypercube transformed to the 1D active subspace via W1. This is used as the volume of the
   * inactive subspace
   *
   * @param numHistogramMCPoints	number of Monte Carlo points for the creation of the
   * histogram
   * @param points					the uniform point on the 1D active subspace
   * @param delta					the grid width of points
   * @param pointStrategy 			strategy how the Monte Carlo points are chosen. 'MC'
   * for uniform random, 'Halton' and 'Sobol' for the according Quasi Monte Carlo point sequences
   *
   * @return 						vector with the weights in the Histogram
   */
  sgpp::base::DataVector uniformIntervalHistogram(size_t numHistogramMCPoints,
                                                  sgpp::base::DataVector points, double delta,
                                                  std::string pointStrategy = "MC");

  /**
   * creates a linear B-spline interpolant for volume l(x) of the inactive subspace orthogonal to a
   * point x in the active subspace using the histogram to evaluate l
   *
   * @param level					level of the regular interpolant
   * @param numHistogramMCPoints	number of Monte Carlo points for the creation of the
   * histogram
   * @param quadDegree				degree for the quadrature
   * @param quadGridType			type of the grid used for the interpolant
   * @param quadGrid				reference to return the interpolation grid
   * @param quadCoefficients		reference to return the interpolation coefficients
   * @param pointStrategy 			strategy how the Monte Carlo points are chosen. 'MC'
   * for uniform random, 'Halton' and 'Sobol' for the according Quasi Monte Carlo point sequences
   */
  void continuousIntervalQuadrature(size_t level, size_t numHistogramMCPoints, size_t quadDegree,
                                    sgpp::base::GridType quadGridType,
                                    std::shared_ptr<sgpp::base::Grid>& quadGrid,
                                    sgpp::base::DataVector& quadCoefficients,
                                    std::string pointStrategy = "MC");

  /**
   *refines the grid surplus adaptive and recalculates the interpoaltion coefficients
   *
   *@param refinementsNum	number of grid points which should be refined
   *@pram objectiveFunc		the objective function
   */
  void refineSurplusAdaptive(size_t refinementsNum,
                             std::shared_ptr<sgpp::optimization::ScalarFunction> objectiveFunc);

  /**
   * calculates the interpolation coefficients for the reduced response surface on the active
   * subspace for a given grid.
   * For x in the original parameter space y = W1T*x is its according point in the active subspace.
   * Accordingly to interpolate at points y in the active subsapce we want to evaluate
   * f(inv(W1T)*y). W1 is usually not invertible so we use the Moore Penrose pseudo inverse instead.
   *
   * @param objectiveFunc	the objective function
   */
  void calculateInterpolationCoefficientsWithPseudoInverse(
      std::shared_ptr<sgpp::optimization::ScalarFunction> objectiveFunc);

  /**
   * returns the corners of an arbitrary dimensional hypercube
   * @param dimension	dimension of the hypercube
   * @return 			the corners (column wise)
   */
  Eigen::MatrixXd hypercubeVertices(size_t dimension);

  /**
   * determine the 1D active subspaces bounds by transforming all corners c of the unit hypercube to
   * the active subspace by calculating W1T*c. The minimum and maximum of these values are the
   * active subspaces bounds
   */
  void transformationfor1DActiveSubspace();
};

}  // namespace optimization
}  // namespace sgpp
