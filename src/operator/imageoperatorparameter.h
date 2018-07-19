#pragma once

#include <string>
#include <vector>
#include <any>
#include <optional>

#include "../utils/event_source.h"


template <typename T>
using OptT = std::optional<T>;

template <typename T>
OptT<T> GetAny(const std::any & any) {
    try {
        return OptT<T>(std::any_cast<T>(any));
    }
    catch(std::bad_any_cast &e) {
    }

    return OptT<T>();
}

struct ImageOperatorParameter
{
    enum class Type {
        Text,
        Select,
        FilePath,
        Button,
        CheckBox,
        Slider,
        Matrix4x4,
    };

    std::string name;
    Type type;

    std::any value;
    std::any values;
    std::any default_val;

    operator bool () const { return name != ""; }
};


typedef EventDesc<
    FuncT<void()>,
    FuncT<void(const ImageOperatorParameter &op)>,
    FuncT<void(const ImageOperatorParameter &op)>> IOPEvtDesc;

class ImageOperatorParameterList : public EventSource<IOPEvtDesc>
{
  using VecT = std::vector<ImageOperatorParameter>;
  using VecIt = VecT::iterator;
  using VecCIt = VecT::const_iterator;
  using OptP = OptT<ImageOperatorParameter>;

public:
  enum Evt { UpdateAny = 0, UpdateValue, UpdateParam };

public:
  ImageOperatorParameterList() = default;

public:
  VecIt begin();
  VecIt end();
  VecCIt begin() const;
  VecCIt end() const;

public:
  bool Add(const ImageOperatorParameter &op);
  bool Update(const ImageOperatorParameter &op);
  bool Delete(const std::string &name);

  OptP const Get(const std::string &name) const;
  template <typename T> OptT<T> const Get(const std::string &name) const;
  bool Set(const std::string &name, const std::any &value);

private:
  VecT m_params;
};


template <typename T>
OptT<T> const ImageOperatorParameterList::Get(const std::string &name) const
{
    try {
        if (auto p = Get(name))
            return OptT<T>(std::any_cast<T>(p->value));
    } catch (std::bad_any_cast &e) {

    }

    return OptT<T>();
}