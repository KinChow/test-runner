#ifndef TASK_IMPL_H
#define TASK_IMPL_H

#include <atomic>

#include "state_transition_impl.h"
#include "task.h"

class TaskImpl
    : public StateTransitionImpl
    , virtual public Task {
public:
    TaskImpl() = default;
    virtual ~TaskImpl() = default;
    std::any Execute() override;
    void Reset() override;
    bool SetPriority(Priority priority) override;
    Priority GetPriority() const override;
    std::chrono::duration<double, std::milli> GetDuration() const override;
    TaskState GetState() const override;

protected:
    std::chrono::duration<double, std::milli> duration_;
    Priority priority_ = Priority::MEDIUM;
};

#endif  // TASK_IMPL_H