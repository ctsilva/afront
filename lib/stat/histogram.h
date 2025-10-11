#ifndef _STATLIB_HIST_H
#define _STATLIB_HIST_H

BEGIN_STAT_NAMESPACE

/*
 * A Q&D  histogram class
 * Assume operator < for type T
 * as well as + - * and /
 */
template<class T>
class Histogram
{
public:
    typedef T value_type;
    typedef unsigned count_type;

    //
    // [low, high] determin the range of values in the histogram
    // bins - number of bins in the histogram
    //
    Histogram(const value_type& low, const value_type& high, unsigned bins);
    ~Histogram();

    void insert_value(const value_type& value);
    count_type num_entries() const;
    unsigned num_bins() const;

    //
    // Compute the bin index for a value
    //
    unsigned bin_index(const value_type& value);

    //
    // return the bin index that has percent entries to its "left"
    //
    unsigned lookup_bin(double percent);

    //
    // Return the middle of the range a specific bin represents
    //
    value_type bin_value(unsigned bin);

    count_type* histogram;

protected:
    unsigned _bins;
    count_type _num_entries;
    value_type _low, _high; // range of legal values
    value_type _range;
};

END_STAT_NAMESPACE

#include "histogram.inl"

#endif // _STATLIB_HIST_H
