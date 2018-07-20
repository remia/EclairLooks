#pragma once

#include "imageoperatorparameter.h"
#include "../utils/event_source.h"


typedef EventDesc<
    FuncT<void()>,
    FuncT<void(const ImageOperatorParameter &op)>,
    FuncT<void(const ImageOperatorParameter &op)>> IOPEvtDesc;

class ImageOperatorParameterList : public EventSource<IOPEvtDesc>
{
  using VecT = std::vector<UPtr<ImageOperatorParameter>>;

public:
  enum Evt { UpdateAny = 0, UpdateValue, UpdateParam };

public:
  ImageOperatorParameterList() = default;

public:
  using VecIt  = VecT::iterator;
  using VecCIt = VecT::const_iterator;
  VecIt begin();
  VecIt end();
  VecCIt begin() const;
  VecCIt end() const;

public:
  bool HasName(const std::string &name) const;

  template <typename T>
  bool Add(const T &op);

  template <typename T>
  bool Update(const T &op);

  bool Delete(const std::string &name);

  template <typename T>
  T const Get(const std::string &name) const;

  template <typename T, typename V>
  bool Set(const std::string &name, const V &value);

private:
  VecT m_params;
};

#include "imageoperatorparameterlist.hpp"