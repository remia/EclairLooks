#pragma once

#include "utils/generic.h"
#include "parameter/parameterlist.h"


class QSettings;

class Settings
{
  public:
    Settings();
    ~Settings();

  public:
    const ParameterList & Parameters() const;

    template <typename T> T* const Get(const std::string &name) const;

  private:
    void LoadParameters();
    void SaveParameters();

    void Load(Parameter &p);
    void Save(const Parameter &p);

  private:
    UPtr<QSettings> m_settings;
    ParameterList m_paramList;
};

template <typename T>
T* const Settings::Get(const std::string &name) const
{
    return m_paramList.Get<T>(name);
}