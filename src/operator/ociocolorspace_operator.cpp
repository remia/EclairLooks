#include "ociocolorspace_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

namespace OCIO = OCIO_NAMESPACE;


OCIOColorSpace::OCIOColorSpace()
{
    AddParameter(FilePathParameter("Config File", "", "Choose an OpenColorIO config", "OCIO Config (*.ocio)"), "ColorSpace Transform");
    AddParameter(SelectParameter("Source"), "ColorSpace Transform");
    AddParameter(SelectParameter("Destination"), "ColorSpace Transform");
    AddParameter(SelectParameter("Look"), "ColorSpace Transform");
    AddParameter(SelectParameter("Direction", {"Forward", "Inverse"}), "ColorSpace Transform");

    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::LookTransform::Create();
}

ImageOperator *OCIOColorSpace::OpCreate() const
{
    return new OCIOColorSpace();
}

ImageOperator *OCIOColorSpace::OpCreateFromPath(const std::string &filepath) const
{
    QFileInfo file = QFileInfo(QString::fromStdString(filepath));
    if (file.completeSuffix() == "ocio") {
        OCIOColorSpace * ct = new OCIOColorSpace();
        ct->SetConfig(filepath);
        return ct;
    }

    return nullptr;
}

std::string OCIOColorSpace::OpName() const
{
    return "OCIO ColorSpace";
}

void OCIOColorSpace::OpApply(Image & img)
{
    try {
        OCIO::PackedImageDesc imgDesc(img.pixels_asfloat(), img.width(), img.height(), img.channels());
        m_processor->apply(imgDesc);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Process Error: " << exception.what() << "\n";
    }
}

bool OCIOColorSpace::OpIsIdentity() const
{
    return m_processor->isNoOp();
}

void OCIOColorSpace::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Config File") {
            auto p = static_cast<const FilePathParameter *>(&op);
            SetConfig(p->value);
        }
        else if (op.name == "Source") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setSrc(p->value.c_str());
        }
        else if (op.name == "Destination") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDst(p->value.c_str());
        }
        else if (op.name == "Look") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setLooks(p->value.c_str());
        }
        else if (op.name == "Direction") {
            auto p = static_cast<const SelectParameter *>(&op);
            m_transform->setDirection(OCIO::TransformDirectionFromString(p->value.c_str()));
        }

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOColorSpace::SetConfig(const std::string &configpath)
{
    {
        auto m = AutoMute(this, UpdateOp);
        m_config = OCIO::Config::CreateFromFile(configpath.c_str());
        SetParameter(FilePathParameter("Config File", configpath));
    }

    std::vector<std::string> colorspaces;
    for (int i = 0; i < m_config->getNumColorSpaces(); i++)
        colorspaces.push_back(m_config->getColorSpaceNameByIndex(i));
    SetParameter(SelectParameter("Source", colorspaces));
    SetParameter(SelectParameter("Destination", colorspaces));

    std::vector<std::string> looks;
    looks.push_back("");
    for (int i = 0; i < m_config->getNumLooks(); ++i)
        looks.push_back(m_config->getLookNameByIndex(i));
    SetParameter(SelectParameter("Look", looks));
}