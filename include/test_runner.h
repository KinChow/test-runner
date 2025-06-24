#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "task.h"
#include <string>
#include <unordered_map>

class Validation {
public:
    Validation() = default;
    virtual ~Validation() = default;

    template <typename F, typename... Args>
    requires std::is_invocable_v<F, Args...>
    bool AddBaselineTask(F &&f, Args &&...args)
    {
        baseline_task_ = std::make_shared<Task>();
        baseline_task_->Init(std::forward<F>(f), std::forward<Args>(args)...);
        return baseline_task_->GetState() == TaskState::Initialized;
    }

    template <typename F, typename... Args>
    requires std::is_invocable_v<F, Args...>
    bool AddComparisonTask(const std::string &name, F &&f, Args &&...args)
    {
        auto task = std::make_shared<Task>();
        task->Init(std::forward<F>(f), std::forward<Args>(args)...);
        if (task->GetState() != TaskState::Initialized) {
            return false;
        }
        comparison_tasks_[name] = task;
        return true;
    }

    virtual bool Run();

private:
    std::shared_ptr<Task> baseline_task_;
    std::unordered_map<std::string, std::shared_ptr<Task>> comparison_tasks_;
};

class Benchmark : public Validation {
public:
    Benchmark(size_t warmup_iterations = 10, size_t benchmark_iterations = 100)
        : warmup_iterations_(warmup_iterations), benchmark_iterations_(benchmark_iterations) {}
    bool Run() override;

private:
    size_t warmup_iterations_ = 10;
    size_t benchmark_iterations_ = 100;
    std::chrono::duration<double, std::milli> baseline_duration_;
    std::unordered_map<std::string, std::chrono::duration<double, std::milli>> comparison_durations_;
};

#endif  // TEST_RUNNER_H