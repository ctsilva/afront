
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


BEGIN_STAT_NAMESPACE

template<class T>
Histogram<T>::Histogram(const value_type& low, const value_type& high, unsigned bins) :
    histogram(new count_type[bins]),
    _bins(bins),
    _num_entries(0),
    _low(low),
    _high(high),
    _range(_high - _low)
{
    std::fill(histogram, histogram+_bins, 0);
}


template<class T>
Histogram<T>::~Histogram()
{
    delete[] histogram;
}

template<class T>
void Histogram<T>::insert_value(const value_type& value)
{
    unsigned idx = bin_index(value);
    ++histogram[idx];
    ++_num_entries;
}

template<class T>
typename Histogram<T>::count_type Histogram<T>::num_entries() const
{
    return _num_entries;
}

//
// return the bin index that has percent entries to its "left"
//
template<class T>
unsigned Histogram<T>::lookup_bin(double percent)
{
    assert((percent >= 0.0) & (percent <= 1.0));
    count_type k = (count_type)(num_entries() * percent);
    count_type sum = 0;
    for (unsigned i = 0; i < _bins; ++i)
    {
        sum += histogram[i];
        if (sum >= k) return i;
    }
    return -1; // Should never reach this point..
}

//
// Return the middle of the range a specific bin represents
//
template<class T>
typename Histogram<T>::value_type Histogram<T>::bin_value(unsigned bin)
{
    assert(bin < _bins);
    return ((value_type)bin)/_bins * _range + _low + _range/(2*_bins);
}

//
// Compute the bin index for a value
//
template<class T>
unsigned Histogram<T>::bin_index(const value_type& value)
{
    assert((value >= _low) && (value <= _high));
    unsigned idx = (unsigned)(_bins*(value - _low)/_range);
    if (idx == _bins) --idx; // in case value == _high
    return idx;
}



END_STAT_NAMESPACE

