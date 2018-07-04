#pragma once

#include "image.h"
#include "operator/imageoperator.h"

#include <vector>
#include <memory>
#include <functional>


class ImagePipeline
{
    template<class T>
    using UPtr = std::unique_ptr<T>;

    using CallbackT = std::function<void(const Image &img)>;

public:
    ImagePipeline();

public:
    void SetInput(const Image & img);
    Image & GetOutput();

    template <typename T>
    T * AddTransformation()
    {
        m_transformations.emplace_back(new T());
        m_transformations.back()->InitParameters();
        m_transformations.back()->SetPipeline(this);
        return static_cast<T*>(m_transformations.back().get());
    }

    void Compute();

    void RegisterResetCallback(const CallbackT func);
    void RegisterUpdateCallback(const CallbackT func);

private:
    Image m_inputImg;
    Image m_outputImg;
    std::vector<UPtr<ImageOperator>> m_transformations;
    std::vector<CallbackT> m_resetCallbacks;
    std::vector<CallbackT> m_updateCallbacks;
};