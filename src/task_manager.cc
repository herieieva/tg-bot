#include <iomanip>
#include <sstream>
#include <tgbot/tools/StringTools.h>

#include "../include/task_manager.h"

bool to_do_bot::TaskManager::AddTask(const std::string &input,
                                     const int64_t &chat_id)
  {
  StringTools::split(input, '-', desc_dead_buffer_);

  auto expected_buffer_size{TaskManager::get_task_counter() * 2};

  if (TaskManager::desc_dead_buffer_.size() == expected_buffer_size)
    {
    std::tm time_components{};

    std::istringstream ss(
        TaskManager::desc_dead_buffer_[expected_buffer_size -
            1]); // extract the last deadline
    ss >> std::get_time(&time_components, "%d.%m.%y %H:%M:%S");

    time_components.tm_year = 2024 - 1900; // default 2024 year
    time_components.tm_hour += 3;          // GMT +3
    time_components.tm_isdst = -1;         // whether DST is in effect

    auto time_storage{std::mktime(&time_components)};
    if (time_storage == -1)
      {
      throw std::runtime_error("Failed to convert tm to time_t");
      }
    auto parsed_deadline{std::chrono::system_clock::from_time_t(time_storage)};

    Task newTask
        {
            TaskManager::desc_dead_buffer_[expected_buffer_size - 2],
            TaskManager::desc_dead_buffer_[expected_buffer_size - 1],
            parsed_deadline, chat_id, false
        };
    tasks_[task_counter_++] = newTask;
    return true;
    }

  return false;
  }

bool to_do_bot::TaskManager::IsDeadlineSoon(const Task &task)
  {
  auto now{std::chrono::system_clock::now() + std::chrono::hours(3)};
  auto deadline{task.deadline_chrono};

  return !task.isDone && deadline <= now + std::chrono::minutes(10);
  }

int to_do_bot::TaskManager::get_task_counter()
  { return task_counter_; }

/*void TaskManager::MarkTaskAsDone(int taskNumber) {
    if (taskMap.find(taskNumber) != taskMap.end()) {
        taskMap[taskNumber].isDone = true;
    }
}*/