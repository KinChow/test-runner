#ifndef STATE_TRANSITION_IMPL_H
#define STATE_TRANSITION_IMPL_H

#include <mutex>
#include <shared_mutex>

#include "task.h"

class StateTransitionImpl : virtual public StateTransition {
public:
    StateTransitionImpl() = default;
    virtual ~StateTransitionImpl() = default;

    TaskState GetState() const override;

protected:
    bool TryTransition(TaskState to) override;

    void OnStateChanged(TaskState from, TaskState to) override;

    bool IsValidTransition(TaskState to) const override;

protected:
    mutable std::shared_mutex mutex_;
    TaskState state_ = TaskState::Created;
};

#endif  // STATE_TRANSITION_IMPL_H