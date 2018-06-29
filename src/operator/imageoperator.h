#pragma once

#include <any>
#include <functional>
#include <vector>

#include "imageoperatorparameter.h"


class Image;
class ImagePipeline;
class QWidget;

class ImageOperator
{
    using CallbackT = std::function<void(const ImageOperatorParameter &op)>;

public:
    ImageOperator();
    ImageOperator(const ImageOperator &) = default;
    virtual ~ImageOperator() = default;

    virtual std::string OpName() const = 0;
    virtual void OpApply(Image & img) = 0;
    virtual bool OpIsIdentity() const { return true; }
    virtual ImageOperatorParameterVec OpExportParams() const { return std::vector<ImageOperatorParameter>(); }
    virtual void OpUpdateParamCallback(const ImageOperatorParameter & op) {}

public:
    bool IsIdentity() const;
    void Apply(Image & img);

    void InitParameters();

    ImageOperatorParameterVec GetParameters() const;
    ImageOperatorParameter * GetParameter(const std::string & name);
    ImageOperatorParameter const * GetParameter(const std::string & name) const;

    // Update from GUI
    void SetParameter(const std::string & name, const std::any & value);
    // Update from MODEL
    void UpdateParameter(const std::string & name, const ImageOperatorParameter & op);

    ImagePipeline * Pipeline() { return m_pipeline; }
    void SetPipeline(ImagePipeline * p) { m_pipeline = p; }

    void RegisterCallback(CallbackT func);

private:
    ImagePipeline * m_pipeline;
    std::vector<CallbackT> m_callbacks;
    ImageOperatorParameterVec m_params;
};