#include "state_transition_impl.h"

#include "log.h"

TaskState StateTransitionImpl::GetState() const
{
    std::shared_lock lock(mutex_);
    return state_;
}

bool StateTransitionImpl::TryTransition(TaskState to)
{
    std::unique_lock lock(mutex_);
    if (IsValidTransition(to)) {
        TaskState old_state = state_;
        state_ = to;
        lock.unlock();
        OnStateChanged(old_state, to);
        return true;
    }
    return false;
}

void StateTransitionImpl::OnStateChanged(TaskState from, TaskState to)
{
    static const char *TASK_STATE_STR[] = {
        "Created",
        "Initialized",
        "Executing",
        "Completed",
        "Failed",
    };
    LOGV("Task state transition: %s -> %s", TASK_STATE_STR[static_cast<int>(from)],
        TASK_STATE_STR[static_cast<int>(to)]);
}

bool StateTransitionImpl::IsValidTransition(TaskState to) const
{
    switch (state_) {
        case TaskState::Created:
            return to == TaskState::Initialized;
        case TaskState::Initialized:
            return to == TaskState::Executing;
        case TaskState::Executing:
            return to == TaskState::Completed || to == TaskState::Failed;
        case TaskState::Completed:
            return to == TaskState::Initialized;
        default:
            return false;
    }
}