#pragma once
#include <memory>
#include <functional>
#include <boost/shared_ptr.hpp>

namespace telef::io {
    /**
     * A Step in Data Pipeline for Channel
     *
     * Can be constructed from a sequence of several Pipes.
     */
    template <class InT, class OutT>
    class Pipe {
    public:
        Pipe(){
            this->processData = std::bind(&Pipe::_processData, this, std::placeholders::_1);
        }

        template<class NextOutT>
        Pipe<InT, NextOutT> then(std::unique_ptr<Pipe<OutT, NextOutT>> nextPipe){
            auto nextProcessData = [this,nextPipe{std::move(nextPipe)}] (InT in)->NextOutT {
                return nextPipe->processData(this->processData(in));
            };
            return Pipe{std::move(nextProcessData)};
        }

        using FuncT = std::function<boost::shared_ptr<OutT>(boost::shared_ptr<InT>)>;

        FuncT processData;
    private:
        // Default data process method
        virtual boost::shared_ptr<OutT> _processData(boost::shared_ptr<InT> in)=0;
        explicit Pipe(FuncT processData) {
            this->processData = std::move(processData);
        }
    };

    /**
     * A Simple Step That Does Nothing on The Input Data
     */
    template <class InT>
    class IdentityPipe : public Pipe<InT, InT> {
        boost::shared_ptr<InT> _processData(boost::shared_ptr<InT> in) override {
            return in;
        }
    };

}



