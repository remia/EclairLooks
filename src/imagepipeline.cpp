#include "imagepipeline.h"


ImagePipeline::ImagePipeline()
{

}

void ImagePipeline::SetInput(const Image & img)
{
    m_inputImg = img;
    m_outputImg = img;

    for (auto & f : m_resetCallbacks)
        f(m_inputImg);

    Compute();
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

    for (auto & f : m_updateCallbacks)
        f(m_outputImg);
}

void ImagePipeline::RegisterResetCallback(const CallbackT func)
{
    m_resetCallbacks.push_back(func);
}

void ImagePipeline::RegisterUpdateCallback(const CallbackT func)
{
    m_updateCallbacks.push_back(func);
}