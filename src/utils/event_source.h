#pragma once

#include <map>
#include <any>
#include <functional>
#include <iostream>
#include <memory>

#include "generic.h"


template <typename EventT>
class EventSource
{
  public:
    template <typename F> void Subscribe(EventT id, const F &callback)
    {
        try {
            auto &vec = std::any_cast<std::vector<F> &>(m_callbacks[id]);
            vec.push_back(callback);
        } catch (std::bad_any_cast &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    template <typename F> void Unsubscribe(EventT id, const F &callback)
    {
        try {
            auto &vec = std::any_cast<std::vector<F> &>(m_callbacks[id]);
            std::remove(vec.begin(), vec.end(), callback);
        } catch (std::bad_any_cast &e) {
            std::cerr << e.what() << std::endl;
        }
    }

  protected:
    template <typename F> void RegisterEvent(EventT id)
    {
        m_callbacks[id] = std::vector<F>();
    }

    template <typename... Args> void EmitEvent(const EventT id, Args&&... args)
    {
        try {
            // std::cerr << "Param Type : " << type_name< Args&&... >() << std::endl;
            auto &vec = std::any_cast<std::vector<FuncT<void(Args...)>> &>(m_callbacks[id]);
            for (auto &f : vec)
                f(args...);
        } catch (std::bad_any_cast &e) {
            std::cerr << e.what() << std::endl;
        }
    }

  private:
    std::map<EventT, std::any> m_callbacks;
};