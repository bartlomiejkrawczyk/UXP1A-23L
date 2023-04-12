#pragma once

template <typename T>
class Example {
public:
    Example(T value) : value(value) {}

    T get() const;

    void set(T value);

private:
    T value;
};
