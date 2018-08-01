#pragma once

#include <string>
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

  public:
    virtual ImageOperator * OpCreate() const = 0;
    virtual ImageOperator * OpCreateFromPath(const std::string &filepath) const { return nullptr; };
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

#include "imageoperator.hpp"