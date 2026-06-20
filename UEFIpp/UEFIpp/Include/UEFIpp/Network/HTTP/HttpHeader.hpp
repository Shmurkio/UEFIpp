#pragma once

#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::Network
{
    class HttpHeader
    {
    public:
        HttpHeader() = default;

        HttpHeader(Library::StringView Name, Library::StringView Value) : Name(Name), Value(Value)
        {
        }

        [[nodiscard]] auto Empty() const -> Foundation::Bool
        {
            return Name.Empty();
        }

        [[nodiscard]] auto Valid() const -> Foundation::Bool
        {
            return !Name.Empty();
        }

    public:
        Library::String Name{};
        Library::String Value{};
    };

    class HttpHeaders
    {
    public:
        [[nodiscard]] auto Add(Library::StringView Name, Library::StringView Value) -> HttpHeaders&
        {
            Headers_.PushBack({ Name, Value });
            return *this;
        }

        [[nodiscard]] auto Remove(Library::StringView Name) -> Foundation::Bool
        {
            for (Foundation::Size Index{}; Index < Headers_.Size(); ++Index)
            {
                if (Headers_[Index].Name.View() == Name)
                {
                    Headers_.Erase(Index);
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] auto Contains(Library::StringView Name) const -> Foundation::Bool
        {
            return !Get(Name).Empty();
        }

        [[nodiscard]] auto Get(Library::StringView Name) const -> Library::StringView
        {
            for (const auto& Header : Headers_)
            {
                if (Header.Name.View() == Name)
                {
                    return Header.Value.View();
                }
            }

            return {};
        }

        [[nodiscard]] auto Count() const -> Foundation::Size
        {
            return Headers_.Size();
        }

        [[nodiscard]] auto begin()
        {
            return Headers_.begin();
        }

        [[nodiscard]] auto end()
        {
            return Headers_.end();
        }

        [[nodiscard]] auto begin() const
        {
            return Headers_.begin();
        }

        [[nodiscard]] auto end() const
        {
            return Headers_.end();
        }

    private:
        Library::Vector<HttpHeader> Headers_{};
    };
}