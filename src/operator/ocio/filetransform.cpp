#include "filetransform.h"

#include <sstream>

#include <QtWidgets/QtWidgets>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#include <core/image.h>
#include <core/imagepipeline.h>
#include <utils/chrono.h>

namespace OCIO = OCIO_NAMESPACE;


OCIOFileTransform::OCIOFileTransform()
: m_valid(false)
{
    OCIO::ClearAllCaches();

    m_config = OCIO::GetCurrentConfig();
    m_transform = OCIO::FileTransform::Create();

    QStringList exts = SupportedExtensions();
    QString filter = QString("LUT files (*.%1)").arg(exts.join(" *."));

    AddParameterByCategory<FilePathParameter>("File", "LUT", "", "Choose a LUT", filter.toStdString());
    AddParameterByCategory<SelectParameter>("File", "Interpolation", std::vector<std::string>{"Best", "Nearest", "Linear", "Tetrahedral"}, "Best");
    AddParameterByCategory<SelectParameter>("File", "Direction", std::vector<std::string>{"Forward", "Inverse"}, "Forward");

    // Initialize transform with default parameters
    auto interp = GetParameter<SelectParameter>("Interpolation");
    auto dir = GetParameter<SelectParameter>("Direction");
    m_transform->setInterpolation(OCIO::InterpolationFromString(interp->value().c_str()));
    m_transform->setDirection(OCIO::TransformDirectionFromString(dir->value().c_str()));
}

ImageOperator *OCIOFileTransform::OpCreate() const
{
    return new OCIOFileTransform();
}

ImageOperator *OCIOFileTransform::OpCreateFromPath(const std::string &filepath) const
{
    QStringList exts = SupportedExtensions();

    QFileInfo file = QFileInfo(QString::fromStdString(filepath));
    if (exts.contains(file.suffix(), Qt::CaseInsensitive)) {
        OCIOFileTransform * ft = new OCIOFileTransform();
        ft->SetFileTransform(filepath);
        return ft;
    }

    return nullptr;
}

std::string OCIOFileTransform::OpName() const
{
    return "OCIO File";
}

std::string OCIOFileTransform::OpLabel() const
{
    QFileInfo fileInfo(m_transform->getSrc());
    if (!fileInfo.absoluteFilePath().isEmpty())
        return "FT - " + fileInfo.fileName().toStdString();
    else
        return "FT";
}

std::string OCIOFileTransform::OpDesc() const
{
    std::ostringstream oStr;
    oStr << "OCIO File Transform\n" << *m_transform;
    return oStr.str();
}

void OCIOFileTransform::OpApply(Image & img)
{
    if (not m_valid)
        return;

    Chrono c;
    c.start();

    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_cpu_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }

    qInfo() << "OCIOFileTransform apply - " << fixed << qSetRealNumberPrecision(2)
            << c.ellapsed(Chrono::MILLISECONDS) / 1000.f << "sec.\n";
}

bool OCIOFileTransform::OpIsIdentity() const
{
    return !m_processor || m_processor->isNoOp();
}

void OCIOFileTransform::OpUpdateParamCallback(const Parameter & op)
{
    try {
        if (op.name() == "LUT") {
            OCIO::ClearAllCaches();

            auto p = static_cast<const FilePathParameter *>(&op);
            m_transform->setSrc((p->value().c_str()));
        }
        else if (op.name() == "Interpolation") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setInterpolation(OCIO::InterpolationFromString(p->value().c_str()));
        }
        else if (op.name() == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value().c_str()));
        }

        m_processor = m_config->getProcessor(m_transform);
        m_cpu_processor = m_processor->getOptimizedCPUProcessor(OCIO::OPTIMIZATION_LOSSLESS);
        m_valid = true;
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
        m_valid = false;
    }
}

void OCIOFileTransform::SetFileTransform(const std::string &lutpath)
{
    try {
        Chrono c;
        c.start();

        OCIO::ClearAllCaches();

        auto m = EventMute(this, { UpdateParam, Update });

        GetParameter<FilePathParameter>("LUT")->setValue(lutpath);

        auto interp = GetParameter<SelectParameter>("Interpolation");
        auto dir = GetParameter<SelectParameter>("Direction");
        m_transform->setSrc(lutpath.c_str());
        m_transform->setInterpolation(OCIO::InterpolationFromString(interp->value().c_str()));
        m_transform->setDirection(OCIO::TransformDirectionFromString(dir->value().c_str()));
        m_processor = m_config->getProcessor(m_transform);
        m_cpu_processor = m_processor->getOptimizedCPUProcessor(OCIO::OPTIMIZATION_LOSSLESS);
        m_valid = true;

        qInfo() << "OCIOFileTransform init - (" << QString::fromStdString(lutpath)
                << ") : " << fixed << qSetRealNumberPrecision(2)
                << c.ellapsed(Chrono::MILLISECONDS) << "msec.\n";
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
        m_valid = false;
    }
}

QStringList OCIOFileTransform::SupportedExtensions() const
{
    QStringList exts;
    for (size_t i = 0; i < m_transform->getNumFormats(); ++i)
        exts << m_transform->getFormatExtensionByIndex(i);
    return exts;
}
