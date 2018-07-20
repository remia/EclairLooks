#pragma once

#include "../utils/generic.h"
#include "imageoperatorparameter.h"


class ImageOperatorParameterList
{
  using VecT = std::vector<UPtr<ImageOperatorParameter>>;

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
  template <typename T> bool Add(const T &op);
  bool Delete(const std::string &name);

  template <typename T> T const Get(const std::string &name) const;
  template <typename T> bool Set(const T &op);

private:
  bool HasName(const std::string &name) const;

private:
  VecT m_params;
};

#include "imageoperatorparameterlist.hpp"