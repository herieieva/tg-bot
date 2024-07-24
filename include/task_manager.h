#ifndef TO_DO_BOT_INCLUDE_TASK_MANAGER_H
#define TO_DO_BOT_INCLUDE_TASK_MANAGER_H

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace to_do_bot
  {
  struct Task
    {
    [[nodiscard]] std::string GetStatus() const
      { return isDone ? "✅" : "💡"; };

    std::string description;
    std::string deadline_text;

    std::chrono::system_clock::time_point deadline_chrono;

    int64_t chat_id;

    bool isDone;
    };

  class TaskManager
    {
   public:
    bool AddTask(const std::string &input,
                 const int64_t &chat_id); // numerates, parses, stores
    // input and sets undone status

    bool IsDeadlineSoon(
        const Task &task); // checks if deadline is in 10 or fewer minutes

    int get_task_counter();

    std::map<int, Task> tasks_; // to store to-do list

    bool expecting_task_;

    // void MarkTaskAsDone(int task_number); // in process

   private:
    int task_counter_{1};
    std::vector<std::string> desc_dead_buffer_; // to temporarily store parsed
    // task description and deadline
    };
  } // namespace to_do_bot

#endif // TO_DO_BOT_INCLUDE_TASK_MANAGER_H