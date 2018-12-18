#pragma once

#include <string>
#include "parameter.h"


class FilePathParameter : public Parameter
{
  public:
    enum class PathType {
        File,
        Folder
    };

  public:
    FilePathParameter() = default;

    FilePathParameter(const std::string &name)
        : Parameter(name, Type::FilePath), m_path_type(PathType::File)
    {
    }

    FilePathParameter(const std::string &name, const std::string &value,
                      const std::string &dialog_title = "",
                      const std::string &filters = "", PathType pt = PathType::File)
        : Parameter(name, Type::FilePath), m_value(value), m_description(dialog_title),
          m_filters(filters), m_path_type(pt)
    {
    }

  public:
    std::string value() const { return m_value; }
    void setValue(const std::string &v)
    {
        m_value = v;
        EmitEvent<UpdateValue>(*this);
    }

    std::string description() const { return m_description; }
    void setDescription(const std::string &v)
    {
        m_description = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    std::string filters() const { return m_filters; }
    void setFilters(const std::string &v)
    {
        m_filters = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    PathType pathType() const { return m_path_type; }
    void setPathType(const PathType &v)
    {
        m_path_type = v;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    std::string m_value;
    std::string m_description;
    std::string m_filters;
    PathType m_path_type;
};
