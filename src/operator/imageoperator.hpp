#pragma once


template <typename T>
bool ImageOperator::AddParameter(const T &op, const std::string & category)
{
    bool res = m_paramList.Add(op);
    if (res)
        m_categoryMap[category].push_back(op.name);

    return res;
}

template <typename T>
T const ImageOperator::GetParameter(const std::string &name) const
{
    return m_paramList.Get<T>(name);
}

template <typename T>
bool ImageOperator::SetParameter(const T &op)
{
    if (m_paramList.Set<T>(op)) {
        EmitEvent<Evt::UpdateOp>(op);
        EmitEvent<Evt::UpdateGui>(op);
        EmitEvent<Evt::Update>();
        return true;
    }

    return false;
}