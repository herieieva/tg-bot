#include "task_manager.h"

#include <cstdio>
#include <thread>

#include <tgbot/tgbot.h>

int main()
  {
  TgBot::Bot ToDoBot("6251096313:AAG7P0F5O2-cqlnrS-qfLUTFf4cIne9bAHE");

  to_do_bot::TaskManager task_manager;

  // Welcome message
  ToDoBot.getEvents().onCommand(
      "start", [&ToDoBot, &task_manager](const TgBot::Message::Ptr &message)
        {
        ToDoBot.getApi().sendMessage(
            message->chat->id,
            "Hi! Use me as your personal task manager :) \n"
            "Let’s start with your current To-Do list!\n\n"
            "To write 1 task to the list, select add_task command below");

        task_manager.save_chat_id(message->chat->id);
        });

  // Function to prompt the user to add a task
  ToDoBot.getEvents().onCommand("add_task", [&ToDoBot, &task_manager](
      const TgBot::Message::Ptr
      &message)
    {
    ToDoBot.getApi().sendMessage(
        message->chat->id, "Please send tasks one by one as in this example:\n"
                           "To send the email - 30.07.24 18:00");
    task_manager.expecting_task_ = true;

    // Capture any message and parse it as a task if the bot is waiting for a
    // task
    ToDoBot.getEvents().onNonCommandMessage(
        [&ToDoBot, &task_manager](const TgBot::Message::Ptr &message)
          {
          if (task_manager.AddTask(message->text, message->chat->id))
            {
            ToDoBot.getApi().sendMessage(
                message->chat->id, "Success! Here is your to-do list: \n");
            for (const auto &task : task_manager.tasks_)
              {
              ToDoBot.getApi().sendMessage(
                  message->chat->id, std::to_string(task.first) + ". " +
                      task.second.description + " - " +
                      task.second.deadline_text + " " +
                      task.second.GetStatus());
              }
            } else
            {
            ToDoBot.getApi().sendMessage(
                message->chat->id,
                "Invalid task format. Please use 'description - deadline' \n "
                "Example: \n "
                "To send the email - 20.07.24 15:00");
            }

          task_manager.expecting_task_ =
              false; // Reset the flag after receiving the task
          });
    });

  std::thread my_reminders([&ToDoBot, &task_manager]()
                             {
                             while (true)
                               {
                               if (!task_manager.tasks_.empty())
                                 {
                                 for (const auto &current_task : task_manager.tasks_)
                                   {
                                   if (task_manager.IsDeadlineSoon(current_task.second))
                                     {
                                     ToDoBot.getApi().sendMessage(
                                         task_manager.get_chat_id(),
                                         "Hey, buddy! The deadline for this task is soon: \n" +
                                             std::to_string(current_task.first) + ". " +
                                             current_task.second.description + " - " +
                                             current_task.second.deadline_text + " " +
                                             current_task.second.GetStatus());

                                     std::this_thread::sleep_for(std::chrono::minutes(9));
                                     }
                                   }
                                 std::this_thread::sleep_for(std::chrono::minutes(1));
                                 }

                               if (!task_manager.reminders_.empty())
                                 {
                                 const auto &last_reminder = task_manager.reminders_.rbegin()->second;
                                 if (last_reminder.hours != 0 && last_reminder.mins != 0)
                                   {
                                   for (const auto &curr_reminder : task_manager.reminders_)
                                     {
                                     if (task_manager.TimeToRemind(curr_reminder.second))
                                       {
                                       ToDoBot.getApi().sendMessage(task_manager.get_chat_id(),
                                                                    "Notification! "
                                                                        + curr_reminder.second.description);
                                       std::this_thread::sleep_for(std::chrono::seconds(30));
                                       }
                                     }
                                   }
                                 }
                               }
                             });

  my_reminders.detach();

  // Asks about notification description and then what time to send it everyday
  ToDoBot.getEvents().onCommand(
      "set_daily_reminder",
      [&ToDoBot, &task_manager](const TgBot::Message::Ptr &message1)
        {
        bool exp_reminder{true};

        ToDoBot.getApi().sendMessage(
            message1->chat->id, "Please provide a short text for the reminder\n");
        if (exp_reminder)
          {
          ToDoBot.getEvents().onNonCommandMessage([&ToDoBot, &task_manager,
                                                      &exp_reminder](
              const TgBot::Message::Ptr
              &message2)
                                                    {
                                                    if (exp_reminder && task_manager.SetReminderText(message2->text))
                                                      {
                                                      ToDoBot.getApi().sendMessage(
                                                          message2->chat->id,
                                                          "Fantastic! Now write the time you'd like to receive the "
                                                          "reminder as in this example: 15:30\n");
                                                      }

                                                    ToDoBot.getEvents().onAnyMessage([&ToDoBot, &task_manager,
                                                                                         &exp_reminder](
                                                        const TgBot::Message::Ptr
                                                        &message3)
                                                                                       {
                                                                                       if (task_manager.SetReminderTime(
                                                                                           message3->text))
                                                                                         {
                                                                                         exp_reminder = false;
                                                                                         ToDoBot.getApi().sendMessage(
                                                                                             message3->chat->id,
                                                                                             "Success! You will receive the reminder at the set time.");
                                                                                         }
                                                                                       });
                                                    });
          }
        });

  // help command - simple text
  ToDoBot.getEvents().onCommand("help", [&ToDoBot](const TgBot::Message::Ptr
                                                   &message)
    {
    ToDoBot.getApi().sendMessage(
        message->chat->id,
        "Your speaking with bot should start from /start command :)\n"
        "To-do bot provides a list of available commands. You can access "
        "this list by typing / in the chat or pressing the '/' or 'menu' "
        "button\n"
        "/start command wakes up the bot and sends welcome message :)\n"
        "/add_task command is our favourite - it receives your input "
        "(please follow the example strictly) and will carefully numerate it, "
        "give a cute emoji status and store it. But it`s not all! It will also "
        "remind you about the task 10 minutes before the deadline.\n");
    });

  // about command - simple text
  ToDoBot.getEvents().onCommand(
      "about", [&ToDoBot](const TgBot::Message::Ptr &message)
        {
        ToDoBot.getApi().sendMessage(
            message->chat->id,
            "This is a pet project bot that is useful for time and task "
            "management. Created in 2024 by @Hereieiva.");
        });

  ToDoBot.getEvents().onCommand("task_done", [&ToDoBot, &task_manager](const TgBot::Message::Ptr
                                                                       &message)
    {
    if (!task_manager.tasks_.empty())
      {
      const auto &chat_id{task_manager.get_chat_id()};
      ToDoBot.getApi().sendMessage(chat_id, "Here is your to-do list: ");
      ToDoBot.getApi().sendMessage(chat_id, task_manager.ShowToDoList());
      ToDoBot.getApi().sendMessage(chat_id,
                                   "Write the number of the task to mark it as done, for example: 1");

      ToDoBot.getEvents().onNonCommandMessage([&ToDoBot, &task_manager](const TgBot::Message::Ptr
                                                                        &message)
                                                {
                                                task_manager.MarkTaskAsDone(message->text);
                                                ToDoBot.getApi().sendMessage(task_manager.get_chat_id(),
                                                                             "Success! Here is your to-do list: ");
                                                ToDoBot.getApi().sendMessage(task_manager.get_chat_id(),
                                                                             task_manager.ShowToDoList());
                                                });
      } else
      {
      ToDoBot.getApi().sendMessage(task_manager.get_chat_id(),
                                   "There are no tasks. You have to add a task first to change it`s status");
      }
    });

  try
    {
    printf("Bot username: %s\n", ToDoBot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(ToDoBot);
    while (true)
      {
      printf("Long poll started\n");
      longPoll.start();
      }
    } catch (TgBot::TgException &e)
    {
    printf("Error: %s\n", e.what());
    }

  return 0;
  }
