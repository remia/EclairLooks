#pragma once

#include "image.h"
#include "utils/event_source.h"
#include "operator/imageoperator.h"

#include <vector>


enum class ImagePipelineEvent { Reset, Update };

class ImagePipeline : public EventSource<ImagePipelineEvent>
{
  public:
    using EventT  = ImagePipelineEvent;
    using ResetT  = FuncT<void(const Image &img)>;
    using UpdateT = FuncT<void(const Image &img)>;

  public:
    ImagePipeline();

  public:
    void SetInput(const Image &img);
    Image &GetOutput();

    template <typename T>
    T *AddTransformation()
    {
        using OP = ImageOperator;

        m_transformations.emplace_back(new T());
        m_transformations.back()->InitParameters();
        m_transformations.back()->Subscribe<OP::UpdateT>(OP::EventT::Update, std::bind(&ImagePipeline::Compute, this));
        return static_cast<T *>(m_transformations.back().get());
    }

    void Compute();
    void ExportLUT(const std::string &filename, uint32_t size);

  private:
    Image m_inputImg;
    Image m_outputImg;
    std::vector<UPtr<ImageOperator>> m_transformations;
};