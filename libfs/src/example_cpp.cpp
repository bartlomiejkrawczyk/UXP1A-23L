#include "example_cpp.hpp"

template <typename T>
T Example<T>::get() const {
    return value;
}

template <typename T>
void Example<T>::set(T value) {
    this->value = value;
}
