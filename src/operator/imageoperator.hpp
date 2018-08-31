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