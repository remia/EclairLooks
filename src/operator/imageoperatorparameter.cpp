#include "imageoperatorparameter.h"


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

bool ImageOperatorParameterList::Add(const ImageOperatorParameter &op)
{
    if (Get(op.name))
        return false;

    m_params.push_back(op);
    return true;
}

bool ImageOperatorParameterList::Update(const ImageOperatorParameter &op)
{
    if (!Get(op.name))
        return false;

    for (auto & p : m_params) {
        if (p.name == op.name) {
            p = op;
            EmitEvent<Evt::UpdateParam>(p);
        }
    }

    return true;
}

bool ImageOperatorParameterList::Delete(const std::string & name)
{
    if (!Get(name))
        return false;

    std::remove_if(m_params.begin(), m_params.end(), [&](auto &p) {
        return p.name == name;
    });
    return true;
}

ImageOperatorParameterList::OptP const ImageOperatorParameterList::Get(const std::string &name) const
{
    for (auto & p : m_params) {
        if (p.name == name)
            return OptP(p);
    }

    return OptP();
}

bool ImageOperatorParameterList::Set(const std::string &name, const std::any &value)
{
    if (!Get(name))
        return false;

    for (auto & p : m_params) {
        if (p.name == name) {
            p.value = value;
            EmitEvent<Evt::UpdateValue>(p);
            EmitEvent<Evt::UpdateAny>();
        }
    }

    return true;
}