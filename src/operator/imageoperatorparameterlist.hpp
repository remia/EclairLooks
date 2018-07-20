
template <typename T>
bool ImageOperatorParameterList::Add(const T &op)
{
    if (HasName(op.name))
        return false;

    m_params.push_back(std::make_unique<T>(op));
    return true;
}

template <typename T>
bool ImageOperatorParameterList::Update(const T &op)
{
    if (!HasName(op.name))
        return false;

    for (auto &p : m_params) {
        if (p->name == op.name) {
            T * param = static_cast<T *>(p.get());
            *param = op;

            EmitEvent<Evt::UpdateParam>(*p);
        }
    }

    return true;
}

template <typename T>
T const ImageOperatorParameterList::Get(const std::string &name) const
{
    for (auto &p : m_params) {
        if (p->name == name)
            return *static_cast<const T*>(p.get());
    }

    return T();
}

template <typename T, typename V>
bool ImageOperatorParameterList::Set(const std::string &name, const V &value)
{
    if (!HasName(name))
        return false;

    for (auto &p : m_params) {
        if (p->name == name) {
            ((T *)p.get())->value = value;

            EmitEvent<Evt::UpdateValue>(*p);
            EmitEvent<Evt::UpdateAny>();
        }
    }

    return true;
}