#include "imagepipeline.h"


ImagePipeline::ImagePipeline()
: m_needCompute(true)
{

}

void ImagePipeline::SetInput(Image & img)
{
    m_inputImg = img;
    m_outputImg = img;
    m_needCompute = true;
}

Image & ImagePipeline::GetOutput()
{
    if (m_needCompute) {
        m_outputImg = m_inputImg;
        for (auto & t : m_transformations)
            t->Apply(m_outputImg);
    }

    return m_outputImg;
}

void ImagePipeline::AddTransformation(UPtr<ImageTransformation> transform)
{
    m_transformations.emplace_back(std::move(transform));
}
