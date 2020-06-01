#pragma once

#include <string>
#include <vector>

#include <OpenColorIO/OpenColorIO.h>

#include <QtCore/QStringList>

#include "../imageoperator.h"


class Image;

class OCIOFileTransform : public ImageOperator
{
  public:
    OCIOFileTransform();

  public:
    ImageOperator *OpCreate() const override;
    ImageOperator *OpCreateFromPath(const std::string &filepath) const override;
    std::string OpName() const override;
    std::string OpLabel() const override;
    std::string OpDesc() const override;
    void OpApply(Image &img) override;
    bool OpIsIdentity() const override;
    void OpUpdateParamCallback(const Parameter &op) override;

  public:
    void SetFileTransform(const std::string &lutpath);

  public:
    QStringList SupportedExtensions() const;

  private:
    // NOTE: this may be migrated to parent class if we find this
    // behavior to be wanted by multiples ops, ie. do nothing
    // if the transform is currently invalid instead of potentially
    // applying the last known working transform.
    bool m_valid;

    OCIO_NAMESPACE::ConstConfigRcPtr m_config;
    OCIO_NAMESPACE::ConstProcessorRcPtr m_processor;
    OCIO_NAMESPACE::ConstCPUProcessorRcPtr m_cpu_processor;
    OCIO_NAMESPACE::FileTransformRcPtr m_transform;
};