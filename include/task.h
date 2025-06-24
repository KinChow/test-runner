#ifndef TASK_H
#define TASK_H

#include <any>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>

/**
 * @brief Task priority enumeration
 */
enum class Priority { HIGH, MEDIUM, LOW };

/**
 * @brief Task state enumeration
 *
 * Defines various states in the task lifecycle, from creation to completion or failure
 */
enum class TaskState { Created, Initialized, Executing, Completed, Failed };

/**
 * @brief State transition interface class
 *
 * Defines basic operations for state transitions, providing general mechanisms for state management and transitions
 */
class StateTransition {
public:
    StateTransition() = default;
    virtual ~StateTransition() = default;

    /**
     * @brief Get current state
     * @return TaskState Current task state
     */
    virtual TaskState GetState() const = 0;

protected:
    /**
     * @brief Try to transition to specified state
     * @param to Target state
     * @return bool Whether transition succeeded
     */
    virtual bool TryTransition(TaskState to) = 0;

    /**
     * @brief Callback when state changes
     * @param from Original state
     * @param to Target state
     */
    virtual void OnStateChanged(TaskState from, TaskState to) = 0;

    /**
     * @brief Check if state transition is valid
     * @param to Target state
     * @return bool Whether transition is valid
     */
    virtual bool IsValidTransition(TaskState to) const = 0;
};

/**
 * @brief Base task class
 *
 * Provides basic functionality for task initialization, execution and state management, is the core component of the
 * task system
 */
class Task : virtual public StateTransition {
public:
    Task() = default;
    virtual ~Task() = default;

    /**
     * @brief Initialize task with function and arguments
     * @param f Task function
     * @param args Task function arguments
     */
    template <typename F, typename... Args>
    requires std::is_invocable_v<F, Args...>
    void Init(F &&f, Args &&...args)
    {
        if (GetState() != TaskState::Created) {
            return;
        }
        task_ = [func = std::forward<F>(f),
                    args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable -> std::any {
            using ResultType = std::invoke_result_t<F, Args...>;
            if constexpr (std::is_void_v<ResultType>) {
                std::apply(func, args_tuple);
                return {};
            } else {
                return std::apply(func, args_tuple);
            }
        };
        TryTransition(TaskState::Initialized);
    }

    /**
     * @brief Execute task
     * @return std::any Task execution result
     */
    virtual std::any Execute() = 0;

    /**
     * @brief Reset task to initial state
     *
     * This method clears the task function and resets the state to Created, allowing for reinitialization
     */
    virtual void Reset() = 0;

    /**
     * @brief Set task priority
     * @param priority Task priority
     * @return bool Whether setting succeeded
     */
    virtual bool SetPriority(Priority priority) = 0;

    /**
     * @brief Get task priority
     * @return Priority Task priority
     */
    virtual Priority GetPriority() const = 0;

    /**
     * @brief Get task execution duration
     * @return std::chrono::duration<double, std::milli> Execution duration in milliseconds
     */
    virtual std::chrono::duration<double, std::milli> GetDuration() const = 0;

    /**
     * @brief Get current task state
     * @return TaskState Current task state
     */
    virtual TaskState GetState() const = 0;

    // Disable external copy and move
    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;
    Task(Task &&) = delete;
    Task &operator=(Task &&) = delete;

protected:
    std::function<std::any()> task_;
};

/**
 * @brief Task manager class
 *
 * Responsible for task creation, storage and lifecycle management, provides unified task operation interface
 */
class TaskManager {
public:
    TaskManager() = default;
    virtual ~TaskManager() = default;

    /**
     * @brief Create basic task
     * @return std::shared_ptr<Task> Task pointer
     */
    virtual std::shared_ptr<Task> CreateTask() = 0;

    /**
     * @brief Add task to manager
     * @param task Task pointer
     * @return uint64_t Task ID
     */
    virtual uint64_t Add(std::shared_ptr<Task> task) = 0;

    /**
     * @brief Remove task from manager
     * @param task_id Task ID
     * @return bool Whether removal succeeded
     */
    virtual bool Remove(uint64_t task_id) = 0;

    /**
     * @brief Get task by ID
     * @param task_id Task ID
     * @return std::shared_ptr<Task> Task pointer, returns nullptr if not exists
     */
    virtual std::shared_ptr<Task> Get(uint64_t task_id) = 0;

    /**
     * @brief Clean up expired tasks
     */
    virtual void CleanupExpiredTasks() = 0;

    // Disable external copy and move
    TaskManager(const TaskManager &) = delete;
    TaskManager &operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager &operator=(TaskManager &&) = delete;
};

#endif  // TASK_H