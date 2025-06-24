#include <algorithm>
#include <format>
#include <sstream>
#include "log.h"
#include "test_runner.h"


bool Benchmark::Run()
{
    if (!baseline_task_ || baseline_task_->GetState() != TaskState::Initialized) {
        LOGE("Baseline task is not initialized.");
        return false;
    }

    for (size_t i = 0; i < warmup_iterations_; ++i) {
        baseline_task_->Execute();
        baseline_task_->Reset();
    }
    for (size_t i = 0; i < benchmark_iterations_; ++i) {
        baseline_task_->Execute();
        auto task_duration = baseline_task_->GetDuration();
        baseline_duration_.total_duration += task_duration;
        baseline_duration_.min_duration = std::min(baseline_duration_.min_duration, task_duration);
        baseline_duration_.max_duration = std::max(baseline_duration_.max_duration, task_duration);
        baseline_task_->Reset();
    }
    baseline_duration_.average_duration = baseline_duration_.total_duration / benchmark_iterations_;

    for (const auto &[name, task] : comparison_tasks_) {
        if (task->GetState() != TaskState::Initialized) {
            LOGE("Comparison task %s is not initialized.", name.c_str());
            return false;
        }

        for (size_t i = 0; i < warmup_iterations_; ++i) {
            task->Execute();
            task->Reset();
        }
        for (size_t i = 0; i < benchmark_iterations_; ++i) {
            task->Execute();
            auto task_duration = task->GetDuration();
            comparison_durations_[name].total_duration += task_duration;
            comparison_durations_[name].min_duration = std::min(comparison_durations_[name].min_duration, task_duration);
            comparison_durations_[name].max_duration = std::max(comparison_durations_[name].max_duration, task_duration);
            task->Reset();
        }
        comparison_durations_[name].average_duration =
            comparison_durations_[name].total_duration / benchmark_iterations_;
    }
    return true;
}

std::string Benchmark::GetResultString()
{
    std::string result;
    std::ostringstream oss;
    oss << "================================= Benchmark Result =================================\n";
    oss << std::format("Warmup Iterations : {}\n", warmup_iterations_);
    oss << std::format("Benchmark Iterations : {}\n", benchmark_iterations_);
    oss << "------------------------------------------------------------------------------------\n";
    oss << std::format(
        "| {:<20} | {:<12} | {:<12} | {:<12} | {:<10} |\n", "Task", "Min(ms)", "Max(ms)", "Avg(ms)", "Speedup");
    oss << "------------------------------------------------------------------------------------\n";

    // Baseline
    oss << std::format("| {:<20} | {:12.3f} | {:12.3f} | {:12.3f} | {:>10} |\n", "Baseline",
        baseline_duration_.min_duration.count(), baseline_duration_.max_duration.count(),
        baseline_duration_.average_duration.count(), "-");

    // Comparison tasks
    for (const auto &[name, duration] : comparison_durations_) {
        double speedup = baseline_duration_.average_duration.count() / duration.average_duration.count();
        oss << std::format("| {:<20} | {:12.3f} | {:12.3f} | {:12.3f} | {:10.2f} |\n", name,
            duration.min_duration.count(), duration.max_duration.count(), duration.average_duration.count(), speedup);
    }
    oss << "====================================================================================\n";
    return oss.str();
}