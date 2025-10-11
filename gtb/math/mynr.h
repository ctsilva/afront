#ifndef __MYNR_H
#define __MYNR_H

/*
 * Stuff from numerical recipes I use
 */

/*------------------ NR CODE ---------------*/
/*
 * Stuff from numerical recipes
 */
extern "C" float ran1f(long *idum);
extern "C" long ran1(long *idum);
extern "C" double gasdev(long* idum);



/*-------------- My extensions -------------*/
GTB_BEGIN_NAMESPACE

void nrseed(long v);
long nrran1();
float nrran1f();
double nrgasdev();

GTB_END_NAMESPACE

#endif // __MYNR_H
