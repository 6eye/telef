#pragma once

#include <boost/shared_ptr.hpp>
#include <memory>
#include <future>
#include "type.h"
#include "io/pipe.h"
#include "feature/feature_detector.h"
#include "io/frontend.h"

using namespace telef::types;
using namespace telef::feature;

namespace telef::io {

    /**
     * Merge Two Data into One OutT Data
     *
     * Further process OutT into PipeOutT using Pipe<OutT, PipeOutT>
     */
    template<class DataAT, class DataBT, class OutT, class PipeOutT>
    class BinaryMerger {
    private:
        using OutPtrT = const boost::shared_ptr<OutT>;
        using DataAPtrT = const boost::shared_ptr<DataAT>;
        using DataBPtrT = const boost::shared_ptr<DataBT>;
        using PipeOutPtrT = const boost::shared_ptr<PipeOutT>;
        using PipeT = Pipe<OutT, PipeOutT>;
    public:
        explicit BinaryMerger (std::shared_ptr<PipeT> pipe) {
            this->pipe = pipe;
        }
        virtual ~BinaryMerger() = default;
        BinaryMerger& operator=(const BinaryMerger&) = delete;
        BinaryMerger (const BinaryMerger&) = default;

        void addFrontEnd(std::shared_ptr<FrontEnd<PipeOutT>> frontend) {
            this->frontends.emplace_back(frontend);
        }

        void run(DataAPtrT a, DataBPtrT b) {
            for (const auto& frontend : frontends) {
                frontend->process(getMergeOut(a, b));
            }
        }

    private:
        PipeOutPtrT getMergeOut(DataAPtrT a, DataBPtrT b) {
            auto merged = merge(a, b);
            return this->pipe->processData(merged);
        }
        virtual OutPtrT merge(DataAPtrT a, DataBPtrT b)=0;
        std::shared_ptr<PipeT> pipe;
        std::vector<std::shared_ptr<FrontEnd<PipeOutT>>> frontends;
    };

    /**
     * Merge Two Data into One OutT Data without any further processing
     */
    template<class DataAT, class DataBT, class OutT>
    class SimpleBinaryMerger : public BinaryMerger<DataAT, DataBT, OutT, OutT> {
    private:
        using BaseT = BinaryMerger<DataAT, DataBT, OutT, OutT>;
        using OutPtrT = const boost::shared_ptr<OutT>;
        using DataAPtrT = const boost::shared_ptr<DataAT>;
        using DataBPtrT = const boost::shared_ptr<DataBT>;
    public:
        SimpleBinaryMerger() : BaseT(std::make_shared<IdentityPipe<OutT>>()) {}
        OutPtrT merge (DataAPtrT a, DataBPtrT b) override = 0;
    };

    /**
     * Merge const PointCloud and Image Using (UV coord) -> Point ID mapping
     */
    template<class OutT>
    class SimpleMappedImageCloudMerger : public SimpleBinaryMerger<ImageT, MappedCloudConstT, OutT> {
    private:
        using OutPtrT = const boost::shared_ptr<OutT>;
        using MappedConstBoostPtrT = boost::shared_ptr<MappedCloudConstT>;
    public:
        OutPtrT merge(const ImagePtrT image, const MappedConstBoostPtrT cloudPair) override=0;
    };

    // TODO: Inject landmark detectors later
    class FittingSuiteMerger : public SimpleMappedImageCloudMerger<FittingSuite> {
    private:
        using OutPtrT = const boost::shared_ptr<FittingSuite>;
        using MappedConstBoostPtrT = boost::shared_ptr<MappedCloudConstT>;
    public:
        OutPtrT merge(const ImagePtrT image, const MappedConstBoostPtrT cloudPair) override {
            auto landmark3d = boost::make_shared<CloudT>();
            auto rawCloud = cloudPair->first;
            auto mapping = cloudPair->second;
            feature::IntraFace featureDetector;
            auto feature = std::make_shared<Feature>(featureDetector.getFeature(*image));
            for (long i=0; i<feature->points.cols(); i++) {
                try {
                    auto pointInd = mapping->getMappedPointId(feature->points(0, i), feature->points(1, i));
                    landmark3d->push_back(rawCloud->at(pointInd));
                } catch (std::out_of_range &e) {
                    std::cout << "WARNING: Landmark Points at Hole." << std::endl;
                }
            }
            landmark3d->height = rawCloud->height;
            landmark3d->width = rawCloud->width;
            std::cout << landmark3d->size() <<std::endl;

            auto result = boost::make_shared<FittingSuite>();
            result->landmark2d = feature;
            result->landmark3d = landmark3d;
            result->rawCloud = rawCloud;
            result->rawImage = image;

            return result;
        }
    };

    /**
     * Just discard image and select PointCloud. Used for debugging
     */
    class DummyMappedImageCloudMerger : public SimpleMappedImageCloudMerger<CloudConstT> {
    private:
        using OutPtrT = const boost::shared_ptr<CloudConstT>;
        using MappedConstBoostPtrT = boost::shared_ptr<MappedCloudConstT>;
    public:
        OutPtrT merge(const ImagePtrT image, const MappedConstBoostPtrT cloud) override {
            return cloud->first;
        }
    };
}
