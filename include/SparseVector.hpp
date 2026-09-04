#pragma once
#include <vector>
#include <cstdint>
#include <type_traits>

namespace sas
{

    template <typename T>
    concept Numeric = std::is_integral_v<T>;

    template <typename T, Numeric N = uint32_t>
    class SparseVector
    {
    private:
        N nullIndex = std::numeric_limits<N>::max();
        std::vector<T> data;
        std::vector<N> sparse;
        std::vector<N> dense;

    public:
        constexpr SparseVector() noexcept = default;
        constexpr SparseVector(const SparseVector &&) noexcept = default;
        constexpr SparseVector(SparseVector &) noexcept = default;
        constexpr SparseVector &operator=(const SparseVector &) noexcept = default;
        constexpr SparseVector &operator=(SparseVector &&) noexcept = default;
        constexpr ~SparseVector() noexcept = default;

        [[nodiscard]] constexpr const T *getItem(N itemId) const noexcept
        {
            if (!itemExists(itemId))
            {
                return nullptr;
            }
            return &data[sparse[itemId]];
        }

        [[nodiscard]] constexpr T *getItem(N itemId) noexcept
        {
            if (!itemExists(itemId))
            {
                return nullptr;
            }
            return &data[sparse[itemId]];
        }

        [[nodiscard]] constexpr T &operator[](N itemId) noexcept
        {
            return data[sparse[itemId]];
        }

        [[nodiscard]] constexpr const T &operator[](N itemId) const noexcept
        {
            return data[sparse[itemId]];
        }

        // This vector IS the owner of the objects
        // But it should not handle the IDs of such objects
        constexpr void addElement(const T &element, N newId) noexcept
        {
            T copy = element;
            addElement(std::move(copy), newId);
        }

        constexpr void addElement(T &&element, N newId) noexcept
        {
            N internal = static_cast<N>(data.size());
            N sparseSize = static_cast<N>(sparse.size());

            if (newId >= sparseSize)
            {
                sparse.resize(newId + 1, nullIndex);
            }

            sparse[newId] = internal;
            dense.emplace_back(newId);
            data.push_back(std::move(element));
        }

        constexpr void removeItem(N itemId) noexcept
        {
            if (!data.size())
            {
                return;
            }

            if (!itemExists(itemId))
            {
                return;
            }

            size_t indToRemove = sparse[itemId];
            size_t lastIndex = data.size() - 1;

            N lastID = dense[lastIndex];

            if (indToRemove != lastIndex)
            {
                data[indToRemove] = std::move(data[lastIndex]);

                sparse[lastID] = indToRemove;
                dense[indToRemove] = lastID;
            }

            data.pop_back();
            dense.pop_back();

            sparse[itemId] = nullIndex;
        }

        [[nodiscard]] constexpr inline bool itemExists(N itemId) const noexcept
        {
            [[unlikely]] if (itemId >= static_cast<N>(sparse.size()))
            {
                return false;
            }

            [[unlikely]] if (sparse[itemId] == nullIndex)
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] constexpr std::optional<T> removeAndGet(N itemId) noexcept
        {
            if (!itemExists(itemId))
            {
                return std::nullopt;
            }

            const std::size_t indToRemove = sparse[itemId];
            const std::size_t lastIndex = data.size() - 1;
            const N lastID = dense[lastIndex];

            std::optional<T> result{std::move(data[indToRemove])};
            if (indToRemove != lastIndex)
            {
                data[indToRemove] = std::move(data[lastIndex]);
                sparse[lastID] = static_cast<N>(indToRemove);
                dense[indToRemove] = lastID;
            }

            data.pop_back();
            dense.pop_back();
            sparse[itemId] = nullIndex;

            return result;
        }

        [[nodiscard]] constexpr inline const std::vector<T> &getAllElements() const noexcept
        {
            return data;
        }

        [[nodiscard]] constexpr inline std::vector<T> &getAllElements() noexcept
        {
            return data;
        }

        [[nodiscard]] constexpr inline size_t size() const noexcept
        {
            return data.size();
        }
    };
} // namespace sas