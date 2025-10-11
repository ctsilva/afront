GTB_BEGIN_NAMESPACE


inline unsigned factorial(unsigned x)
{
    unsigned result = 1;
    while (x)
    {
        result *= x;
        --x;
    }
    return result;
}


inline int rand32()
{
    return (rand() << 15) + rand();
}


GTB_END_NAMESPACE
