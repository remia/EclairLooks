#include "ociocolorspace_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include <iostream>

namespace OCIO = OCIO_NAMESPACE;


OCIOColorSpace::OCIOColorSpace()
{
    m_config = OCIO::GetCurrentConfig();
    m_processor = OCIO::Processor::Create();
    m_transform = OCIO::LookTransform::Create();
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

ImageOperatorParameterVec OCIOColorSpace::OpExportParams() const
{
    return
    {
        { ImageOperatorParameter::Type::FilePath, "Config File" },
        { ImageOperatorParameter::Type::Select, "Source" },
        { ImageOperatorParameter::Type::Select, "Destination" },
        { ImageOperatorParameter::Type::Select, "Look" },
        { ImageOperatorParameter::Type::Select, "Direction", "Forward", std::vector<std::string>{"Forward", "Inverse"} }
    };
}

void OCIOColorSpace::OpUpdateParamCallback(const ImageOperatorParameter & op)
{
    try {
        if (op.name == "Config File")
            SetConfig(std::any_cast<std::string>(op.value));
        else if (op.name == "Source")
            m_transform->setSrc(std::any_cast<std::string>(op.value).c_str());
        else if (op.name == "Destination")
            m_transform->setDst(std::any_cast<std::string>(op.value).c_str());
        else if (op.name == "Look")
            m_transform->setLooks(std::any_cast<std::string>(op.value).c_str());
        else if (op.name == "Direction")
            m_transform->setDirection(OCIO::TransformDirectionFromString(std::any_cast<std::string>(op.value).c_str()));

        m_processor = m_config->getProcessor(m_transform);
    } catch (OCIO::Exception &exception) {
        qWarning() << "OpenColorIO Setup Error: " << exception.what() << "\n";
    }
}

void OCIOColorSpace::SetConfig(const std::string &configpath)
{
    m_config = OCIO::Config::CreateFromFile(configpath.c_str());
    UpdateParameter("Config File", { ImageOperatorParameter::Type::FilePath, "Config File", configpath });

    std::vector<std::string> colorspaces;
    for (int i = 0; i < m_config->getNumColorSpaces(); i++)
        colorspaces.push_back(m_config->getColorSpaceNameByIndex(i));
    UpdateParameter("Source", { ImageOperatorParameter::Type::Select, "Source", "0", colorspaces });
    UpdateParameter("Destination", { ImageOperatorParameter::Type::Select, "Destination", "0", colorspaces });

    std::vector<std::string> looks;
    for (int i = 0; i < m_config->getNumLooks(); ++i)
        looks.push_back(m_config->getLookNameByIndex(i));
    UpdateParameter("Look", { ImageOperatorParameter::Type::Select, "Look", "0", looks });
}