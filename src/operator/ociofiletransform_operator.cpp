#include "ociofiletransform_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOFileTransform::OCIOFileTransform()
{
    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::FileTransform::Create();

    QStringList exts = SupportedExtensions();
    QString filter = QString("LUT files (*.%1)").arg(exts.join(" *."));

    AddParameter(FilePathParameter("LUT", "", "Choose a LUT", filter.toStdString()), "File Transform");
    AddParameter(SelectParameter("Interpolation", {"Best", "Nearest", "Linear", "Tetrahedral"}), "File Transform");
    AddParameter(SelectParameter("Direction", {"Forward", "Inverse"}), "File Transform");

    m_transform->setInterpolation(OCIO::InterpolationFromString("Best"));
    m_transform->setDirection(OCIO::TransformDirectionFromString("Forward"));
}

ImageOperator *OCIOFileTransform::OpCreate() const
{
    return new OCIOFileTransform();
}

ImageOperator *OCIOFileTransform::OpCreateFromPath(const std::string &filepath) const
{
    QStringList exts = SupportedExtensions();

    QFileInfo file = QFileInfo(QString::fromStdString(filepath));
    if (exts.contains(file.completeSuffix())) {
        OCIOFileTransform * ft = new OCIOFileTransform();
        ft->SetFileTransform(filepath);
        return ft;
    }

    return nullptr;
}

std::string OCIOFileTransform::OpName() const
{
    return "OCIO File Transform";
}

void OCIOFileTransform::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOFileTransform::OpIsIdentity() const
{
    return m_processor->isNoOp();
}

void OCIOFileTransform::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "LUT") {
            auto p = static_cast<const FilePathParameter *>(&op);
            m_transform->setSrc((p->value.c_str()));
        }
        else if (op.name == "Interpolation") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setInterpolation(OCIO::InterpolationFromString(p->value.c_str()));
        }
        else if (op.name == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value.c_str()));
        }

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
        m_processor = OCIO::Processor::Create();
    }
}

QStringList OCIOFileTransform::SupportedExtensions() const
{
    QStringList exts;
    for (size_t i = 0; i < m_transform->getNumFormats(); ++i)
        exts << m_transform->getFormatExtensionByIndex(i);
    return exts;
}

void OCIOFileTransform::SetFileTransform(const std::string &lutpath)
{
    try {
        auto m = AutoMute(this, UpdateOp);

        auto interp = GetParameter<SelectParameter>("Interpolation");
        auto dir = GetParameter<SelectParameter>("Direction");
        m_transform->setSrc(lutpath.c_str());
        m_transform->setInterpolation(OCIO::InterpolationFromString(interp.value.c_str()));
        m_transform->setDirection(OCIO::TransformDirectionFromString(dir.value.c_str()));
        m_processor = m_config->getProcessor(m_transform);

        SetParameter(FilePathParameter("LUT", lutpath));
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}