#include "test_runner.h"

bool Validation::Run()
{
    if (!baseline_task_ || baseline_task_->GetState() != TaskState::Initialized) {
        return false;
    }

    baseline_task_->Execute();
    for (const auto &pair : comparison_tasks_) {
        const auto &task = pair.second;
        if (task->GetState() != TaskState::Initialized) {
            return false;
        }

        task->Execute();
    }
    return true;
}