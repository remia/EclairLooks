#include "imageoperatorparameterlist.h"


ImageOperatorParameterList::VecIt ImageOperatorParameterList::begin()
{
    return m_params.begin();
}

ImageOperatorParameterList::VecIt ImageOperatorParameterList::end()
{
    return m_params.end();
}

ImageOperatorParameterList::VecCIt ImageOperatorParameterList::begin() const
{
    return m_params.begin();
}

ImageOperatorParameterList::VecCIt ImageOperatorParameterList::end() const
{
    return m_params.end();
}

bool ImageOperatorParameterList::HasName(const std::string &name) const
{
    for (auto &p : m_params) {
        if (p->name == name)
            return true;
    }

    return false;
}

bool ImageOperatorParameterList::Delete(const std::string & name)
{
    if (!HasName(name))
        return false;

    std::remove_if(m_params.begin(), m_params.end(), [&](auto &p) {
        return p->name == name;
    });

    return true;
}
