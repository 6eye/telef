#pragma once

#include <cuda_runtime_api.h>
#include <cublas_v2.h>
#include "face/raw_model.h"

__global__
void _calculateVertexPosition(float *position_d, const C_Params params, const C_PcaDeformModel deformModel);

void calculateVertexPosition(float *position_d, const C_Params params, const C_PcaDeformModel deformModel);

__global__
void _homogeneousPositions(float *h_position_d, const float *position_d, int nPoints);

__global__
void _hnormalizedPositions(float *position_d, const float *h_position_d, int nPoints);

//
//__device__
//void convertXyzToUv(float *uv, const float* xyz, float fx, float fy, float cx, float cy);

__global__
void _find_mesh_to_scan_corr(int *meshToScanCorr_d, float *distance_d,
                             const float *position_d, int num_points, C_ScanPointCloud scan, float radius);

/**
 *
 * @param meshToScanCorr_d, Output Mesh To Scan Correspondance
 * @param distance_d, Output Corresponding Mesh to Scan Distance
 * @param position_d, Aligned Mesh
 * @param num_points
 * @param scan
 * @param radius, Tolerance or search window, if radius is 0, include all points
 */
void find_mesh_to_scan_corr(int *meshToScanCorr_d, float *distance_d,
                            const float *position_d, int num_points, C_ScanPointCloud scan, float radius);

/**
 * Applies Transformation matrix on CUDA device model
 * @param align_pos_d
 * @param position_d
 * @param deformModel
 * @param scanPointCloud
 */
void applyRigidAlignment(float *align_pos_d, cublasHandle_t cnpHandle,
                         const float *position_d, const float *transMat, int N);

/**
 * GPU MatrixMultiply using Cublas
 * @param matC
 * @param matA_host
 * @param aCols
 * @param aRows
 * @param matB
 * @param bCols
 * @param bRows
 */
void cudaMatMul(float *matC, cublasHandle_t cnpHandle,
                const float *matA, int aRows, int aCols,
                const float *matB, int bRows, int bCols);

/**
 * Calculate residual and jacobian of the loss function representing distance btw scan and model
 *
 * Loss = (L2 distance btw corresponding landmarks)
 *      + (L2 norm of parameters)
 */
void calculateLoss(float *residual, float *fa1Jacobian, float *fa2Jacobian, float *ftJacobian, float *fuJacobian,
                   float *position_d,
                   cublasHandle_t cnpHandle,
                   const C_Params params,
                   const C_PcaDeformModel deformModel, const C_ScanPointCloud scanPointCloud,
                   const bool isJacobianRequired);