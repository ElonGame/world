#pragma once

#include "worldapi/worldapidef.h"

#include <armadillo/armadillo>

#ifdef USE_OPENCV

#include <opencv2/opencv.hpp>

namespace world {

    cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Mat<double> &mat);

    cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Cube<double> &cube);
}

#endif