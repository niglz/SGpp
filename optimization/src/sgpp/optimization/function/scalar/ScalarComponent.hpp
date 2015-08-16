// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef SGPP_OPTIMIZATION_FUNCTION_SCALAR_SCALARCOMPONENT_HPP
#define SGPP_OPTIMIZATION_FUNCTION_SCALAR_SCALARCOMPONENT_HPP

#include <sgpp/globaldef.hpp>
#include <sgpp/optimization/function/scalar/ScalarFunction.hpp>
#include <sgpp/optimization/function/vector/VectorFunction.hpp>

#include <stdexcept>
#include <cmath>
#include <vector>
#include <algorithm>

namespace SGPP {
  namespace optimization {

    /**
     * One component of a vector-valued function.
     *
     * Given a vector-valued \f$f\colon [0, 1]^d \to \mathbb{R}^m\f$
     * and indices \f$1 \le i_1 < \dotsb < i_n \le d\f$,
     * this class represents a new function
     * \f$g\colon [0, 1]^n \to \mathbb{R}\f$ with
     * \f$g(y_{i_1}, \dotsc, y_{i_n}) := f_k(y_1, \dotsc, y_d)\f$,
     * where \f$y_t\f$ is constant for \f$t \in \{i_1, \dotsc, i_n\}\f$.
     *
     * The resulting function \f$g\f$ is similar to a "slice plot" of
     * the component \f$f_k\f$ of \f$f\f$.
     */
    class ScalarComponent : public ScalarFunction {
      public:
        /**
         * Constructor.
         *
         * Use it like this:
         * ScalarComponent g(f, 3, {NAN, NAN, 0.42});
         * where f is a vector-valued function with 5 components and
         * 3 parameters.
         * This selects the first two parameters and the fourth component
         * of f, while constantly using 0.42 for the third parameter.
         *
         * @param f             vector-valued function (m components)
         * @param k             index of component \f$f_k\f$ to select
         *                      (between 0 and m - 1)
         * @param defaultValues Vector of constant default values.
         *                      It can be either empty (the default) or
         *                      a vector of exactly m float_ts,
         *                      each of which can be finite or NAN.
         *                      If the vector is empty, it will be initialized
         *                      as m NANs (i.e., no restriction of the
         *                      parameter domain).
         *                      Each NAN represents a free parameter \f$x_t\f$,
         *                      while the finite entries denote the constant
         *                      values for the corresponding parameter.
         */
        ScalarComponent(
          VectorFunction& f,
          size_t k,
          std::vector<float_t> defaultValues = std::vector<float_t>()) :

          ScalarFunction((defaultValues.size() > 0) ?
                         std::count(defaultValues.begin(),
                                    defaultValues.end(), NAN) :
                         f.getDimension()),
          f(f),
          k(k),
          defaultValues((defaultValues.size() > 0) ?
                        defaultValues :
                        std::vector<float_t>(f.getDimension(), NAN)),
          y(f.getDimension()),
          fy(f.getNumberOfComponents()) {

          // make sure defaultValues has the correct size
          if (this->defaultValues.size() != f.getDimension()) {
            throw std::runtime_error(
              "ScalarComponent(): Invalid defaultValues.");
          }

          // initialize constant non-NAN entries
          for (size_t t = 0; t < f.getDimension(); t++) {
            if (!std::isnan(this->defaultValues[t])) {
              y[t] = this->defaultValues[t];
            }
          }
        }

        /**
         * @param x     evaluation point \f$\vec{x} \in [0, 1]^n\f$
         * @return      \f$g(\vec{x}) := f_k(y_1, \dotsc, y_d)\f$
         *              where \f$(x_1, \dotsc, x_n) =
         *              (y_{i_1}, \dotsc, y_{i_n})\f$
         */
        float_t eval(const base::DataVector& x) {
          size_t t2 = 0;

          // select entries of x which correspond to NAN entries in
          // defaultValues
          for (size_t t = 0; t < f.getDimension(); t++) {
            if (std::isnan(defaultValues[t])) {
              y[t] = x[t2];
              t2++;
            }
          }

          // evaluate and select component
          f.eval(y, fy);
          return fy[k];
        }

        /**
         * @param[out] clone pointer to cloned object
         */
        virtual void clone(std::unique_ptr<ScalarFunction>& clone) const {
          clone = std::unique_ptr<ScalarFunction>(
                    new ScalarComponent(f, k, defaultValues));
        }

      protected:
        /// vector-valued function
        VectorFunction& f;
        /// index of component
        size_t k;
        /// vector of default values, indicating free variables with NAN
        std::vector<float_t> defaultValues;
        /// temporary evaluation point
        base::DataVector y;
        /// temporary result point
        base::DataVector fy;
    };

  }
}

#endif /* SGPP_OPTIMIZATION_FUNCTION_SCALAR_SCALARCOMPONENT_HPP */
