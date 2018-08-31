#pragma once

#include "generic.h"
#include "parameter.h"


class ParameterList
{
  using VecT = std::vector<UPtr<Parameter>>;

public:
  ParameterList() = default;

public:
  using VecIt  = VecT::iterator;
  using VecCIt = VecT::const_iterator;
  VecIt begin();
  VecIt end();
  VecCIt begin() const;
  VecCIt end() const;

public:
  template <typename T> bool Add(const T &op);
  bool Delete(const std::string &name);

  template <typename T> T const Get(const std::string &name) const;
  template <typename T> bool Set(const T &op);
  bool Set(const Parameter &op);

private:
  bool HasName(const std::string &name) const;

private:
  VecT m_params;
};

#include "parameterlist.hpp"