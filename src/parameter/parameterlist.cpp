#include "parameterlist.h"


ParameterList::VecIt ParameterList::begin()
{
    return m_params.begin();
}

ParameterList::VecIt ParameterList::end()
{
    return m_params.end();
}

ParameterList::VecCIt ParameterList::begin() const
{
    return m_params.begin();
}

ParameterList::VecCIt ParameterList::end() const
{
    return m_params.end();
}

bool ParameterList::HasName(const std::string &name) const
{
    for (auto &p : m_params) {
        if (p->name == name)
            return true;
    }

    return false;
}

bool ParameterList::Delete(const std::string & name)
{
    if (!HasName(name))
        return false;

    std::remove_if(m_params.begin(), m_params.end(), [&](auto &p) {
        return p->name == name;
    });

    return true;
}

bool ParameterList::Set(const Parameter &op)
{
    if (!HasName(op.name))
        return false;

    for (auto &p : m_params)
        if (p->name == op.name)
            *p = op;


    return true;
}