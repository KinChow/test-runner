#ifndef TASK_MANAGER_IMPL_H
#define TASK_MANAGER_IMPL_H

#include <chrono>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>

#include "task.h"

class TaskManagerImpl : public TaskManager {
public:
  TaskManagerImpl() = default;

  explicit TaskManagerImpl(std::chrono::seconds task_timeout)
      : task_timeout_(task_timeout) {}

  ~TaskManagerImpl() override = default;

  std::shared_ptr<Task> CreateTask() override;

  uint64_t Add(std::shared_ptr<Task> task) override;

  bool Remove(uint64_t task_id) override;

  std::shared_ptr<Task> Get(uint64_t task_id) override;

  void CleanupExpiredTasks() override;

private:
  uint64_t GenerateUniqueId();

private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<uint64_t, std::shared_ptr<Task>> tasks_;
  std::unordered_map<uint64_t, std::chrono::steady_clock::time_point>
      task_creation_times_;
  std::queue<uint64_t> recycled_ids_;
  uint64_t next_task_id_{0};
  std::chrono::milliseconds task_timeout_{std::chrono::minutes(10)};
};

#endif // TASK_MANAGER_IMPL_H