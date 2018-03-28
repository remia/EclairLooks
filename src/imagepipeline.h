#pragma once

#include "image.h"
#include "imagetransformation.h"

#include <memory>


template<class T>
using UPtr = std::unique_ptr<T>;

class ImagePipeline
{
public:
    ImagePipeline();

public:
    void SetInput(Image & img);
    Image & GetOutput();

    void AddTransformation(UPtr<ImageTransformation> transform);

private:
    Image m_inputImg;
    Image m_outputImg;
    std::vector<UPtr<ImageTransformation>> m_transformations;

    bool m_needCompute;
};