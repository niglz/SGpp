// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef DATADRIVEN_OP_FACTORY_HPP
#define DATADRIVEN_OP_FACTORY_HPP

#include <sgpp/base/grid/Grid.hpp>

#include <sgpp/datadriven/operation/hash/OperationTest.hpp>
#include <sgpp/base/operation/hash/OperationMatrix.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityMarginalize.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityMargTo1D.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityConditional.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensitySampling1D.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensitySampling.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityRejectionSampling.hpp>
#include <sgpp/datadriven/operation/hash/OperationTransformation1D.hpp>
#include <sgpp/datadriven/operation/hash/OperationRosenblattTransformation.hpp>
#include <sgpp/datadriven/operation/hash/OperationInverseRosenblattTransformation.hpp>
#include <sgpp/base/operation/hash/OperationMultipleEval.hpp>
#include <sgpp/datadriven/operation/hash/DatadrivenOperationCommon.hpp>

#include <sgpp/datadriven/operation/hash/OperationRosenblattTransformationKDE.hpp>
#include <sgpp/datadriven/operation/hash/OperationInverseRosenblattTransformationKDE.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityMarginalizeKDE.hpp>
#include <sgpp/datadriven/operation/hash/OperationDensityConditionalKDE.hpp>

/*
 * This file contains factory methods for operations.
 */

#include <sgpp/globaldef.hpp>

namespace SGPP {
namespace op_factory {
/**
 * Factory method, returning an OperationTest for the grid at hand.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for OperationTest
 * @return Pointer to the new OperationTest object for the Grid grid
 */
datadriven::OperationTest* createOperationTest(base::Grid& grid);

/**
 * Factory method, returning an OperationRegularizationDiagonal for the grid at hand.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for OperationRegularizationDiagonal
 * @param mode Mode, specifying which regularization to use. Example: OperationRegularizationDiagonal::HKMIX.
 * @param k Parameter for @f$H^k@f$
 * @return Pointer to the new OperationRegularizationDiagonal object for the Grid grid
 */
base::OperationMatrix* createOperationRegularizationDiagonal(base::Grid& grid,
        int mode, float_t k);

/**
 * Factory method, returning an OperationDensityMarginalize for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensityMarginalize for the Grid grid
 */
datadriven::OperationDensityMarginalize* createOperationDensityMarginalize(
        base::Grid& grid);

/**
 * Factory method, returning an OperationDensityMargTo1D for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensityMarginalize for the Grid grid
 */
datadriven::OperationDensityMargTo1D* createOperationDensityMargTo1D(
        base::Grid& grid);

/**
 * Factory method, returning an OperationDensitySampling1D for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensitySampling1D for the Grid grid
 */
datadriven::OperationDensitySampling1D* createOperationDensitySampling1D(
        base::Grid& grid);

/**
 * Factory method, returning an OperationDensitySampling for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensitySampling1D for the Grid grid
 */
datadriven::OperationDensitySampling* createOperationDensitySampling(
        base::Grid& grid);

/**
 * Factory method, returning an OperationDensityRejectionSampling for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensitySampling1D for the Grid grid
 */
datadriven::OperationDensityRejectionSampling* createOperationDensityRejectionSampling(
        base::Grid& grid);

/**
 * Factory method, returning an OperationDensityConditional for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationDensityConditional for the Grid grid
 */
datadriven::OperationDensityConditional* createOperationDensityConditional(
        base::Grid& grid);

/**
 * Factory method, returning an OperationRosenblattTransformation for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationRosenblattTransformation for the Grid grid
 */
datadriven::OperationRosenblattTransformation* createOperationRosenblattTransformation(
        base::Grid& grid);

/**
 *
 * @param grid
 * @return
 */
datadriven::OperationTransformation1D* createOperationRosenblattTransformation1D(
        base::Grid& grid);

/**
 * Factory method, returning an OperationInverseRosenblattTransformation for the grid.
 * Note: object has to be freed after use.
 *
 * @param grid Grid which is to be used for the operation
 * @return Pointer to new OperationRosenblattTransformation for the Grid grid
 */
datadriven::OperationInverseRosenblattTransformation* createOperationInverseRosenblattTransformation(
        base::Grid& grid);

/**
 *
 * @param grid
 * @return
 */
datadriven::OperationTransformation1D* createOperationInverseRosenblattTransformation1D(
        base::Grid& grid);

/**
 *
 * @param trainingSamples
 * @return
 */
datadriven::OperationRosenblattTransformationKDE* createOperationRosenblattTransformationKDE(
        datadriven::GaussianKDE& kde);

/**
 *
 * @param trainingSamples
 * @return
 */
datadriven::OperationInverseRosenblattTransformationKDE* createOperationInverseRosenblattTransformationKDE(
        datadriven::GaussianKDE& kde);

/**
 * Factory method, returning an OperationDensityMarginalize for the kernel density.
 * Note: object has to be freed after use.
 *
 * @param kde kernel density which is to be used for the operation
 * @return Pointer to new OperationDensityMarginalize
 */
datadriven::OperationDensityMarginalizeKDE* createOperationDensityMarginalizeKDE(
        datadriven::GaussianKDE& kde);

/**
 * Factory method, returning an OperationDensityConditional for the kernel density.
 * Note: object has to be freed after use.
 *
 * @param kde kernel density which is to be used for the operation
 * @return Pointer to new OperationDensityConditional
 */
datadriven::OperationDensityConditionalKDE* createOperationDensityConditionalKDE(
        datadriven::GaussianKDE& kde);


base::OperationMultipleEval* createOperationMultipleEval(base::Grid& grid,
        base::DataMatrix& dataset,
        SGPP::datadriven::OperationMultipleEvalConfiguration configuration);

}
}

#endif /*DATADRIVEN_OP_FACTORY_HPP*/
