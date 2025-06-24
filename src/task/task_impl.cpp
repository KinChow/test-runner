#include "task_impl.h"

#include <any>
#include <chrono>

#include "log.h"

std::any TaskImpl::Execute()
{
    if (StateTransitionImpl::GetState() != TaskState::Initialized) {
        LOGE("Task is not initialized.");
        return {};
    }
    StateTransitionImpl::TryTransition(TaskState::Executing);
    auto start = std::chrono::high_resolution_clock::now();
    std::any result;
    if (!task_) {
        result = {};
        LOGE("Task function is empty.");
        StateTransitionImpl::TryTransition(TaskState::Failed);
        return result;
    }
    result = task_();
    auto end = std::chrono::high_resolution_clock::now();
    duration_ = end - start;
    StateTransitionImpl::TryTransition(TaskState::Completed);
    return result;
}

void TaskImpl::Reset()
{
    StateTransitionImpl::TryTransition(TaskState::Initialized);
}

bool TaskImpl::SetPriority(Priority priority)
{
    if (StateTransitionImpl::GetState() == TaskState::Created ||
        StateTransitionImpl::GetState() == TaskState::Initialized) {
        priority_ = priority;
        return true;
    }
    return false;
}

Priority TaskImpl::GetPriority() const { return priority_; }

std::chrono::duration<double, std::milli> TaskImpl::GetDuration() const
{
    return StateTransitionImpl::GetState() == TaskState::Completed ? duration_
                                                                   : std::chrono::duration<double, std::milli>(0);
}

TaskState TaskImpl::GetState() const { return StateTransitionImpl::GetState(); }