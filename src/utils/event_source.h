#pragma once

#include <map>
#include <vector>

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
    EventSource();
    EventSource(const EventSource &rhs) = delete;
    EventSource& operator=(const EventSource &rhs) = delete;

  public:
    void Mute(EventIdT id = InvalidEventT);
    void Unmute(EventIdT id = InvalidEventT);

    template <EventIdT Id, typename F> EventProxy Subscribe(const F &f);
    template <EventIdT Id> void Unsubscribe(const EventProxy &ep);

  protected:
    template <EventIdT Id, typename... Args> void EmitEvent(Args &&... args);

  private:
    EvtDesc m_callbacks;
    SilentT m_silent;
    ConnectIdT m_currentId;
};

template <typename T> class EventMute
{
  public:
    EventMute(T *obj, EventIdT id);
    EventMute(T *obj, std::vector<EventIdT> ids);
    ~EventMute();

  private:
    T *m_obj;
    std::vector<EventIdT> m_ids;
};

#include "event_source.hpp"