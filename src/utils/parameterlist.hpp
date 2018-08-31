#pragma once


template <typename T>
bool ParameterList::Add(const T &op)
{
    if (HasName(op.name))
        return false;

    m_params.push_back(std::make_unique<T>(op));
    return true;
}

template <typename T>
T const ParameterList::Get(const std::string &name) const
{
    for (auto &p : m_params)
        if (p->name == name)
            return *static_cast<const T*>(p.get());

    return T();
}

template <typename T>
bool ParameterList::Set(const T &op)
{
    if (!HasName(op.name))
        return false;

    for (auto &p : m_params)
        if (p->name == op.name) {
            T *param = static_cast<T*>(p.get());
            *param = op;
        }

    return true;
}