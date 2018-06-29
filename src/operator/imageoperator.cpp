#include "imageoperator.h"
#include "../imagepipeline.h"

#include "ociomatrix_operator.h"
#include "ociofiletransform_operator.h"

#include <QtCore/QDebug>
#include <iostream>
#include <fstream>


template<typename T> const T& constant(T& _) { return const_cast<const T&>(_); }
template<typename T> T& variable(const T& _) { return const_cast<T&>(_); }


ImageOperator::ImageOperator()
:   m_pipeline(nullptr)
{

}

bool ImageOperator::IsIdentity() const
{
    auto enabled = GetParameter("Enabled");
    return (!enabled || !std::any_cast<bool>(enabled->value) || OpIsIdentity());
}

void ImageOperator::Apply(Image & img)
{
    float isolate_cts = std::any_cast<float>(GetParameter("Isolate Contrast")->value);
    float isolate_color = std::any_cast<float>(GetParameter("Isolate Color")->value);

    Image img_orig = img;

    if (isolate_cts != 0.0f || isolate_color != 0.0f) {

        // Compute 1D LUT
        Image img_1d = Image::Ramp1D(65535, 0.0f, 1.0f, RampType::NEUTRAL);
        OpApply(img_1d);

        std::ofstream fs("test.spi1d");

        fs << "Version 1\n";
        fs << "From 0.000000 1.000000\n";
        fs << "Length 65535\n";
        fs << "Components 3\n";
        fs << "{\n";

        float * pix = img_1d.pixels_asfloat();
        for (uint64_t i = 0; i < 65535; ++i) {
            fs << pix[i * 3] << "\t" << pix[i * 3 + 1] << "\t" << pix[i * 3 + 2] << "\n";
        }
        fs << "}\n";
        fs.close();

        // Apply 1D LUT
        OCIOFileTransform file_op;
        file_op.InitParameters();
        file_op.SetFileTransform("test.spi1d");

        if (isolate_cts != 0.0f) {
            float a = isolate_cts / 100.0f;
            file_op.OpApply(img);
            img = img_orig * (1.0f - a) + img * a;
        }
        if (isolate_color != 0.0f) {
            float a = isolate_color / 100.0f;
            file_op.SetParameter("Direction", std::string("Inverse"));
            file_op.OpApply(img);
            OpApply(img);
            img = img_orig * (1.0f - a) + img * a;
        }
    }
    else {
        OpApply(img);
    }
}

void ImageOperator::InitParameters()
{
    m_params = {
        { ImageOperatorParameter::Type::CheckBox, "Enabled", true },
        { ImageOperatorParameter::Type::Slider, "Isolate Contrast", 0.0f, std::vector<float>{0.0f, 100.0f, 1.0f} },
        { ImageOperatorParameter::Type::Slider, "Isolate Color", 0.0f, std::vector<float>{0.0f, 100.0f, 1.0f} },
    };

    ImageOperatorParameterVec customParams = OpExportParams();
    m_params.insert(m_params.end(), customParams.begin(), customParams.end());
}

ImageOperatorParameterVec ImageOperator::GetParameters() const
{
    return m_params;
}

ImageOperatorParameter * ImageOperator::GetParameter(const std::string & name)
{
    return (ImageOperatorParameter *)(constant(*this).GetParameter(name));
}

ImageOperatorParameter const * ImageOperator::GetParameter(const std::string & name) const
{
    auto it = std::find_if(m_params.begin(), m_params.end(), [=](auto & elem){
        return (elem.name == name);
    });

    if (m_params.end() != it)
        return &*it;

    return nullptr;
}

void ImageOperator::SetParameter(const std::string & name, const std::any & value)
{
    qInfo() << QString::fromStdString(OpName()) << "-" << QString::fromStdString(name) << "updated by event !";

    auto param = GetParameter(name);
    if (param) {
        param->value = value;

        // NOTE : Base update logic here...
        // ....
        // Custom update logic here...
        OpUpdateParamCallback(*param);
    }

    if (Pipeline())
        Pipeline()->Compute();
}

void ImageOperator::UpdateParameter(const std::string & name, const ImageOperatorParameter & op)
{
    qInfo() << QString::fromStdString(OpName()) << "-" << QString::fromStdString(name) << "updated by code !";

    auto param = GetParameter(name);
    if (param)
        *param = op;

    for (auto &func : m_callbacks)
        func(*param);

    if (Pipeline())
        Pipeline()->Compute();
}

void ImageOperator::RegisterCallback(CallbackT func) { m_callbacks.push_back(func); }
