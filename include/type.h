#pragma once

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/image.h>
#include <unordered_map>
#include "util/UvPointMapping.h"

namespace telef::types {
    using PointT = pcl::PointXYZRGBA;
    using CloudT = pcl::PointCloud<pcl::PointXYZRGBA>;
    using CloudConstT = const pcl::PointCloud<pcl::PointXYZRGBA>;
    using CloudPtrT = pcl::PointCloud<pcl::PointXYZRGBA>::Ptr;
    using CloudConstPtrT = pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr;

    using ImageT = pcl::io::Image;
    using ImagePtrT = pcl::io::Image::Ptr;
    using ImageConstPtrT = pcl::io::Image::ConstPtr;

    using Uv2PointIdMapT = telef::util::UvPointMapping;
    using Uv2PointIdMapConstT = const Uv2PointIdMapT;
    using Uv2PointIdMapPtrT = std::shared_ptr<Uv2PointIdMapT>;
    using Uv2PointIdMapConstPtrT = std::shared_ptr<Uv2PointIdMapConstT>;

}

namespace telef::io {
    /** Point cloud with metadata */
    using DeviceCloud = struct DeviceCloud {
        telef::types::CloudConstPtrT cloud;
        telef::types::Uv2PointIdMapPtrT img2cloudMapping;
        float fx;
        float fy;
    };
}

namespace telef::types {
    using DeviceCloudT = telef::io::DeviceCloud;
    using DeviceCloudConstT = const DeviceCloudT;
    using DeviceCloudPtrT = std::shared_ptr<DeviceCloudT>;
    using DeviceCloutConstPtrT = std::shared_ptr<DeviceCloudConstT>;
};