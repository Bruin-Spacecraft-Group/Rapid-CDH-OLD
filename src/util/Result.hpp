#ifndef RAPID_CDH_RESULT_H
#define RAPID_CDH_RESULT_H

#include <variant>
#include <string>

/**
 * A class representing the result of an operation.
 * Holds either the expected value or a string containing the error message.
 */
template <typename T>
class Result {
public:
    /** Default constructor */
    Result() = default;

    /** Expected value constructor */
    explicit Result(const T& t) : m_variant{t} {}

    /** Error string constructor */
    explicit Result(const std::string& str) : m_variant{str} {}

    /** Returns true if the result holds the expected value */
    [[nodiscard]] bool valid() const {
        return std::holds_alternative<T>(m_variant);
    }

    /**
     * Returns the value of the result.
     * Should be used with valid().
     */
    T get() const {
        return std::get<T>(m_variant);
    }

    /**
     * Returns the value of the string representing the error.
     * Should be used with valid().
     */
    [[nodiscard]] const std::string& get_error() const {
        return std::get<std::string>(m_variant);
    }

private:
    // std:::monostate allows default construction, i.e. an instance with no data
    std::variant<std::monostate, T, std::string> m_variant;
};

/** Result type representing a void return type */
typedef Result<std::monostate> VoidResult;

#endif //RAPID_CDH_RESULT_H