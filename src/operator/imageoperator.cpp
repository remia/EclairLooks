#include "imageoperator.h"
#include "../imagepipeline.h"
#include "../utils/generic.h"

#include "ociomatrix_operator.h"
#include "ociofiletransform_operator.h"

#include <QtCore/QDebug>
#include <fstream>


ImageOperator::ImageOperator()
{
    using IOP = ImageOperatorParameter;
    using IOPL = ImageOperatorParameterList;

    m_paramList.Add({ "Enabled", IOP::Type::CheckBox, true });
    m_paramList.Add({ "Opacity", IOP::Type::Slider, 100.0f, std::vector<float>{0.0f, 100.0f, 1.0f} } );
    m_paramList.Add({ "Isolate Contrast", IOP::Type::Slider, 0.0f, std::vector<float>{0.0f, 100.0f, 1.0f} } );
    m_paramList.Add({ "Isolate Color", IOP::Type::Slider, 0.0f, std::vector<float>{0.0f, 100.0f, 1.0f} } );

    using std::placeholders::_1;
    m_paramList.Subscribe<IOPL::UpdateValue>(std::bind(&ImageOperator::OpUpdateParamCallback, this, _1));
}

ImageOperatorParameterList & ImageOperator::Parameters()
{
    return m_paramList;
}

ImageOperatorParameterList const & ImageOperator::Parameters() const
{
    return m_paramList;
}

bool ImageOperator::IsIdentity() const
{
    auto enabled = m_paramList.Get<bool>("Enabled");
    return (!enabled.value_or(false) || OpIsIdentity());
}

void ImageOperator::Apply(Image & img)
{
    float isolate_cts = m_paramList.Get<float>("Isolate Contrast").value_or(0.f);
    float isolate_color = m_paramList.Get<float>("Isolate Color").value_or(0.f);

    const Image img_orig = img;

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
        file_op.SetFileTransform("test.spi1d");

        if (isolate_cts != 0.0f) {
            float a = isolate_cts / 100.0f;
            file_op.OpApply(img);
            img = img_orig * (1.0f - a) + img * a;
        }
        if (isolate_color != 0.0f) {
            float a = isolate_color / 100.0f;
            file_op.Parameters().Set("Direction", std::string("Inverse"));
            file_op.OpApply(img);
            OpApply(img);
            img = img_orig * (1.0f - a) + img * a;
        }
    }
    else {
        OpApply(img);
    }

    float opacity = m_paramList.Get<float>("Opacity").value_or(100.f);
    float a = opacity / 100.0f;
    img = img_orig * (1.0f - a) + img * a;
}
