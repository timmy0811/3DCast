#include "3DCast/Core.h"
#include <iostream>
#include <string>

class User {
public:
    User(std::string name, int age) : m_Name(std::move(name)), m_Age(age) {}

    const std::string& GetName() const { return m_Name; }
    int GetAge() const { return m_Age; }

private:
    std::string m_Name;
    int m_Age;
};

void PrintUserInfo(const Cast::Optional<User>& userOpt) {
    // Check if the optional contains a value
    if (userOpt) {
        std::cout << "User: " << userOpt->GetName() << ", Age: " << userOpt->GetAge() << std::endl;
    } else {
        std::cout << "No user information available." << std::endl;
    }
}

int main() {
    // Creating an empty optional
    Cast::Optional<int> emptyOpt;

    // Check if empty
    std::cout << "emptyOpt has value: " << (emptyOpt ? "true" : "false") << std::endl;

    // Creating an optional with a value
    Cast::Optional<int> numberOpt = 42;

    // Alternative way using MakeOptional
    auto stringOpt = Cast::MakeOptional<std::string>("Hello, Cast::Optional!");

    // Accessing value with operator*
    std::cout << "numberOpt value: " << *numberOpt << std::endl;

    // Using value_or for safe access with default
    std::cout << "emptyOpt value or default: " << emptyOpt.value_or(0) << std::endl;
    std::cout << "numberOpt value or default: " << numberOpt.value_or(0) << std::endl;

    // Using a custom class with Optional
    Cast::Optional<User> userOpt = User("Alice", 30);
    PrintUserInfo(userOpt);

    // Using arrow operator for member access
    std::cout << "User name: " << userOpt->GetName() << std::endl;

    // Resetting an optional (making it empty)
    userOpt.reset();
    PrintUserInfo(userOpt);

    // Emplacing a new value
    userOpt.emplace("Bob", 25);
    PrintUserInfo(userOpt);

    // Assignment
    Cast::Optional<int> anotherOpt;
    anotherOpt = 100;
    std::cout << "Another optional: " << *anotherOpt << std::endl;

    // Access to native std::optional if needed
    std::optional<int>& nativeOpt = anotherOpt.native();
    nativeOpt = 200;
    std::cout << "Modified through native: " << *anotherOpt << std::endl;

    return 0;
}

