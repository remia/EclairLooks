#include "imageoperator.h"
#include "../imagepipeline.h"
#include "../utils/generic.h"

#include "ociomatrix_operator.h"
#include "ociofiletransform_operator.h"

#include <fstream>

#include <QtCore/QDebug>


std::string ImageOperator::m_defaultCategory = "Global";

ImageOperator::ImageOperator()
{
    AddParameter(CheckBoxParameter{ "Enabled", true });
    AddParameter(SliderParameter{ "Opacity", 100.0f, 0.0f, 100.0f, 1.0f });
    AddParameter(SliderParameter{ "Isolate Contrast", 0.0f, 0.0f, 100.0f, 1.0f });
    AddParameter(SliderParameter{ "Isolate Color", 0.0f, 0.0f, 100.0f, 1.0f });

    using std::placeholders::_1;
    Subscribe<UpdateOp>(std::bind(&ImageOperator::OpUpdateParamCallback, this, _1));
}

ImageOperatorParameterList & ImageOperator::Parameters()
{
    return m_paramList;
}

ImageOperatorParameterList const & ImageOperator::Parameters() const
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


bool ImageOperator::DeleteParameter(const std::string &name)
{
    return m_paramList.Delete(name);
}

bool ImageOperator::IsIdentity() const
{
    auto enabled = m_paramList.Get<CheckBoxParameter>("Enabled").value;
    return (!enabled || OpIsIdentity());
}

void ImageOperator::Apply(Image & img)
{
    float isolate_cts = m_paramList.Get<SliderParameter>("Isolate Contrast").value;
    float isolate_color = m_paramList.Get<SliderParameter>("Isolate Color").value;

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
            auto direction = file_op.GetParameter<SelectParameter>("Direction");
            direction.value = "Inverse";
            file_op.SetParameter(direction);
            file_op.OpApply(img);
            OpApply(img);

            float a = isolate_color / 100.0f;
            img = img_orig * (1.0f - a) + img * a;
        }
    }
    else {
        OpApply(img);
    }

    float opacity = m_paramList.Get<SliderParameter>("Opacity").value;
    float a = opacity / 100.0f;
    img = img_orig * (1.0f - a) + img * a;
}
