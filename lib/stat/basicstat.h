#ifndef __BASICSTAT_H
#define __BASICSTAT_H

/*
 * Functions to compute basic statistical properties
 *
 * mean, variance, standard deviation
 * median
 * ssq - sum of squares
 */
BEGIN_STAT_NAMESPACE

/*
 * f,l define the sequence
 * dof - degrees of freedom.
 */
template<class IT>
typename IT::value_type mean(IT f, IT l, int dof)
{
	typename IT::value_type m=0;
	typename IT::value_type n = (l-f);
	for(; f != l; ++f)
	{
		m += *f;
	}
	return m / (n-dof);
}

/*
 * Numerically stable variance
 * variance(X) = \SUM_i(x-\mu)^2 = \frac{n \sum x_i^2 - (sum x_i)^2}{n^2}
 * or divide by (n*(n-dof)) for unbiased formula
 * 
 */
template<class IT>
typename IT::value_type variance(IT f, IT l, int dof)
{
	typename IT::value_type sumx2 = 0; // Sum of x_i^2
	typename IT::value_type sumx = 0; // Sum of x_i
    typename IT::value_type n = (l-f);
	for(; f != l; ++f)
	{
        sumx += *f;
        sumx2 += (*f)*(*f);
	}
	return (n*sumx2 - sumx*sumx)/(n*(n-dof));
}

template<class T, class IT>
T variance(IT f, IT l, int dof)
{
	T sumx2 = 0; // Sum of x_i^2
	T sumx = 0; // Sum of x_i
    T n = (l-f);
	for(; f != l; ++f)
	{
        sumx += *f;
        sumx2 += (*f)*(*f);
	}
	return (n*sumx2 - sumx*sumx)/(n*(n-dof));
}

// compute the median of a range of data
// by sorting...
template <class IT, class T>
void median(IT first, IT last, T& med)
{
    unsigned N = last-first;
    std::sort(first, last);
    med = *(first+N/2);
}

//
// Compute the sum of squares of a sequence
//
#if 0
template <class IT>
typename IT::value_type ssq(IT first, IT last)
{
    IT::value_type result = 0;
    for (; f != l; ++f)
    {
        result += (*f)*(*f);
    }
    reutrn result
}
#endif

template <class IT, class T>
void ssq(IT first, IT last, T& result)
{
    result = 0;
    for (; first != last; ++first)
    {
        result += (*first)*(*first);
    }
}

END_STAT_NAMESPACE
#endif //  __BASICSTAT_H
