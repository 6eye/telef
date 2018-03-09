#include <iostream>
#include <pcl/io/openni2_grabber.h>
#include <memory>

#include "io/device.h"
#include "io/channel.h"
#include "io/pipe.h"
#include "cloud/cloud_pipe.h"
#include "type.h"

using namespace pcl;
using namespace telef::io;
using namespace telef::types;
using namespace telef::cloud;

/**
 * Continuously get frames of point cloud and image.
 *
 * Prints size of pointcloud and size of the image on every frame received
 */

int main(int ac, char* av[]) 
{
    pcl::io::OpenNI2Grabber::Mode depth_mode = pcl::io::OpenNI2Grabber::OpenNI_Default_Mode;
    pcl::io::OpenNI2Grabber::Mode image_mode = pcl::io::OpenNI2Grabber::OpenNI_Default_Mode;

    std::unique_ptr<Grabber> grabber {new io::OpenNI2Grabber("#1", depth_mode, image_mode)};

    std::unique_ptr<IdentityPipe<ImageT>> stubImagePipe{new IdentityPipe<ImageT>};
    std::unique_ptr<RemoveNaNPoints> stubCloudPipe{new RemoveNaNPoints()};

    std::shared_ptr<ImageChannel<ImageT>> imageChannel{ new ImageChannel<ImageT>(std::move(stubImagePipe)) };
    std::shared_ptr<CloudChannel<CloudConstT>> cloudChannel{ new CloudChannel<CloudConstT>(std::move(stubCloudPipe)) };

    ImagePointCloudDevice<CloudConstT, ImageT> device {std::move(grabber)};
    device.addCloudChannel(cloudChannel);
    device.addImageChannel(imageChannel);

    device.run();

    return 0;
}

