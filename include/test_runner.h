#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <string>
#include <unordered_map>
#include "task.h"

class Validation {
public:
    Validation();
    virtual ~Validation() = default;

    template <typename F, typename... Args>
        requires std::is_invocable_v<F, Args...>
    bool AddBaselineTask(const std::string &name, F &&f, Args &&...args)
    {
        baseline_task_ = std::make_pair(name, task_manager_->CreateTask());
        auto &baseline_task = baseline_task_.second;
        baseline_task->Init(std::forward<F>(f), std::forward<Args>(args)...);
        return baseline_task->GetState() == TaskState::Initialized;
    }

    template <typename F, typename... Args>
        requires std::is_invocable_v<F, Args...>
    bool AddComparisonTask(const std::string &name, F &&f, Args &&...args)
    {
        auto task = task_manager_->CreateTask();
        task->Init(std::forward<F>(f), std::forward<Args>(args)...);
        if (task->GetState() != TaskState::Initialized) {
            return false;
        }
        comparison_tasks_[name] = task;
        return true;
    }

    virtual bool Run();

protected:
    std::unique_ptr<TaskManager> task_manager_;
    std::pair<std::string, std::shared_ptr<Task>> baseline_task_;
    std::unordered_map<std::string, std::shared_ptr<Task>> comparison_tasks_;
};

struct Duration {
    std::chrono::duration<double, std::milli> total_duration = std::chrono::duration<double, std::milli>::zero();
    std::chrono::duration<double, std::milli> average_duration = std::chrono::duration<double, std::milli>::zero();
    std::chrono::duration<double, std::milli> min_duration = std::chrono::duration<double, std::milli>::max();
    std::chrono::duration<double, std::milli> max_duration = std::chrono::duration<double, std::milli>::zero();
};

class Benchmark : public Validation {
public:
    Benchmark(size_t warmup_iterations = 10, size_t benchmark_iterations = 100)
        : warmup_iterations_(warmup_iterations), benchmark_iterations_(benchmark_iterations)
    {}
    bool Run() override;
    std::string GetResultString();

private:
    size_t warmup_iterations_ = 10;
    size_t benchmark_iterations_ = 100;
    Duration baseline_duration_;
    std::unordered_map<std::string, Duration> comparison_durations_;
};

#endif  // TEST_RUNNER_H