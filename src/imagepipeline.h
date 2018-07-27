#pragma once

#include "image.h"
#include "utils/event_source.h"
#include "operator/imageoperator.h"

#include <vector>


typedef EventDesc<
    FuncT<void(const Image &img)>,
    FuncT<void(const Image &img)>> IPEvtDesc;

class ImagePipeline : public EventSource<IPEvtDesc>
{
  public:
    enum Evt { Reset = 0, Update };

  public:
    ImagePipeline();

  public:
    void SetInput(const Image &img);
    Image &GetOutput();

    ImageOperator &GetOperator(int index);

    template <typename T> T *AddTransformation();

    void Compute();
    void ExportLUT(const std::string &filename, uint32_t size);

  private:
    Image m_inputImg;
    Image m_outputImg;
    std::vector<UPtr<ImageOperator>> m_transformations;
};


template <typename T>
T * ImagePipeline::AddTransformation()
{
    UPtr<ImageOperator> & op = m_transformations.emplace_back(new T());
    op->Subscribe<ImageOperator::Update>(std::bind(&ImagePipeline::Compute, this) );
    return static_cast<T *>(op.get());
}