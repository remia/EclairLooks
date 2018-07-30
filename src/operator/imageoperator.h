#pragma once

#include <vector>
#include <map>

#include "../utils/event_source.h"
#include "imageoperatorparameterlist.h"


typedef EventDesc<
    FuncT<void()>,
    FuncT<void(const ImageOperatorParameter &op)>,
    FuncT<void(const ImageOperatorParameter &op)>> IOPEvtDesc;

class Image;

class ImageOperator : public EventSource<IOPEvtDesc>
{
  public:
    using CategoryMapT = std::map<std::string, std::vector<std::string>>;
    enum Evt { Update, UpdateOp, UpdateGui };

  public:
    ImageOperator();
    ImageOperator(const ImageOperator &) = default;
    virtual ~ImageOperator() = default;

    virtual std::string OpName() const = 0;
    virtual void OpApply(Image &img) = 0;
    virtual bool OpIsIdentity() const { return true; }
    virtual void OpUpdateParamCallback(const ImageOperatorParameter &op) {}

  public:
    ImageOperatorParameterList & Parameters();
    ImageOperatorParameterList const & Parameters() const;

    std::string DefaultCategory() const;
    CategoryMapT const & Categories() const;

    template <typename T> bool AddParameter(const T &op, const std::string & category = m_defaultCategory);
    bool DeleteParameter(const std::string &name);

    template <typename T> T const GetParameter(const std::string &name) const;
    template <typename T> bool SetParameter(const T &op);

    bool IsIdentity() const;
    void Apply(Image &img);

  private:
    ImageOperatorParameterList m_paramList;

    CategoryMapT m_categoryMap;
    static std::string m_defaultCategory;
};


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