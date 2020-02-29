#include "imageoperator.h"

#include <QtCore/QDebug>

#include <utils/generic.h>
#include <utils/chrono.h>
#include <core/imagepipeline.h>
#include <core/transform.h>


ImageOperator::ImageOperator()
{
    AddParameter<CheckBoxParameter>("Enabled", true);
    AddParameter<SliderParameter>("Opacity", 100.0f, 0.0f, 100.0f, 1.0f);
    AddParameter<SliderParameter>("Contrast", 100.0f, 0.0f, 100.0f, 1.0f);
    AddParameter<SliderParameter>("Color", 100.0f, 0.0f, 100.0f, 1.0);
    // AddParameter<SliderParameter>("Log", 100.0f, 0.0001f, 10000.0f, 1.0f, SliderParameter::Scale::Log);

    using std::placeholders::_1;
    Subscribe<UpdateParam>(std::bind(&ImageOperator::OpUpdateParamCallback, this, _1));
}

ParameterList & ImageOperator::Parameters()
{
    return m_paramList;
}

ParameterList const & ImageOperator::Parameters() const
{
    return m_paramList;
}

std::string ImageOperator::DefaultCategory() const
{
    return m_defaultCategory;
}

ImageOperator::CategoryMapT const & ImageOperator::Categories() const
{
    return m_categoryMap;
}

void ImageOperator::UpdatedParameter(const Parameter &p)
{
    // Order matters here, first give a chance to the operator to update
    // it's internal processing state.
    EmitEvent<Evt::UpdateParam>(p);
    // Then emit the event that will throw a pipeline update...
    EmitEvent<Evt::Update>();
}

bool ImageOperator::IsIdentity() const
{
    auto enabled = m_paramList.Get<CheckBoxParameter>("Enabled")->value();
    return (!enabled || OpIsIdentity());
}

void ImageOperator::Apply(Image & img)
{
    using std::placeholders::_1;

    const Image img_orig = img;

    float contrast = m_paramList.Get<SliderParameter>("Contrast")->value() / 100.f;
    float color = m_paramList.Get<SliderParameter>("Color")->value() / 100.f;
    IsolateColorContrast(img, std::bind(&ImageOperator::OpApply, this, _1), color, contrast);

    float opacity = m_paramList.Get<SliderParameter>("Opacity")->value() / 100.f;
    if(opacity != 1.0) {
        float a = opacity;
        img = img_orig * (1.0f - a) + img * a;
    }
}
