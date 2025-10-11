#ifndef __STLTOOLS_H
#define __STLTOOLS_H

#include "statdef.h"

BEGIN_STAT_NAMESPACE

/*
 * Generic STL like tools
 */

 /*
  * Find the position of the minimax / maximal elements
  */
template <class IT>
unsigned min_element_pos(IT first, IT last)
{
    return std::min_element(first, last) - first;
}

template <class IT>
unsigned max_element_pos(IT first, IT last)
{
    return std::max_element(first, last) - first;
}

/*
 * Find the position of the give element in the sequence
 */
template <class IT, class T>
unsigned element_pos(IT first, IT last, const T& v)
{
    return std::find(first, last, v) - first;
}

/*
 * Find the value of the two maximal elements
 *
 * Need to write a min and K elements for small K
 *
 * Input [first,last) - elements sequences
 * Return: the two maximal values in mx1, mx2;
 */
template <class IT, class T>
void max2_elements(IT first, IT last, T& mx1, T& mx2)
{
    if (first == last) return;
    mx1 = *first;
    ++first;
    if (first == last) return;
    mx2 = *first;
    ++first;
    if (mx1 < mx2) std::swap(mx1, mx2);
    for (; first != last; ++first)
    {
        if (*first > mx1)
        {
            mx2 = mx1;
            mx1 = *first;
        }
        else if (*first > mx2)
        {
            mx2 = *first;
        }
    }
}

/*
 * Find the value of the two maximal elements
 *
 * Need to write a min and K elements for small K
 *
 * Input [first,last) - elements sequences
 * Return: pointers to the two maximal values in pmx1, pmx2;
 */
template <class IT>
void max2_elements(IT first, IT last, IT& pmx1, IT& pmx2)
{
    if (first == last) return;
    pmx1 = first;
    ++first;
    if (first == last) return;
    pmx2 = first;
    ++first;
    if (*pmx1 < *pmx2) std::swap(pmx1, pmx2);
    for (; first != last; ++first)
    {
        if (*first > *pmx1)
        {
            pmx2 = pmx1;
            pmx1 = first;
        }
        else if (*first > *pmx2)
        {
            pmx2 = first;
        }
    }
}

END_STAT_NAMESPACE

#endif // __STLTOOLS_H
