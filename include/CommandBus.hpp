#pragma once
#include <any>
#include <string>
#include <memory>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include "Commands.hpp"

namespace sas
{

    class CommandBus
    {

        std::unordered_map<std::type_index, std::vector<std::function<void(const ICommand &cmd)>>> commands;
        std::vector<std::unique_ptr<ICommand>> commandQueue;

    public:
        template <typename T>
        void subscribe(std::function<void(const T &)> handler) noexcept
        {
            static_assert(std::is_base_of_v<ICommand, T>, "T must inherit from ICoomand");

            commands[std::type_index(typeid(T))].push_back([handler](const ICommand &baseCmd)
                                                           { handler(static_cast<const T &>(baseCmd)); });
        }

        template <typename T, typename... Args>
        void post(Args &&...args) noexcept
        {
            static_assert(std::is_base_of_v<ICommand, T>, "T must inherit from ICommand");
            commandQueue.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template <typename T>
        void dispatch(T &cmd) noexcept
        {
            static_assert(std::is_base_of_v<ICommand, T>);
            auto it = commands.find(std::type_index(typeid(T)));
            if (it != commands.end())
            {
                for (auto &handler : it->second)
                {
                    handler(cmd);
                }
            }
        }

        void processQueue() noexcept
        {
            for (const auto &cmd : commandQueue)
            {
                auto it = commands.find(std::type_index(typeid(*cmd)));
                if (it != commands.end())
                {
                    for (auto &handler : it->second)
                    {
                        handler(*cmd);
                    }
                }
            }
            commandQueue.clear();
        }
    };

} // namespace sas