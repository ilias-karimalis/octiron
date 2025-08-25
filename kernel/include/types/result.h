#pragma once

#include <types/error.h>
template<typename T>
struct result
{
    /// Creates a Result<T, E>::Some(value) instance.
    result make_some(T value);

    /// Creates a Result<T, E>::Err(err) instance.
    result make_err(error err);

    /// Returns true if the Result<T, E> is Some(_).
    bool is_some();

    /// Returns true if the Result<T, E> is Err(_).
    bool is_err();

    /// Gets the value of a Result::Some(_) if it's Some(_), undefined behaviour if it is Err(_).
    T& some();

    /// Gets the value of a Result::Err(_) if it's Err(_), undefined behaviour if it is Some(_).
    error& err();

    bool m_some;
    union
    {
        T m_value;
        error m_err;
    } m_u;
};

template<typename T>
result<T>
result<T>::make_some(T value)
{
    return { .m_some = true, .m_u = { .m_value = value } };
}

template<typename T>
result<T>
result<T>::make_err(error err)
{
    return { .m_some = false, .m_u = { .m_err = err } };
}

template<typename T>
bool
result<T>::is_some()
{
    return m_some;
}

template<typename T>
bool
result<T>::is_err()
{
    return !m_some;
}

template<typename T>
T&
result<T>::some()
{
    return m_u.m_value;
}

template<typename T>
error&
result<T>::err()
{
    return m_u.m_err;
}