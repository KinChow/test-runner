#include <algorithm>
#include <iomanip>
#include <sstream>
#include "log.h"
#include "test_runner.h"

bool Benchmark::Run()
{
    auto &baseline_task = baseline_task_.second;
    if (!baseline_task || baseline_task->GetState() != TaskState::Initialized) {
        LOGE("Baseline task is not initialized.");
        return false;
    }

    for (size_t i = 0; i < warmup_iterations_; ++i) {
        baseline_task->Execute();
        baseline_task->Reset();
    }
    for (size_t i = 0; i < benchmark_iterations_; ++i) {
        baseline_task->Execute();
        auto task_duration = baseline_task->GetDuration();
        baseline_duration_.total_duration += task_duration;
        baseline_duration_.min_duration = std::min(baseline_duration_.min_duration, task_duration);
        baseline_duration_.max_duration = std::max(baseline_duration_.max_duration, task_duration);
        baseline_task->Reset();
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
    oss << "Warmup Iterations : " << warmup_iterations_ << "\n";
    oss << "Benchmark Iterations : " << benchmark_iterations_ << "\n";
    oss << "------------------------------------------------------------------------------------\n";

    oss << "| " << std::left << std::setw(20) << "Task"
        << " | " << std::setw(12) << "Min(ms)"
        << " | " << std::setw(12) << "Max(ms)"
        << " | " << std::setw(12) << "Avg(ms)"
        << " | " << std::setw(10) << "Speedup"
        << " |\n";
    oss << "------------------------------------------------------------------------------------\n";

    // Baseline
    oss << "| " << std::left << std::setw(20) << baseline_task_.first << " | " << std::right << std::setw(12)
        << std::fixed << std::setprecision(3) << baseline_duration_.min_duration.count() << " | " << std::setw(12)
        << baseline_duration_.max_duration.count() << " | " << std::setw(12)
        << baseline_duration_.average_duration.count() << " | " << std::setw(10) << "-"
        << " |\n";

    // Comparison tasks
    for (const auto &[name, duration] : comparison_durations_) {
        double speedup = baseline_duration_.average_duration.count() / duration.average_duration.count();
        oss << "| " << std::left << std::setw(20) << name << " | " << std::right << std::setw(12) << std::fixed
            << std::setprecision(3) << duration.min_duration.count() << " | " << std::setw(12)
            << duration.max_duration.count() << " | " << std::setw(12) << duration.average_duration.count() << " | "
            << std::setw(10) << std::setprecision(2) << speedup << " |\n";
    }
    oss << "====================================================================================\n";
    return oss.str();
}