#pragma once

#include <exception>
#include <string>

class NotImplementedException : public std::exception {
private:
    std::string m_message;

public:
    explicit NotImplementedException(const std::string& message) : m_message(message) {}

    const char* what() const noexcept override {
        return m_message.c_str();
    }
};


class InternalException : public std::exception {
private:
    std::string m_message;

public:
    explicit InternalException(const std::string& message) : m_message(message) {}

    const char* what() const noexcept override {
        return m_message.c_str();
    }
};
