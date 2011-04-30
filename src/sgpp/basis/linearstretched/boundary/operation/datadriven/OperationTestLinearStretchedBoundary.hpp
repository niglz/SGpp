/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef OPERATIONTESTLINEARSTRETCHEDBOUNDARY_HPP
#define OPERATIONTESTLINEARSTRETCHEDBOUNDARY_HPP

#include "operation/datadriven/OperationTest.hpp"
#include "grid/GridStorage.hpp"
using namespace sg::base;

namespace sg
{
namespace datadriven
{

/**
 * This class implements OperationTest for a grids with linear basis ansatzfunctions with
 * boundaries
 *
 * @version $HEAD$
 */
class OperationTestLinearStretchedBoundary : public OperationTest
{
public:
	/**
	 * Constructor
	 *
	 * @param storage the grid's GridStorage object
	 */
	OperationTestLinearStretchedBoundary(GridStorage* storage) : storage(storage) {}

	/**
	 * Destructor
	 */
	virtual ~OperationTestLinearStretchedBoundary() {}

	virtual double test(DataVector& alpha, DataMatrix& data, DataVector& classes);
	virtual double testMSE(DataVector& alpha, DataMatrix& data, DataVector& refValues);
	virtual double testWithCharacteristicNumber(DataVector& alpha, DataMatrix& data, DataVector& classes, DataVector& charaNumbers);

protected:
	/// Pointer to GridStorage object
	GridStorage* storage;
};

}
}

#endif /* OPERATIONTESTLINEARSTRETCHEDBOUNDARY_HPP */
