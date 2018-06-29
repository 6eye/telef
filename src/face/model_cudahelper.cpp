#include <cuda_runtime_api.h>

#include "util/cudautil.h"
#include "face/model_cudahelper.h"


void loadModelToCUDADevice(C_PcaDeformModel *deformModel,
                           const Eigen::MatrixXf deformBasis, const Eigen::VectorXf ref,
                           const std::vector<int> lmkInds) {
    std::cout << "deformBasis.size() <<: " << deformBasis.size() << std::endl;
    CUDA_CHECK(cudaMalloc((void**)(&deformModel->deformBasis_d), deformBasis.size()*sizeof(float)));
    CUDA_CHECK(cudaMalloc((void**)(&deformModel->ref_d), ref.size()*sizeof(float)));
    CUDA_CHECK(cudaMalloc((void**)(&deformModel->lmks_d), lmkInds.size()*sizeof(int)));

    CUDA_CHECK(cudaMemcpy((void*)deformModel->deformBasis_d,
               deformBasis.data(), deformBasis.size()*sizeof(float), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy((void*)deformModel->ref_d,
               ref.data(), ref.size()*sizeof(float), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy((void*)deformModel->lmks_d,
               lmkInds.data(), lmkInds.size()*sizeof(int), cudaMemcpyHostToDevice));

    deformModel->rank = (int)deformBasis.cols();
    deformModel->dim = (int)deformBasis.rows();
    deformModel->lmkCount = (int)lmkInds.size();

    CUDA_CHECK(cudaMalloc((void**)&deformModel->rank_d, sizeof(int)));
    CUDA_CHECK(cudaMalloc((void**)&deformModel->dim_d, sizeof(int)));
    CUDA_CHECK(cudaMalloc((void**)&deformModel->lmkCount_d, sizeof(int)));

    CUDA_CHECK(cudaMemcpy(deformModel->rank_d, &deformModel->rank, sizeof(int), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(deformModel->dim_d, &deformModel->dim, sizeof(int), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(deformModel->lmkCount_d, &deformModel->lmkCount, sizeof(int), cudaMemcpyHostToDevice));

    assert(deformBasis.rows() == ref.size());
}

void freeModelCUDA(C_PcaDeformModel deformModel) {
    cudaFree(deformModel.deformBasis_d);
    cudaFree(deformModel.ref_d);
    cudaFree(deformModel.lmks_d);
}

void loadScanToCUDADevice(C_ScanPointCloud *scanPointCloud,
                          boost::shared_ptr<const pcl::PointCloud<pcl::PointXYZRGBA>> scan,
                          const std::vector<int> scanLmkIdx,
                          const std::vector<int> validLmks,
                          const Eigen::MatrixXf rigidTransform) {

    cudaMalloc((void**)(&scanPointCloud->scanPoints_d), scan->points.size()*3*sizeof(float));
    cudaMalloc((void**)(&scanPointCloud->validModelLmks_d), validLmks.size()*sizeof(int));
    cudaMalloc((void**)(&scanPointCloud->scanLmks_d), scanLmkIdx.size()*sizeof(int));
    cudaMalloc((void**)(&scanPointCloud->rigidTransform_d), rigidTransform.size()*sizeof(float));

    float *scanPoints = new float[scan->points.size()*3];
    for (int i=0; i<scan->points.size(); i+=3) {
        scanPoints[i] = scan->points[i].x;
        scanPoints[i+1] = scan->points[i].y;
        scanPoints[i+2] = scan->points[i].z;
    }

    cudaMemcpy((void*)scanPointCloud->scanPoints_d,
               scanPoints, scan->points.size()*3*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy((void*)scanPointCloud->validModelLmks_d,
               validLmks.data(), validLmks.size()*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy((void*)scanPointCloud->scanLmks_d,
               scanLmkIdx.data(), scanLmkIdx.size()*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy((void*)scanPointCloud->rigidTransform_d,
               rigidTransform.data(), rigidTransform.size()*sizeof(float), cudaMemcpyHostToDevice);

    scanPointCloud->numPoints = scan->points.size();
    scanPointCloud->transformCols = (int)rigidTransform.cols();
    scanPointCloud->transformRows = (int)rigidTransform.rows();
    //TODO: Need Fix
    scanPointCloud->numLmks = scan->points.size();


    cudaMalloc((void**)&scanPointCloud->numPoints_d, sizeof(int));
    cudaMalloc((void**)&scanPointCloud->transformCols_d, sizeof(int));
    cudaMalloc((void**)&scanPointCloud->transformRows_d, sizeof(int));
    cudaMalloc((void**)&scanPointCloud->numLmks_d, sizeof(int));

    cudaMemcpy(scanPointCloud->numPoints_d, &scanPointCloud->numPoints, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(scanPointCloud->transformCols_d, &scanPointCloud->transformCols, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(scanPointCloud->transformRows_d, &scanPointCloud->transformRows, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(scanPointCloud->numLmks_d, &scanPointCloud->numLmks, sizeof(int), cudaMemcpyHostToDevice);

    assert(scanLmkIdx.size() == validLmks.size());
}

void freeScanCUDA(C_ScanPointCloud scanPointCloud) {
    cudaFree(scanPointCloud.scanPoints_d);
}

void allocParamsToCUDADevice(C_Params *params, int numParams) {
    cudaMalloc((void **)(&params->params_d), numParams*sizeof(float));
    float *zero = new float[numParams]{0,};
    params->numParams = numParams;

    updateParamsInCUDADevice(*params, zero, numParams);
    delete[] zero;
}

void updateParamsInCUDADevice(const C_Params params, const float * const paramsIn, int numParams) {
    cudaMemcpy((void*)params.params_d, paramsIn, numParams*sizeof(float), cudaMemcpyHostToDevice);


    cudaMalloc((void**)&params.numParams_d, sizeof(int));
    cudaMemcpy(params.numParams_d, &params.numParams, sizeof(int), cudaMemcpyHostToDevice);
}

void freeParamsCUDA(C_Params params) {
    cudaFree(params.params_d);
}

void allocPositionCUDA(float **position_d, int dim) {
    cudaMalloc((void**)(position_d), dim*sizeof(float));
}

void freePositionCUDA(float *position_d) {
    cudaFree(position_d);
}

