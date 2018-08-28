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
    enum Evt { NewInput = 0, Update };

  public:
    ImagePipeline();

  public:
    void SetInput(const Image &img);
    Image &GetInput();
    Image &GetOutput();

    uint8_t OperatorCount() const;
    ImageOperator &GetOperator(uint8_t index);

    void AddOperator(ImageOperator * op);
    template <typename T> T * AddOperator();
    bool DeleteOperator(uint8_t index);

    void Reset();

    void Compute();
    void ExportLUT(const std::string &filename, uint32_t size);

  private:
    Image m_inputImg;
    Image m_outputImg;

    std::vector<UPtr<ImageOperator>> m_operators;
};


template <typename T>
T * ImagePipeline::AddOperator()
{
    UPtr<ImageOperator> & op = m_operators.emplace_back(new T());
    op->Subscribe<ImageOperator::Update>(std::bind(&ImagePipeline::Compute, this) );

    Compute();

    return static_cast<T *>(op.get());
}