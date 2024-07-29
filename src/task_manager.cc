#include <iomanip>
#include <sstream>
#include <tgbot/tools/StringTools.h>

#include "task_manager.h"

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
            parsed_deadline, false
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

bool to_do_bot::TaskManager::SetReminderText(const std::string &input)
  {
  Reminder new_reminder
      {
          input
      };

  reminders_[++reminder_counter_] = new_reminder;

  return !reminders_.empty(); //report if creating new reminder was successful
  }
bool to_do_bot::TaskManager::SetReminderTime(const std::string &input)
  {
  std::istringstream input_stream(input);
  std::tm tm_storage{};

  input_stream >> std::get_time(&tm_storage, "%H:%M:%S");

  tm_storage.tm_sec = 0;

  if (input_stream.fail())
    {
    throw std::runtime_error("Failed to parse time string");
    }

  reminders_[reminder_counter_].hours = tm_storage.tm_hour;
  reminders_[reminder_counter_].mins = tm_storage.tm_min;
  reminders_[reminder_counter_].sec = tm_storage.tm_sec;
  }

bool to_do_bot::TaskManager::TimeToRemind(const Reminder &reminder)
  {
  auto now{std::chrono::system_clock::now()};

  const auto &now_time_t{std::chrono::system_clock::to_time_t(now)};

  std::tm now_tm{*std::localtime(&now_time_t)};

  const auto &hours_now{now_tm.tm_hour};
  const auto &mins_now{now_tm.tm_min};
  const auto &sec_now{now_tm.tm_sec};

  return hours_now == reminder.hours && mins_now == reminder.mins && sec_now == reminder.sec;
  }

int to_do_bot::TaskManager::get_task_counter()
  { return task_counter_; }

void to_do_bot::TaskManager::save_chat_id(int64_t id)
  {
  chat_id = id;
  }
int64_t to_do_bot::TaskManager::get_chat_id()
  {
  return chat_id;
  };


/*void TaskManager::MarkTaskAsDone(int taskNumber) {
    if (taskMap.find(taskNumber) != taskMap.end()) {
        taskMap[taskNumber].isDone = true;
    }
}*/