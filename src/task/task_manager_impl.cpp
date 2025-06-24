#include "task_manager_impl.h"

#include <limits>
#include <memory>
#include <ranges>
#include <algorithm>

#include "log.h"
#include "task_impl.h"

std::shared_ptr<Task> TaskManagerImpl::CreateTask() { return std::make_shared<TaskImpl>(); }

uint64_t TaskManagerImpl::Add(std::shared_ptr<Task> task)
{
    std::unique_lock lock(mutex_);
    uint64_t id = GenerateUniqueId();
    tasks_[id] = task;
    task_creation_times_[id] = std::chrono::steady_clock::now();
    return id;
}

bool TaskManagerImpl::Remove(uint64_t task_id)
{
    std::unique_lock lock(mutex_);

    auto task_it = tasks_.find(task_id);
    if (task_it == tasks_.end()) {
        LOGW("Attempted to remove non-existent task ID: %lu", task_id);
        return false;
    }

    TaskState state = task_it->second->GetState();
    // Allow removal of non-terminal tasks, but log warning
    if (state != TaskState::Completed && state != TaskState::Failed) {
        LOGW("Attempted to remove active task ID: %lu (state: %d)", task_id, static_cast<int>(state));
    }

    tasks_.erase(task_it);
    task_creation_times_.erase(task_id);
    recycled_ids_.push(task_id);

    return true;
}

std::shared_ptr<Task> TaskManagerImpl::Get(uint64_t task_id)
{
    std::shared_lock lock(mutex_);
    auto it = tasks_.find(task_id);
    return it != tasks_.end() ? it->second : nullptr;
}

void TaskManagerImpl::CleanupExpiredTasks()
{
    auto now = std::chrono::steady_clock::now();
    std::unique_lock lock(mutex_);

    std::vector<uint64_t> expired_task_ids;

    for (const auto &[task_id, task] : tasks_) {
        const auto &creation_time = task_creation_times_[task_id];
        const auto &task_state = task->GetState();

        if (task_state == TaskState::Completed || task_state == TaskState::Failed ||
            (creation_time + task_timeout_ < now)) {
            expired_task_ids.push_back(task_id);
        }
    }

    // Clean up expired tasks
    for (auto task_id : expired_task_ids) {
        LOGW("Task %lu has expired, removing it", task_id);
        recycled_ids_.push(task_id);
        task_creation_times_.erase(task_id);
        tasks_.erase(task_id);
    }
}

uint64_t TaskManagerImpl::GenerateUniqueId()
{
    if (!recycled_ids_.empty()) {
        uint64_t id = recycled_ids_.front();
        recycled_ids_.pop();
        return id;
    }

    if (next_task_id_ == std::numeric_limits<uint64_t>::max()) {
        LOGW("Task ID counter approaching maximum value, resetting to find available IDs");
        // Start searching for unused IDs from low values
        next_task_id_ = 0;
    }

    uint64_t candidate_id = next_task_id_++;

    // If all possible IDs are in use (extremely unlikely), log error
    uint64_t start_id = candidate_id;
    while (tasks_.contains(candidate_id)) {
        candidate_id = next_task_id_++;
        if (candidate_id == start_id) {
            LOGE("All possible task IDs are in use!");
            throw std::runtime_error("Task ID space exhausted");
        }
    }
    return candidate_id;
}
