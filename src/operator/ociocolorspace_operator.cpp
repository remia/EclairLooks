#include "ociocolorspace_operator.h"
#include "../image.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>
#include <iostream>

namespace OCIO = OCIO_NAMESPACE;


OCIOColorSpace::OCIOColorSpace()
{
    Parameters().Add(FilePathParameter("Config File"));
    Parameters().Add(SelectParameter("Source"));
    Parameters().Add(SelectParameter("Destination"));
    Parameters().Add(SelectParameter("Look"));
    Parameters().Add(SelectParameter("Direction", {"Forward", "Inverse"}));

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
    m_config = OCIO::Config::CreateFromFile(configpath.c_str());
    Parameters().Update(FilePathParameter("Config File", configpath));

    std::vector<std::string> colorspaces;
    for (int i = 0; i < m_config->getNumColorSpaces(); i++)
        colorspaces.push_back(m_config->getColorSpaceNameByIndex(i));
    Parameters().Update(SelectParameter("Source", colorspaces));
    Parameters().Update(SelectParameter("Destination", colorspaces));

    std::vector<std::string> looks;
    looks.push_back("");
    for (int i = 0; i < m_config->getNumLooks(); ++i)
        looks.push_back(m_config->getLookNameByIndex(i));
    Parameters().Update(SelectParameter("Look", looks));
}