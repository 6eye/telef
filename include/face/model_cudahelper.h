#pragma once

#include <vector>
#include <Eigen/Core>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

/* Includes, cuda */
#include <cuda_runtime.h>
//#include <cublas_v2.h>


#include "face/raw_model.h"



/** Loads MorphableFaceModel to GPU Device */
void loadModelToCUDADevice(C_PcaDeformModel *deformModel,
                           const Eigen::MatrixXf shapeDeformBasis, const Eigen::MatrixXf expressionDeformBasis,
                           const Eigen::VectorXf ref,
                           const Eigen::VectorXf meanShapeDeformation, 
                           const Eigen::VectorXf meanExpressionDeformation,
                           const std::vector<int> lmkInds);
void freeModelCUDA(C_PcaDeformModel deformModel);

/** Loads PointCloud to GPU Device */
void loadScanToCUDADevice(C_ScanPointCloud *scanPointCloud,
                          boost::shared_ptr<const pcl::PointCloud<pcl::PointXYZRGBA>> scan,
                          const std::vector<int> scanLmkIdx,
                          const std::vector<int> validLmks,
                          const Eigen::MatrixXf rigidTransform);

void freeScanCUDA(C_ScanPointCloud scanPointCloud);

/**
 * Loads PCA+Transform coefficients to GPU Device
 *
 * If update is true, doesn't malloc. uses previously allocated address
 */
void allocParamsToCUDADevice(C_Params *params, int numa, int numt, int numu);
void updateParams(const C_Params params,
                  const float *const aIn, int numa,
                  const float *const tIn, int numt,
                  const float *const uIn, int numu);
void freeParamsCUDA(C_Params params);

void allocPositionCUDA(float **position_d, int dim);
void freePositionCUDA(float *position_d);

