#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Network
{
    class FtpResponse
    {
    public:
        Foundation::Uint32 Code{};
        Library::String Message{};
        Library::Vector<Library::String> Lines{};
        Library::String Raw{};

    public:
        [[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
        {
            return Code == 0;
        }

        [[nodiscard]] constexpr auto Informational() const -> Foundation::Bool
        {
            return Code >= 100 && Code < 200;
        }

        [[nodiscard]] constexpr auto Success() const -> Foundation::Bool
        {
            return Code >= 200 && Code < 300;
        }

        [[nodiscard]] constexpr auto Intermediate() const -> Foundation::Bool
        {
            return Code >= 300 && Code < 400;
        }

        [[nodiscard]] constexpr auto TransientError() const -> Foundation::Bool
        {
            return Code >= 400 && Code < 500;
        }

        [[nodiscard]] constexpr auto PermanentError() const -> Foundation::Bool
        {
            return Code >= 500 && Code < 600;
        }

        [[nodiscard]] constexpr auto Error() const -> Foundation::Bool
        {
            return Code >= 400;
        }

        [[nodiscard]] constexpr auto Positive() const -> Foundation::Bool
        {
            return Code >= 100 && Code < 400;
        }

        [[nodiscard]] constexpr auto NeedPassword() const -> Foundation::Bool
        {
            return Code == 331;
        }

        [[nodiscard]] constexpr auto LoggedIn() const -> Foundation::Bool
        {
            return Code == 230;
        }

        [[nodiscard]] constexpr auto FileActionOk() const -> Foundation::Bool
        {
            return Code == 150 || Code == 125 || Code == 226 || Code == 250;
        }

        [[nodiscard]] constexpr auto OpeningDataConnection() const -> Foundation::Bool
        {
            return Code == 150 || Code == 125;
        }

        [[nodiscard]] constexpr auto TransferComplete() const -> Foundation::Bool
        {
            return Code == 226 || Code == 250;
        }
    };
}