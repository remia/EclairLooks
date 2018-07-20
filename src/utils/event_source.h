#pragma once

#include <tuple>
#include <map>

#include "generic.h"


template <typename... T>
using EventDesc = std::tuple<std::vector<T>...>;

template <typename EvtDesc> class EventSource
{
  public:
    using EventT = uint8_t;
    using EventDescT = EvtDesc;
    using SilentT = std::map<EventT, bool>;

    static const EventT InvalidEventT = std::numeric_limits<EventT>::max();

public:
    EventSource()
    {
        for (int i = 0; i < std::tuple_size<EvtDesc>::value; ++i)
            m_silent[i] = false;
    }

  public:
    void Mute(EventT id = InvalidEventT)
    {
        for (auto &[k, v] : m_silent)
            if (id == InvalidEventT || k == id) {
                v = true;
            }
    }

    void Unmute(EventT id = InvalidEventT)
    {
        for (auto &[k, v] : m_silent)
            if (id == InvalidEventT || k == id)
                v = false;
    }

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
        if (m_silent[Id])
            return;

        for (auto &f : std::get<Id>(m_callbacks))
            f(args...);
    }

  private:
    EventDescT m_callbacks;
    SilentT m_silent;
};

template <typename T> class AutoMute
{
  public:
    AutoMute(T *obj, typename T::EventT id = T::InvalidEventT)
    : m_obj(obj)
    , m_id(id)
    {
        m_obj->Mute(m_id);
    }

    ~AutoMute()
    {
        m_obj->Unmute(m_id);
    }

  private:
    T *m_obj;
    typename T::EventT m_id;
};