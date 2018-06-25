#pragma once

#include <cuda_runtime_api.h>

/**
 * Quaternion calculation helper functions for CUDA code
 *
 * There are three different representations for origin-preserving rotation in this project
 *
 * 1. Axis-angle representation 'u'
 *
 *      * 'u' is a 3-element vector
 *      * The direction of u represents the axis of rotation
 *          (axis of rotation) = (u / |u|)
 *      * The l2-norm of u represents the amount of rotation
 *          (amount of rotation in radian) = |u|
 *          |u| <= PI
 *
 * 2. Unit quaternion representation 'q'
 *
 *      * 'q' is a 4-element vector
 *      * q = (a, v) where a is real scalar and v is 3-element vector
 *      * q(u) represents a unit quaternion determined by a axis-angle representation 'u'
 *      * The direction of imaginary part 'v' represents the axis of rotation
 *          (axis of rotation) = v
 *      * The value of 'a' represents the amount of rotation
 *          (amount of rotation in radian) = 2arccos(a)
 *
 * 3. Rotation matrix 'R'
 *
 *      * 'R' is a 3x3 matrix
 *      * R(q) represents a rotation matrix determined by a unit quaternion 'q'
 *
 * The reason we use three different representations is because they all have different use.
 *
 * 'u' has the least number of parameters, so it is appropriate for parameter optimization
 * 'q' is good for composing different rotations
 * 'R' is needed to apply the rotation to any 3d point in R^3
 */

/**
 * Calculate dR(q(u)) / du_i on GPU
 *
 * @param dr_du_d 3x3x3 array in device
 *          (dr_du_d + i*3*3) represents 3x3 column major matrix for dR(q(u)) / du_i
 * @param u_d       3-element axis-angle representation in device
 */
__device__
void calc_dr_du(float *dr_du_d, const float *u_d);

/**
 * Calculate dR(q) / dq_i on GPU
 *
 * @param dr_dq_d 4x3x3 array in device
 *          (dr_dq_d + i*3*3) represents 3x3 column major matrix for dR(q) / dq_i
 * @param q_d     4-element quaternion in device
 */
__device__
void calc_dr_dq(float *dr_dq_d, const float *q_d);

/**
 * Calculate quaternion q from axis-angle vector u
 *
 * @param q_d     4-element quaternion in device
 * @param u_d     3-element axis-angle vector in device
 */
__device__
void calc_q(float *q_d, const float *u_d);

/**
 * Calculate dq(u) / du_i on GPU
 *
 * @param dq_du_d   3x4 array in device
 *          (dq_du_d + i*4) represents 4-element vector for dq(u) / du_i
 * @param u_d       3-element axis-angle vector in device
 */
__device__
void calc_dq_du(float *dq_du_d, const float *u_d);