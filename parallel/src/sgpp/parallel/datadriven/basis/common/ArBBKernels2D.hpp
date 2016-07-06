// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef ARBBKERNELS2D_HPP
#define ARBBKERNELS2D_HPP

#include <sgpp/globaldef.hpp>

#include <cstdlib>

namespace sgpp {

namespace parallel {

class ArBBKernels2D {
 private:
  bool isMultTransSPfirst;
  bool isMultSPfirst;
  bool isMultTransfirst;
  bool isMultfirst;
  bool isMultModTransSPfirst;
  bool isMultModSPfirst;
  bool isMultModTransfirst;
  bool isMultModfirst;

 public:
  ArBBKernels2D();

  ~ArBBKernels2D();

  double multTransArBB(double* ptrSource, double* ptrData, double* ptrLevel, double* ptrIndex,
                       double* ptrGlobalResult, size_t sourceSize, size_t storageSize, size_t dims);

  double multArBB(double* ptrAlpha, double* ptrData, double* ptrLevel, double* ptrIndex,
                  double* ptrResult, size_t result_size, size_t storageSize, size_t dims);

  double multTransSPArBB(float* ptrSource, float* ptrData, float* ptrLevel, float* ptrIndex,
                         float* ptrGlobalResult, size_t sourceSize, size_t storageSize,
                         size_t dims);

  double multSPArBB(float* ptrAlpha, float* ptrData, float* ptrLevel, float* ptrIndex,
                    float* ptrResult, size_t result_size, size_t storageSize, size_t dims);

  double multModTransArBB(double* ptrSource, double* ptrData, double* ptrLevel, double* ptrIndex,
                          double* ptrGlobalResult, size_t sourceSize, size_t storageSize,
                          size_t dims);

  double multModArBB(double* ptrAlpha, double* ptrData, double* ptrLevel, double* ptrIndex,
                     double* ptrResult, size_t result_size, size_t storageSize, size_t dims);

  double multModTransSPArBB(float* ptrSource, float* ptrData, float* ptrLevel, float* ptrIndex,
                            float* ptrGlobalResult, size_t sourceSize, size_t storageSize,
                            size_t dims);

  double multModSPArBB(float* ptrAlpha, float* ptrData, float* ptrLevel, float* ptrIndex,
                       float* ptrResult, size_t result_size, size_t storageSize, size_t dims);

  void resetKernels();
};
}  // namespace parallel
}  // namespace sgpp

#endif /* ARBBKERNELS2D_HPP */