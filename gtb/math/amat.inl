template<class T>
inline AVec<T>::AVec(const AVec& rhs) : N(rhs.N)
{
    allocate();
    copy(rhs.v);
}

template<class T>
inline AVec<T>::~AVec()
{
    cleanup();
}

template<class T>
inline typename AVec<T>::reference AVec<T>::operator[](int n)
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::const_reference AVec<T>::operator[](int n) const
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::reference AVec<T>::operator()(int n)
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::const_reference AVec<T>::operator()(int n) const
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline void AVec<T>::cleanup()
{
    delete[] v;
}


/* ---------------------- amat ---------------------- */
template<class T>
inline typename AMat<T>::reference AMat<T>::operator() (int i, int j)
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i][j];
}

template<class T>
inline typename AMat<T>::const_reference AMat<T>::operator() (int i, int j) const
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i][j];
}

