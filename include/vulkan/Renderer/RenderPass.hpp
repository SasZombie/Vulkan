#pragma once
#include <string>

namespace sas
{

    class IRenderPass
    {
    private:
        std::string name;

    public:
        IRenderPass(const std::string &passName) noexcept
            : name(passName)
        {
        }

        virtual void record() const noexcept = 0;

        virtual ~IRenderPass() = default;
    };

    class MainScenePass final : public IRenderPass
    {
    public:
        MainScenePass(const std::string &passName) noexcept
            : IRenderPass(passName)
        {
        }

        void record() const noexcept override;

        ~MainScenePass() noexcept = default;
    };

} // namespace sas