#pragma once

#include <hpx/hpx.hpp>

template <typename Iter>
struct line_iterator
  : hpx::util::iterator_facade<
        // Our type:
        line_iterator<Iter>,
        // Value type (When dereferencing the iterator)
        double,
        // Our iterator is random access.
        std::random_access_iterator_tag>
{
    using base_type = Iter;

    Iter up, middle, down;

    line_iterator(Iter up_, Iter middle_, Iter down_)
      : up(up_)
      , middle(middle_)
      , down(down_)
    {}

    void increment() {
        ++up;
        ++middle;
        ++down;
    }
    void decrement() {
        --up;
        --middle;
        --down;
    }
    void advance(std::ptrdiff_t n) {
        up += n;
        middle += n;
        down += n;
    }
    bool equal(line_iterator const& other) const {
        return middle == other.middle;
    }
    typename base_type::reference dereference() const {
        return *middle;
    }
    std::ptrdiff_t distance_to(line_iterator const& other) const {
        return other.middle - middle;
    }
};

template <typename Iter>
struct row_iterator
    // iterator_facade is a facade class that defines the boilerplate needed for
    // a proper standard C++ iterator.
  : hpx::util::iterator_facade<
        // Our type:
        row_iterator<Iter>,
        // Value type (When dereferencing the iterator)
        line_iterator<Iter>,
        // Our iterator is random access.
        std::random_access_iterator_tag,
        // Since dereferencing should return a new line_iterator, we need to
        // explicitly set the reference type.
        line_iterator<Iter>
    >
{
    using line_iterator_type = line_iterator<Iter>;

    row_iterator(std::size_t nx, Iter middle)
      : up_(middle + nx)
      , middle_(middle)
      , down_(middle - nx)
      , nx_(nx)
    {}

    line_iterator_type dereference() const
    {
        return {up_, middle_, down_};
    }

    void increment() {
        up_ += nx_;
        middle_ += nx_;
        down_ += nx_;
    }

    void decrement() {
        up_ -= nx_;
        middle_ -= nx_;
        down_ -= nx_;
    }

    void advance(std::ptrdiff_t n)
    {
        for(std::size_t i = 0; i<n; ++i)
        {
            increment();
        }
        // up_ += (n * nx_);
        // down_ += (n * nx_);
    }

    Iter up_, middle_, down_;
    std::size_t nx_;

    bool equal(row_iterator const& other) const {
        return middle_ == other.middle_;
    }

    std::ptrdiff_t distance_to(row_iterator const& other) const {
        return (other.middle_ - middle_)/nx_;
    }
};
