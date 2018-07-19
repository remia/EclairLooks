#pragma once

#include <tuple>

#include "generic.h"


template <typename... T>
using EventDesc = std::tuple<std::vector<T>...>;

template <typename EvtDesc> class EventSource
{
  public:
    typedef uint8_t EventT;
    typedef EvtDesc EventDescT;

  public:
    template <EventT Id, typename F> void Subscribe(const F &f)
    {
        auto &vec = std::get<Id>(m_callbacks);
        vec.push_back(f);
    }

    template <EventT Id, typename F> void Unsubscribe(const EventT id, const F &f)
    {
        auto &vec = std::get<Id>(m_callbacks);
        std::remove(vec.begin(), vec.end(), f);
    }

  protected:
    template <EventT Id, typename... Args> void EmitEvent(Args &&... args)
    {
        for (auto &f : std::get<Id>(m_callbacks))
            f(args...);
    }

  private:
    EventDescT m_callbacks;
};