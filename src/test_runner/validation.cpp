#include "log.h"
#include "task_manager_impl.h"
#include "test_runner.h"

Validation::Validation() { task_manager_ = std::make_unique<TaskManagerImpl>(); }

bool Validation::Run()
{
    auto &baseline_task = baseline_task_.second;
    if (!baseline_task || baseline_task->GetState() != TaskState::Initialized) {
        LOGE("Baseline task is not initialized.");
        return false;
    }

    baseline_task->Execute();
    for (const auto &[name, task] : comparison_tasks_) {
        if (task->GetState() != TaskState::Initialized) {
            LOGE("Comparison task %s is not initialized.", name.c_str());
            return false;
        }
        task->Execute();
    }
    return true;
}