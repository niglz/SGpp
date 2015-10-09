// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef OPERATIONNAIVEEVALLINEARCLENSHAWCURTIS_HPP
#define OPERATIONNAIVEEVALLINEARCLENSHAWCURTIS_HPP

#include <sgpp/globaldef.hpp>
#include <sgpp/base/operation/hash/OperationNaiveEval.hpp>
#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/operation/hash/common/basis/LinearClenshawCurtisBasis.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

namespace SGPP {
  namespace base {

    class OperationNaiveEvalLinearClenshawCurtis : public OperationNaiveEval {
      public:
        /**
         * Constructor.
         *
         * @param storage       storage of the sparse grid
         */
        OperationNaiveEvalLinearClenshawCurtis(
          GridStorage* storage)
          : storage(storage) {
        }

        /**
         * Virtual destructor.
         */
        virtual ~OperationNaiveEvalLinearClenshawCurtis() {
        }

        /**
         * @param alpha     coefficient vector
         * @param point     evaluation point
         * @return          value of linear combination
         */
        virtual float_t eval(const DataVector& alpha, const DataVector& point);

      protected:
        /// storage of the sparse grid
        GridStorage* storage;
        /// 1D linear basis
        SLinearClenshawCurtisBase base;
    };

  }
}

#endif /* OPERATIONNAIVEEVALLINEARCLENSHAWCURTIS_HPP */
