#include "imagepipeline.h"


ImagePipeline::ImagePipeline()
{

}

void ImagePipeline::SetInput(const Image & img)
{
    m_inputImg = img;
    m_outputImg = img;
}

Image & ImagePipeline::GetOutput()
{
    return m_outputImg;
}

void ImagePipeline::Compute()
{
    m_outputImg = m_inputImg;
    for (auto & t : m_transformations)
        if (!t->IsIdentity())
            t->Apply(m_outputImg);

    for (auto & f : m_callbacks)
        f(m_outputImg);
}

void ImagePipeline::RegisterCallback(const CallbackT func)
{
    m_callbacks.push_back(func);
}