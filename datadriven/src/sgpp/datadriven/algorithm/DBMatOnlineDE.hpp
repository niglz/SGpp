// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/datadriven/algorithm/DBMatOnline.hpp>

#include <list>

namespace sgpp {
namespace datadriven {

using sgpp::base::DataMatrix;
using sgpp::base::DataVector;

/**
 * Class that stores, generates and manipulates a density function during online phase in on/off
 * learning.
 */
class DBMatOnlineDE : public DBMatOnline {
 public:
  /**
   * Constructor
   *
   * @param offline The offline object we base our evaluations on.
   * @param grid The underlying grid (TODO(fuchsgruber) do we need this?)
   * @param lambda The regularization strength (TODO(fuchsgruber) remove this)
   * @param beta The initial weighting factor
   */
  explicit DBMatOnlineDE(DBMatOffline& offline, Grid& grid, double lambda, double beta = 0.);

  /**
   * Restructures the rhs (b vector) of the system matrix. This is only availible for streaming,
   * i.e. when computeDensityFunction was called with save_b = true.
   * First b is coarsened, then extended according to the new grid size (refinement).
   *
   * @param gridSize grid size after coarsening and refinement (inherently gives the number of
   * points added during refinement after coarsening)
   * @param deletedPoints pointer to list of indexes that will be removed from b
   */
  void updateRhs(size_t gridSize, std::list<size_t> *deletedPoints);

  /**
  * Computes the density function again based on the saved b's (only applicable for streaming)
  *
  * @param alpha the vector where surplusses for the density function will be stored
  * @param grid The underlying grid
  * @param densityEstimationConfig Configuration for the density estimation
  *        combined with the new right hand side (aka streaming)
  * @param do_cv Indicates whether crossvalidation should take place
  */
  void computeDensityFunction(DataVector& alpha, Grid& grid,
    DensityEstimationConfiguration& densityEstimationConfig, bool do_cv = false);

  /**
   * Computes the density function for a certain data matrix
   *
   * @param alpha the vector where surplusses for the density function will be stored
   * @param m the matrix that contains the data points
   * @param grid The underlying grid
   * @param densityEstimationConfig Configuration for the density estimation
   * @param save_b Indicates whether the old right hand side should be saved and
   *        combined with the new right hand side (aka streaming)
   * @param do_cv Indicates whether crossvalidation should take place
   * @param deletedPoints indicates the indices of removed grid points due to
   * coarsening
   * @param newPoints indicates the amount of added points due to refinement
   */
  void computeDensityFunction(DataVector& alpha, DataMatrix& m, Grid& grid,
      DensityEstimationConfiguration& densityEstimationConfig, bool save_b = false,
      bool do_cv = false, std::list<size_t>* deletedPoints = nullptr, size_t newPoints = 0);

  /**
   * Evaluates the density function at a certain point
   *
   * @param alpha the vector of surplusses
   * @param p the point at which the function is evaluated
   * @param grid the underlying grid
   * @param force if set, it will even try to evaluate if the internal state recommends otherwise
   * @return the result of the evaluation
   */
  double eval(DataVector& alpha, const DataVector& p, Grid& grid, bool force = false);

  /**
   * Evaluates the density function on multiple points
   *
   * @param alpha the vector of surplusses
   * @param values the points at which the function is evaluated
   * @param results the result of the evaluation
   * @param grid the underlying grid
   * @param force if set, it will even try to evaluate if the internal state recommends otherwise
   */
  void eval(DataVector& alpha, DataMatrix& values, DataVector& results, Grid& grid,
      bool force = false);

  /**
   * Returns if the surplus has already been computed
   */
  bool isComputed();

  /**
   * Sets the weighting factor
   *
   * @param beta the new weighting factor. If set to 0, no plasticity takes
   * place.
   */
  void setBeta(double beta);

  /**
   * Returns the current weighting factor
   */
  double getBeta();

  /**
   * Normalize the Density
   *
   * @param alpha the vector of surplusses
   * @param grid the underlying grid
   * @param samples number of samples to be used for MC quadrature
   */
  double normalize(DataVector& alpha, Grid& grid, size_t samples = 1000);

  /**
   * Normalize the Density using Quadrature
   *
   * @param alpha the vector of surplusses
   * @param grid the underlying grid
   */
  double normalizeQuadrature(DataVector& alpha, Grid& grid);

 protected:
  virtual void solveSLE(DataVector& alpha, DataVector& b, Grid& grid,
      DensityEstimationConfiguration& densityEstimationConfig, bool do_cv) = 0;
  double computeL2Error(DataVector& alpha, Grid& grid);
  double resDensity(DataVector& alpha, Grid& grid);

  bool functionComputed;
  DataVector bSave;
  DataVector bTotalPoints;
  double beta;
  size_t totalPoints;
  DataMatrix *testMat, *testMatRes;
  double normFactor;
  double lambda;
  size_t oDim;
};

}  // namespace datadriven
}  // namespace sgpp
