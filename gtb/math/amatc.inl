
template<class T>
AMatc<T>::AMatc(int row, int col, const_pointer v, bool ftn) : M(row), N(col)
{
    allocate();
    if (v) assign(v, ftn);
}

template<class T>
AMatc<T>::AMatc(const AMatc<T>& rhs) : M(rhs.M), N(rhs.N)
{
    allocate();
    std::copy(rhs.m, rhs.m+M*N, m);
}

template<class T>
AMatc<T>::~AMatc()
{
    cleanup();
}


template<class T>
void AMatc<T>::set(const T& value)
{
    pointer p = m;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            *p = value;
            ++p;
        }
    }
}


template<class T>
inline typename AMatc<T>::reference AMatc<T>::operator() (int i, int j)
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i+j*M];
}

template<class T>
inline typename AMatc<T>::const_reference AMatc<T>::operator() (int i, int j) const
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i+j*M];
}

