#pragma once

#include <any>
#include <vector>

#include "imageoperatorparameter.h"
#include "../utils/event_source.h"

class Image;
class ImagePipeline;
class QWidget;

enum class ImageOperatorEvent { Update, UpdateGUI };

class ImageOperator : public EventSource<ImageOperatorEvent>
{
  public:
    using UpdateT = FuncT<void()>;
    using UpdateGuiT = FuncT<void(const ImageOperatorParameter &op)>;

  public:
    ImageOperator();
    ImageOperator(const ImageOperator &) = default;
    virtual ~ImageOperator() = default;

    virtual std::string OpName() const = 0;
    virtual void OpApply(Image &img) = 0;
    virtual bool OpIsIdentity() const { return true; }
    virtual ImageOperatorParameterVec OpExportParams() const
    {
        return std::vector<ImageOperatorParameter>();
    }
    virtual void OpUpdateParamCallback(const ImageOperatorParameter &op) {}

  public:
    bool IsIdentity() const;
    void Apply(Image &img);

    void InitParameters();

    ImageOperatorParameterVec GetParameters() const;
    ImageOperatorParameter *GetParameter(const std::string &name);
    ImageOperatorParameter const *GetParameter(const std::string &name) const;

    // Update from GUI
    void SetParameter(const std::string &name, const std::any &value);
    // Update from MODEL
    void UpdateParameter(const std::string &name, const ImageOperatorParameter &op);

  private:
    ImageOperatorParameterVec m_params;
};