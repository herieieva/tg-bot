#include <cstdio>
#include <thread>

#include <tgbot/tgbot.h>

#include "task_manager.h"

int main()
  {
  TgBot::Bot ToDoBot("6251096313:AAG7P0F5O2-cqlnrS-qfLUTFf4cIne9bAHE");

  to_do_bot::TaskManager task_manager;

  // Welcome message
  ToDoBot.getEvents().onCommand(
      "start", [&ToDoBot](const TgBot::Message::Ptr &message)
        {
        ToDoBot.getApi().sendMessage(
            message->chat->id,
            "Hi! Use me as your personal task manager :) \n"
            "Let’s start with your current To-Do list!\n\n"
            "To write 1 task to the list, select add_task command below");
        });

  // Function to prompt the user to add a task
  ToDoBot.getEvents().onCommand("add_task", [&ToDoBot, &task_manager](
      const TgBot::Message::Ptr
      &message)
    {
    ToDoBot.getApi().sendMessage(
        message->chat->id, "Please send tasks one by one as in this example:\n"
                           "To send the email - 20.07.24 15:00");
    task_manager.expecting_task_ = true;

    // Capture any message and parse it as a task if the bot is waiting for a
    // task
    ToDoBot.getEvents().onAnyMessage([&ToDoBot, &task_manager](
        const TgBot::Message::Ptr &message)
                                       {
                                       if (task_manager.expecting_task_)
                                         {
                                         if (task_manager.AddTask(message->text, message->chat->id))
                                           {
                                           ToDoBot.getApi().sendMessage(message->chat->id,
                                                                        "Success! Here is your to-do list: \n");
                                           for (const auto &task : task_manager.tasks_)
                                             {
                                             ToDoBot.getApi().sendMessage(message->chat->id,
                                                                          std::to_string(task.first) + ". " +
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
                                         }
                                       });
    });

  std::thread my_deadlines([&ToDoBot, &task_manager]()
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
                                         current_task.second.chat_id,
                                         "Hey, buddy! The deadline for this task is soon: \n" +
                                             std::to_string(current_task.first) + ". " +
                                             current_task.second.description + " - " +
                                             current_task.second.deadline_text + " " +
                                             current_task.second.GetStatus());
                                     }
                                   }
                                 }
                               std::this_thread::sleep_for(std::chrono::minutes(1));
                               }
                             });

  my_deadlines.detach();

  //help command - simple text
  ToDoBot.getEvents().onCommand("help", [&ToDoBot](const TgBot::Message::Ptr
                                                   &message)
    {
    ToDoBot.getApi().sendMessage(
        message->chat->id,
        "To-do bot provides a list of available commands. You can access "
        "this list by typing / in the chat or pressing the '/' or 'menu' "
        "button\n"
        "/start command wakes up the bot and sends welcome message :)\n"
        "/add_task command is our favourite - it receives your input "
        "(please follow the example strictly) and will carefully numerate it, "
        "give a cute emoji status and store it. But it`s not all! It will also "
        "remind you about the task 10 minutes before the deadline.\n");
    });

//about command - simple text
  ToDoBot.getEvents().onCommand("about", [&ToDoBot](const TgBot::Message::Ptr
                                                   &message)
    {
    ToDoBot.getApi().sendMessage(
        message->chat->id,
        "This is a pet project bot that is useful for time and task management. Created in 2024 by @Hereieiva.");
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