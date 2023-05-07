#ifndef _ZIP_H_
#define _ZIP_H_

#include <tuple>
#include <iterator>

namespace detail
{
    template <typename... _Tp>
    bool variadic_or(_Tp &&...args)
    {
        return (... || args);
    }

    template <typename Tuple, std::size_t... I>
    bool any_equals(Tuple &&t1, Tuple &&t2, std::index_sequence<I...>)
    {
        return variadic_or(std::get<I>(std::forward<Tuple>(t1)) == std::get<I>(std::forward<Tuple>(t2))...);
    }
}

template <typename... T>
struct zip
{
    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = std::tuple<std::iter_value_t<std::ranges::iterator_t<T>>...>;
        using reference = std::tuple<std::iter_reference_t<std::ranges::iterator_t<T>>...>;
        using difference_type = std::tuple<std::iter_difference_t<std::ranges::iterator_t<T>>...>;
        using pointer = std::tuple<typename std::iterator_traits<std::ranges::iterator_t<T>>::pointer...>;

        reference operator*()
        {
            return std::apply(
                []<typename... _Tp>(_Tp &&...e)
                { return std::forward_as_tuple(*std::forward<_Tp>(e)...); },
                data_);
        }

        iterator &operator++()
        {
            std::apply(
                [this]<typename... _Tp>(_Tp &&...e)
                { data_ = make_tuple(++std::forward<_Tp>(e)...); },
                data_);
            return *this;
        }

        auto operator!=(const iterator &iter) const
        {
            return !detail::any_equals(data_, iter.data_, std::index_sequence_for<T...>{});
        }

        std::tuple<std::ranges::iterator_t<T>...> data_;
    };

    zip(T &...args) : data(std::forward_as_tuple(std::forward<T>(args)...)) {}

    auto begin()
    {
        return iterator{std::apply(
            []<typename... _Tp>(_Tp &&...e)
            { return std::make_tuple(std::begin(std::forward<_Tp>(e))...); },
            data)};
    }

    auto end()
    {
        return iterator{std::apply(
            []<typename... _Tp>(_Tp &&...e)
            { return std::make_tuple(std::end(std::forward<_Tp>(e))...); },
            data)};
    }

    std::tuple<T &...> data;
};
#endif // _ZIP_H_
