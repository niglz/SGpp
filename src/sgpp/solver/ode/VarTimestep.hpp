/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/

#ifndef VARTIMESTEP_HPP
#define VARTIMESTEP_HPP

#include "application/common/ScreenOutput.hpp"
#include "solver/ODESolver.hpp"
#include "algorithm/pde/BlackScholesODESolverSystem.hpp"
#include <string>

namespace sg
{

/**
 * This class implements the explicit Adams-Bashforth method
 * for solving ordinary partial equations
 *
 * @version $HEAD$
 */
class VarTimestep : public ODESolver
{
private:
	/// specifies if a grid evaluation should be execute in every time step
	bool bAnimation;
	/// specifies the evaluation per dimension when a animation is created
	size_t evalsAnimation;
	/// Pointer to ScreenOutput object
	ScreenOutput* myScreen;

	double TimestepSize;


public:
	/**
	 * Std-Constructer
	 *
	 * @param Mode the mode of the euler that should be executed, must be ExEul or ImEul
	 * @param imax number of maximum executed iterations
	 * @param timestepSize the size of one timestep
	 * @param generateAnimation set this, if you want to create a grid evaluation in every time step, in order to create an animation
	 * @param numEvalsAnimation specifies the evaluation per dimension when a animation is created
	 * @param screen possible pointer to a ScreenOutput object
	 */
	VarTimestep(size_t imax, double timestepSize, bool generateAnimation = false, size_t numEvalsAnimation = 20, ScreenOutput* screen = NULL);

	/**
	 * Std-Destructor
	 */
	virtual ~VarTimestep();

	virtual void solve(SLESolver& LinearSystemSolver, OperationODESolverSystem& System, bool bIdentifyLastStep = false, bool verbose = false);
};

}

#endif /* VARTIMESTEP_HPP */
