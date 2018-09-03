#pragma once

#include <map>

#include "generic.h"


using ConnectIdT = uint16_t;
using EventIdT = uint8_t;

template <typename... T>
using EventDesc = TupleT<std::map<ConnectIdT, T>...>;

struct EventProxy
{
    EventIdT EventId;
    ConnectIdT ConnectionId;
};

template <typename EvtDesc> class EventSource
{
  public:
    using SilentT = std::map<EventIdT, bool>;

    static const EventIdT InvalidEventT = std::numeric_limits<EventIdT>::max();

public:
    EventSource()
    : m_currentId(0)
    {
        for (int i = 0; i < std::tuple_size<EvtDesc>::value; ++i)
            m_silent[i] = false;
    }

  public:
    void Mute(EventIdT id = InvalidEventT)
    {
        for (auto &[k, v] : m_silent)
            if (id == InvalidEventT || k == id) {
                v = true;
            }
    }

    void Unmute(EventIdT id = InvalidEventT)
    {
        for (auto &[k, v] : m_silent)
            if (id == InvalidEventT || k == id)
                v = false;
    }

    template <EventIdT Id, typename F> EventProxy Subscribe(const F &f)
    {
        auto &cbs = std::get<Id>(m_callbacks);
        cbs[m_currentId] = f;

        return EventProxy { Id, m_currentId++ };
    }

    template <EventIdT Id> void Unsubscribe(const EventProxy &ep)
    {
        auto &cbs = std::get<Id>(m_callbacks);
        cbs.erase(ep.ConnectionId);
    }

  protected:
    template <EventIdT Id, typename... Args> void EmitEvent(Args &&... args)
    {
        if (m_silent[Id])
            return;

        for (auto &[coId, f] : std::get<Id>(m_callbacks))
            f(args...);
    }

  private:
    EvtDesc m_callbacks;
    SilentT m_silent;
    ConnectIdT m_currentId;
};

template <typename T> class AutoMute
{
  public:
    AutoMute(T *obj, EventIdT id = T::InvalidEventT)
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
    EventIdT m_id;
};