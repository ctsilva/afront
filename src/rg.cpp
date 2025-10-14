
#include "rg.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define EXIT(retcode)

/* ------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------- */

#define gN(i)      ((!i)?(0):((this)->n.mem+(i)))
#define gA(i)      ((!i)?(0):((this)->a.mem+(i)))
#define gL(i)      ((!i)?(0):((this)->l.mem+(i)))
#define gB(i)      ((!i)?(0):((this)->b.mem+(i)))

#define NNext(N) (gN(N)->adown)
#define ANext(A) (gA(A)->l0   )   
#define LNext(L) (gL(L)->a    )
#define BNext(B) (gB(B)->Next )

#define NFree(N)  (gN(N)->aup  = ((uint)-2))
#define AFree(A)  (gA(A)->l1   = ((uint)-2))
#define LFree(L)  (gL(L)->Next = ((uint)-2))
#define BFree(B)  (gB(B)->Prev = ((uint)-2))

#define bNFree(N) (gN(N)->aup  ==((uint)-2))
#define bAFree(A) (gA(A)->l1   ==((uint)-2))
#define bLFree(L) (gL(L)->Next ==((uint)-2))
#define bBFree(B) (gB(B)->Prev ==((uint)-2))

#define NewN(N)  {N=this->n.ff;this->n.ff=gN(N)->adown;++(this->n.num);memset(gN(N),0,sizeof(Tn));}
#define NewA(A)  {A=this->a.ff;this->a.ff=gA(A)->l0   ;++(this->a.num);memset(gA(A),0,sizeof(Ta));}
#define NewL(L)  {L=this->l.ff;this->l.ff=gL(L)->a    ;++(this->l.num);memset(gL(L),0,sizeof(Tl));}
#define NewB(B)  {B=this->b.ff;this->b.ff=gB(B)->Next ;++(this->b.num);memset(gB(B),0,sizeof(Tb));}

#define DelN(N)  {CHECKPOINT(!bNFree(N));NFree(N);NNext(N) = this->n.ff;this->n.ff=(N);--(this->n.num);}
#define DelA(A)  {CHECKPOINT(!bAFree(A));AFree(A);ANext(A) = this->a.ff;this->a.ff=(A);--(this->a.num);}
#define DelL(L)  {CHECKPOINT(!bLFree(L));LFree(L);LNext(L) = this->l.ff;this->l.ff=(L);--(this->l.num);}
#define DelB(B)  {CHECKPOINT(!bBFree(B));BFree(B);BNext(B) = this->b.ff;this->b.ff=(B);--(this->b.num);}

#define Vless(n0,n1) (n0->value<n1->value || (n0->value==n1->value && n0->id<n1->id))

#define isRegular(n) ((n)->adown && !gA((n)->adown)->a1n && (n)->aup && !gA((n)->aup)->a0n)

#define bIndex(N) ((int)((bucket_samples-1)*((gN(N)->value-minf)/(maxf-minf))))


#define bMerge2(Bdst,Bsrc) \
{\
	Tb* _bdst=gB(Bdst);\
	Tb* _bsrc=gB(Bsrc);\
	uint  Num=_bdst->Num+_bsrc->Num;\
	if (Num)\
	{\
		float alpha=_bdst->Num/((float)Num);\
		float beta =_bsrc->Num/((float)Num);\
		_bdst->X=alpha*_bdst->X+beta*_bsrc->X;\
		_bdst->Y=alpha*_bdst->Y+beta*_bsrc->Y;\
		_bdst->Z=alpha*_bdst->Z+beta*_bsrc->Z;\
		_bdst->Num=Num;\
	}\
}


#define bMerge3(Bdst,Bsrc,N) \
{\
	Tb* _bdst=gB(Bdst);\
	Tb* _bsrc=gB(Bsrc);\
	uint  Num=_bdst->Num+_bsrc->Num+1;\
	float alpha=_bdst->Num/((float)Num);\
	float beta =_bsrc->Num/((float)Num);\
	float gamma=         1/((float)Num);\
	_bdst->X=alpha*_bdst->X+beta*_bsrc->X+gamma*gN(N)->X;\
	_bdst->Y=alpha*_bdst->Y+beta*_bsrc->Y+gamma*gN(N)->Y;\
	_bdst->Z=alpha*_bdst->Z+beta*_bsrc->Z+gamma*gN(N)->Z;\
	_bdst->Num=Num;\
}


#define addUA(N,A) \
{\
	Tn* n=gN(N);Ta* a=gA(A);\
	CHECKPOINT(a->n0==(N));\
	a->a0p=0;a->a0n=n->aup;\
	if (n->aup) gA(n->aup)->a0p=(A);\
	n->aup=(A);\
}

#define addDA(N,A) \
{ \
	Tn* n=gN(N);Ta* a=gA(A);\
	CHECKPOINT(a->n1==(N));\
	a->a1p=0;\
	a->a1n=n->adown;\
	if (n->adown) gA(n->adown)->a1p=(A);\
	n->adown=(A);\
}

#define remUA(N,A) \
{\
	Tn* n=gN(N);Ta* a=gA(A);\
	CHECKPOINT(a->n0==(N));\
	if (a->a0p) gA(a->a0p)->a0n=a->a0n; else n->aup=a->a0n;\
	if (a->a0n) gA(a->a0n)->a0p=a->a0p;\
}


#define remDA(N,A) \
{\
	Tn* n=gN(N);Ta* a=gA(A);\
	CHECKPOINT(a->n1==(N));\
	if (a->a1p) gA(a->a1p)->a1n=a->a1n; else n->adown=a->a1n;\
	if (a->a1n) gA(a->a1n)->a1p=a->a1p;\
}


/* ------------------------------------------------------------- */
/* allocate memory
/* ------------------------------------------------------------- */
void ReebGraph::memory_n(uint n)
{
	uint oldsize,i;

	if ((this->n.max-this->n.num)<n)
	{
		oldsize=this->n.max;
		if (!this->n.max) this->n.max=n;
		while ((this->n.max-this->n.num)<n) 
			this->n.max<<=1;
#ifdef _DEBUG
		printf("Reallocating nodes to a size of %dkb\n",(int)(sizeof(Tn)*this->n.max/1024.0f));
#endif
		this->n.mem=(Tn*)realloc(this->n.mem,sizeof(Tn)*this->n.max);
		if (!this->n.mem)
		{
			fprintf(stderr,"error in realloc FILE=%s LINE=%d\n",__FILE__,(int)__LINE__);
			fflush(stdout);
			CHECKPOINT(this->n.mem);EXIT(-1);
		}
		for (i=oldsize;i<this->n.max-1;i++)
			{NNext(i)=i+1;NFree(i);}
		NNext(i)=this->n.ff;
		NFree(i);
		this->n.ff=oldsize;
	}
}


void ReebGraph::memory_a(uint n)
{	
	uint oldsize,i;
	if ((this->a.max-this->a.num)<n)
	{
		oldsize=this->a.max;
		if (!this->a.max) this->a.max=n;
		while ((this->a.max-this->a.num)<n) 
			this->a.max<<=1;
#ifdef _DEBUG
		printf("Reallocating arches to a size of %dkb\n",(int)(sizeof(Ta)*this->a.max/1024.0f));
#endif
		this->a.mem=(Ta*)realloc(this->a.mem,sizeof(Ta)*this->a.max);
		if (!this->a.mem)
		{
			fprintf(stderr,"error in realloc FILE=%s LINE=%d\n",__FILE__,(int)__LINE__);
			fflush(stdout);
			CHECKPOINT(this->a.mem);EXIT(-1);
		}
		for (i=oldsize;i<this->a.max-1;i++)
			{ANext(i)=i+1;AFree(i);}
		ANext(i)=this->a.ff;AFree(i);
		this->a.ff=oldsize;
	}
}
	


void ReebGraph::memory_l(uint n)
{
	uint oldsize,i;
	if ((this->l.max-this->l.num)<n)
	{
		oldsize=this->l.max;
		if (!this->l.max) this->l.max=n;
		while ((this->l.max-this->l.num)<n) 
			this->l.max<<=1;
#ifdef _DEBUG
		printf("Reallocating labels to a size of %dkb\n",(int)(sizeof(Tl)*this->l.max/1024.0f));
#endif
		this->l.mem=(Tl*)realloc(this->l.mem,sizeof(Tl)*this->l.max);
		if (!this->l.mem)
		{
			fprintf(stderr,"error in realloc FILE=%s LINE=%d\n",__FILE__,(int)__LINE__);
			fflush(stdout);
			CHECKPOINT(this->l.mem);EXIT(-1);
		}
		for (i=oldsize;i<this->l.max-1;i++) 
			{LNext(i)=i+1;LFree(i);}
		LNext(i)=this->l.ff;LFree(i);
		this->l.ff=oldsize;
	}
}


void ReebGraph::memory_b(uint n)
{
	uint oldsize,i;
	if ((this->b.max-this->b.num)<n)
	{
		oldsize=this->b.max;
		if (!this->b.max) this->b.max=n;
		while ((this->b.max-this->b.num)<n) 
			this->b.max<<=1;
#ifdef _DEBUG
		printf("Reallocating buckets to a size of %dkb\n",(int)(sizeof(Tb)*this->b.max/1024.0f));
#endif
		this->b.mem=(Tb*)realloc(this->b.mem,sizeof(Tb)*this->b.max);
		if (!this->b.mem)
		{
			fprintf(stderr,"error in realloc FILE=%s LINE=%d\n",__FILE__,(int)__LINE__);
			fflush(stdout);
			CHECKPOINT(this->b.mem);EXIT(-1);
		}
		for (i=oldsize;i<this->b.max-1;i++) 
			{BNext(i)=i+1;BFree(i);}
		BNext(i)=this->b.ff;BFree(i);
		this->b.ff=oldsize;
	}
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
ReebGraph::ReebGraph()
{
	init();
	ncollapse=nvertices=0;
	minf=maxf=0;
	bucket_samples=0;
	nendvertex=0;
	getclock(&t1);
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
ReebGraph::~ReebGraph()
{
	dispose();
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::setBuckets(float minf,float maxf,int Num)
{
	this->bucket_samples=Num;
	this->minf=minf-(maxf-minf)/10000;
	this->maxf=maxf+(maxf-minf)/10000;
	printf("Reeb graph uses %d buckets on the range %f %f\n",(int)Num,(float)minf,(float)maxf);
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::init()
{
	this->n.mem=(Tn*)malloc(sizeof(Tn)*2);
	this->a.mem=(Ta*)malloc(sizeof(Ta)*2);
	this->l.mem=(Tl*)malloc(sizeof(Tl)*2);
	this->b.mem=(Tb*)malloc(sizeof(Tb)*2);

	this->n.max=2;this->n.num=1; //the item "0" is blocked
	this->a.max=2;this->a.num=1; //the item "0" is blocked
	this->l.max=2;this->l.num=1; //the item "0" is blocked
	this->b.max=2;this->b.num=1;

	this->n.ff=1;NFree(1);NNext(1)=0;
	this->a.ff=1;AFree(1);ANext(1)=0;
	this->l.ff=1;LFree(1);LNext(1)=0;
	this->b.ff=1;BFree(1);BNext(1)=0;
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::dispose()
{
	free(this->n.mem);this->n.mem=0;
	free(this->a.mem);this->a.mem=0;
	free(this->l.mem);this->l.mem=0;
	free(this->b.mem);this->b.mem=0;

	this->n.max=this->n.num=0;
	this->a.max=this->a.num=0;
	this->l.max=this->l.num=0;
	this->b.max=this->b.num=0;
	
	this->n.ff=0;
	this->a.ff=0;
	this->l.ff=0;
	this->b.ff=0;
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::terminate()
{
	uint A;

	getclock(&t2);
	float diff=(float)getdiffclock(&t2,&t1);

	if (nendvertex!=this->nvertices)
	{
		printf("*** warning: you didn't call endVertex for all vertices !!! ***\n");
	}

	//a simplification of vertices
	int nmyend=0;
	for (uint N=1;N<this->n.max;N++)
	{
		if (bNFree(N))
			continue;

		Tn* n=gN(N);

		if (isRegular(n))
		{
			nmyend++;

			if (n->finilized)
				endVertex(N);
		}
	}

	printf("reeb graph calculated in %.2f seconds \n",diff);

#ifdef _DEBUG
	printf("Checking reeb graph(DEBUG mode)...");fflush(stdout);
	this->check();
	printf("ok\n");
#endif

	//simplification of buckets
	for ( A=1;A<this->a.max;A++)
	{
		if (bAFree(A)) continue;

		Ta* a=gA(A);

		if (a->b0!=a->b1 && gB(a->b0)->Next!=a->b1)
		{
			uint BPrev=a->b0;
			uint B=gB(BPrev)->Next;

			for (;B;B=gB(B)->Next)
			{
				Tb* b=gB(B);
				if (b->Num || B==a->b1) 
				{
					gB(BPrev)->Next=B;
					b->Prev=BPrev;
					BPrev=B;
					continue;
				}
			}
		}
	}

	//smoothing process

	int nsmooth=2;

	Tb* mem=(Tb*)malloc(sizeof(Tb)*this->b.max);

	for (int s=0;s<nsmooth;s++)
	{
		memcpy(mem,this->b.mem,sizeof(Tb)*this->b.max);

		//simplification of buckets
		for ( A=1;A<this->a.max;A++)
		{
			if (bAFree(A)) continue;

			Ta* a=gA(A);

			//smoothing processs

			if (a->b0!=a->b1 && gB(a->b0)->Next!=a->b1)
			{
				float Xp=gN(a->n0)->X;
				float Yp=gN(a->n0)->Y;
				float Zp=gN(a->n0)->Z;

				uint BNext;
				for (uint B=gB(a->b0)->Next;B!=a->b1;B=BNext)
				{
					Tb* b=gB(B);
					BNext=b->Next;

					float X=gB(B)->X;
					float Y=gB(B)->Y;
					float Z=gB(B)->Z;

					float Xn=(BNext==a->b1)?gN(a->n1)->X:gB(BNext)->X;
					float Yn=(BNext==a->b1)?gN(a->n1)->Y:gB(BNext)->Y;
					float Zn=(BNext==a->b1)?gN(a->n1)->Z:gB(BNext)->Z;

					mem[B].X=(1/3.0f)*(Xp+X+Xn);
					mem[B].Y=(1/3.0f)*(Yp+Y+Yn);
					mem[B].Z=(1/3.0f)*(Zp+Z+Zn);

					Xp=X;Yp=Y;Zp=Z;
				}
			}
		}

		SWAP(Tb*,mem,this->b.mem);
	}

	free(mem);

	printf("reeb graph info:\n");

	printf("   total collapse %d (%dk/sec; %dk triangle per sec) \n",
		(int)this->ncollapse,(int)(this->ncollapse/(1000*diff)),(int)(0.5*this->ncollapse/(1000*diff)));

	printf("   total verticess %d (%dk/sec) nendv=%d\n",
		(int)this->nvertices ,(int)(this->nvertices/(1000*diff)),
		(int)nmyend);

	printf("   Nodes num=%d (%d kb) max=%d (%d kb) %.2f\n",
		(int)this->n.num,(int)(sizeof(Tn)*this->n.num/1024.0f),
		(int)this->n.max,(int)(sizeof(Tn)*this->n.max/1024.0f),
		(float)(100.0*this->n.num/(float)this->n.max));

	printf("   Arches num=%d (%d kb) max=%d (%d kb) %.2f\n",
		(int)this->a.num,(int)(sizeof(Ta)*this->a.num/1024.0f),
		(int)this->a.max,(int)(sizeof(Ta)*this->a.max/1024.0f),
		(float)(100.0*this->a.num/(float)this->a.max));

	printf("   Labels num=%d (%d kb) max=%d (%d kb) %.2f \n",
		(int)this->l.num,(int)(sizeof(Tl)*this->l.num/1024.0f),
		(int)this->l.max,(int)(sizeof(Tl)*this->l.max/1024.0f),
		(float)(100.0*this->l.num/(float)this->l.max));

	printf("   Buckets num=%d (%d kb) max=%d (%d kb) %.2f \n",
		(int)this->b.num,(int)(sizeof(Tb)*this->b.num/1024.0f),
		(int)this->b.max,(int)(sizeof(Tb)*this->b.max/1024.0f),
		(float)(100.0*this->b.num/(float)this->b.max));
	
	//clean up labels, dealloc labels and restore the labels to a minimun size
	for (A=1;A<this->a.max;A++)
		if (!bAFree(A))
			gA(A)->l0=gA(A)->l1=0;

	if (this->l.mem) 
		free(this->l.mem);
	
	this->l.mem=(Tl*)malloc(sizeof(Tl)*2);
	this->l.max=2;this->l.num=1;
	this->l.ff=1;LFree(1);LNext(1)=0;
}



/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::print(FILE* file) const
{
	uint A;
	fprintf(file,"nodes\n");
	for (uint N=1;N<this->n.max;N++)
	{
		if (bNFree(N))
			continue;
		
		Tn* n=gN(N);
		fprintf(file,"\t[id=%u rg_id=%u] %f ",(uint)n->id,N,(float)n->value);
		fprintf(file, " dw(");
		for (A=n->adown;A;A=gA(A)->a1n)
			fprintf(file,"%u%s",gA(A)->n0,(gA(A)->a1n)?" ":"");
		fprintf(file,") up(");
		for (A=n->aup;A;A=gA(A)->a0n)
			fprintf(file,"%u%s",gA(A)->n1,(gA(A)->a0n)?" ":"");
		fprintf(file, ")\n");
	}
	
	fprintf(file,"arches\n");
	for (A=1;A<this->a.max;A++)
	{
		Ta* a=gA(A);
		if (bAFree(A))
			continue;
		fprintf(file,"\t[%u] (%u %u) Labels(",A,a->n0,a->n1);
		for (uint L=a->l0;L;L=gL(L)->Next)
		{
			Tl* l=gL(L);
			fprintf(file,"([%u p=%u n=%u] (%u %u)",L,l->lp,l->ln,
				(uint)(l->label>>32),(uint)(l->label & ((((uint64)1)<<32)-1)));
		}
		fprintf(file,")\n");
	}
	fprintf(file,"\n");
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */

void ReebGraph::glDisplay() const
{		
	glColor3f(0,0,0);
	glDisable(GL_LIGHTING);
	

	if (!bucket_samples)
	{
		glBegin(GL_LINES);

		for (uint A=1;A<this->a.max;A++)
		{
			if (!bAFree(A)) 
			{
				Tn *n0=gN(gA(A)->n0),*n1=gN(gA(A)->n1);
				glVertex3f(n0->X,n0->Y,n0->Z);
				glVertex3f(n1->X,n1->Y,n1->Z);
			}
		}
		glEnd();
	}
	else
	{
		for (uint A=1;A<this->a.max;A++)
		{
			if (bAFree(A)) continue;

			glBegin(GL_LINE_STRIP);

			Ta* a=gA(A);
			Tn *n0=gN(gA(A)->n0),*n1=gN(gA(A)->n1);

			glVertex3f(n0->X,n0->Y,n0->Z);

			if (a->b0!=a->b1 && gB(a->b0)->Next!=a->b1)
			{
				for (uint B=gB(a->b0)->Next;B!=a->b1;B=gB(B)->Next)
				{
					Tb* b=gB(B);
					CHECKPOINT(b->Num);
					glVertex3f(b->X,b->Y,b->Z);
				}
			}
			glVertex3f(n1->X,n1->Y,n1->Z);
			glEnd();
		}
	}
	
	glEnable(GL_LIGHTING);
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::check() const
{
	uint N,A,L,B;
	uint Nn=1,An=1,Ln=1,Bn=1,Nf=0,Af=0,Lf=0,Bf=0;

#define EnforceCheck(cond) \
	if (!(cond)) {\
		printf("check failed file %s line %d\n",__FILE__,(int)__LINE__);\
		assert(0);\
		EXIT(-1);\
	}

	for (N=1;N<this->n.max;N++)
	{
		if (bNFree(N))
		{
			Nf++;
			continue;
		}

		Nn++;
		Tn* n=gN(N);

		//not regular point
		CHECKPOINT(!n->adown || !n->aup || (gA(n->adown)->a1n || gA(n->aup)->a0n));

		for (A=n->adown;A;A=gA(A)->a1n)
		{
			EnforceCheck(gA(A)->n1==N);
			uint M=gA(A)->n0;

			uint cur;
			for (cur=gN(M)->aup;cur;cur=gA(cur)->a0n)
				if (gA(cur)->n1==N) break;

			EnforceCheck(cur);
		}
	
		for (A=n->aup  ;A;A=gA(A)->a0n)
		{
			EnforceCheck(gA(A)->n0==N);

			uint M=gA(A)->n1;

			uint cur;
			for (cur=gN(M)->adown;cur;cur=gA(cur)->a1n)
				if (gA(cur)->n0==N) break;
			EnforceCheck(cur);
		}
	}
	EnforceCheck(Nn==this->n.num && ((this->n.max-this->n.num)==Nf));

	for (A=1;A<this->a.max;A++)
	{
		if (bAFree(A))
		{
			Af++;
			continue;
		}

		An++;
		Ta* a=gA(A);

		uint Acur,Aprev;

		for (Acur=gN(a->n0)->aup;Acur;Acur=gA(Acur)->a0n)
			if (Acur==A) 
				break;
		EnforceCheck(Acur);

		for (Acur=gN(a->n1)->adown;Acur;Acur=gA(Acur)->a1n)
			if (Acur==A) 
				break;
		EnforceCheck(Acur);

		for (Aprev=0,Acur=gN(a->n0)->aup;Acur;Acur=gA(Acur)->a0n)
		{
			EnforceCheck(gA(Acur)->a0p==Aprev);
			Aprev=Acur;
		}

		for (Aprev=0,Acur=gN(a->n1)->adown;Acur;Acur=gA(Acur)->a1n)
		{
			EnforceCheck(gA(Acur)->a1p==Aprev);
			Aprev=Acur;
		}

		uint Prev=0;
		for (L=a->l0;L;L=gL(L)->Next)
		{
			EnforceCheck(gL(L)->a==A);
			EnforceCheck(gL(L)->Prev==Prev);

			if (!gL(L)->Next)
				EnforceCheck(L==a->l1);

			Prev=L;
		}

		//buckets

		CHECKPOINT(a->b0 && a->b1);
		uint BPrev=0;
		int Cont=0;
		for (uint B=a->b0;B;B=gB(B)->Next)
		{
			EnforceCheck(gB(B)->Prev==BPrev);
			Cont++;
			BPrev=B;
		}
		uint i0=bIndex(a->n0);
		uint i1=bIndex(a->n1);
		EnforceCheck(Cont==(1+i1-i0));
	}
	EnforceCheck(An==this->a.num && ((this->a.max-this->a.num)==Af));

	for (L=1;L<this->l.max;L++)
	{
		if (bLFree(L))
		{
			Lf++;
			continue;
		}

		Ln++;

		Tl* l=gL(L);
		uint Lcur;

		for (Lcur=gA(l->a)->l0;Lcur;Lcur=gL(Lcur)->Next)
			if (Lcur==L) break;
		EnforceCheck(Lcur);
	
		for (Lcur=gA(l->a)->l0;Lcur;Lcur=gL(Lcur)->Next)
			EnforceCheck(Lcur==L || !(gL(Lcur)->label==l->label));

		uint Lp=0;
		uint N0=gA(l->a)->n0;
		uint N1=gA(l->a)->n1;
		uint cont;

		for (cont=0,A=gN(N0)->adown;A;A=gA(A)->a1n)
		{
			for (Lcur=gA(A)->l0;Lcur;Lcur=gL(Lcur)->Next)
			{
				if (gL(Lcur)->label==l->label)
				{
					EnforceCheck(l->lp==Lcur && gL(Lcur)->ln==L);
					cont++;
				}
			}
		}
		EnforceCheck((!l->lp && !cont) || (l->lp && cont==1));
			
		for (cont=0,A=gN(N1)->aup;A;A=gA(A)->a0n)
		{
			for (Lcur=gA(A)->l0;Lcur;Lcur=gL(Lcur)->Next)
			{
				if (gL(Lcur)->label==l->label)
				{
					EnforceCheck(l->ln==Lcur && gL(Lcur)->lp==L);
					cont++;
				}
			}
		}
		EnforceCheck((!l->ln && !cont) || (l->ln && cont==1));
	}
	EnforceCheck(Ln==this->l.num && ((this->l.max-this->l.num)==Lf));

	for (B=1;B<this->b.max;B++)
	{
		if (bBFree(B))
		{
			Bf++;
			continue;
		}
		Bn++;
	}
	EnforceCheck(Bn==this->b.num && ((this->b.max-this->b.num)==Bf));


#undef EnforceCheck

}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
uint ReebGraph::addVertex(uint id,float X,float Y,float Z,Tvalue value)
{
	++nvertices;
	uint N0;
	memory_n(1);NewN(N0);Tn* node=gN(N0);
	node->id=id;
	node->value=value;
	node->X=X;
	node->Y=Y;
	node->Z=Z;
	node->adown=0;
	node->aup=0;
	return N0;
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
#define vCollapse(N,n) \
{\
	uint Lb,Lnext,La;\
	Tl* lb;\
	CHECKPOINT(n->finilized);\
	uint A0=n->adown;\
	uint A1=n->aup  ;\
	Ta *a0=gA(A0); \
	Ta *a1=gA(A1);\
	CHECKPOINT(!a0->a1n && !a1->a0n);\
	a0->n1  =a1->n1;\
	a0->a1p =a1->a1p;if (a1->a1p) gA(a1->a1p)->a1n=A0;\
	a0->a1n =a1->a1n;if (a1->a1n) gA(a1->a1n)->a1p=A0;\
	if (gN(a1->n1)->adown==A1) gN(a1->n1)->adown=A0;\
\
	if (bucket_samples)\
	{\
		bMerge3(a0->b1,a1->b0,N);\
		uint B1Next=gB(a1->b0)->Next;\
		if (B1Next) {gB(a0->b1)->Next=B1Next;gB(B1Next)->Prev=a0->b1;a0->b1=a1->b1;} \
		DelB(a1->b0);\
		a1->b0=a1->b1=0;\
	}\
\
	for (Lb=a1->l0;Lb;Lb=Lnext) \
	{\
		lb=gL(Lb);\
		Lnext=lb->Next;\
\
		if (lb->lp)\
		{\
			La=lb->lp;\
			CHECKPOINT(gL(La)->a==A0 && gL(La)->ln==Lb);\
			gL(La)->ln=lb->ln;\
		}\
\
		if (lb->ln)\
			gL(lb->ln)->lp=lb->lp;\
\
		DelL(Lb);\
	}\
\
	DelA(A1);\
	DelN(N);\
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::endVertex(const int N)
{
	uint A,L,Lnext;
	Tl  *l;
	Tn  *n=gN(N);

	CHECKPOINT(!bNFree(N));

	n->finilized=1;
	++nendvertex;

	for (A=n->aup;A;A=gA(A)->a0n)
	{
		for (L=gA(A)->l0;L;L=Lnext) 
		{
			Lnext=gL(L)->Next;

			if (!(l=gL(L))->lp) 
			{
				for (uint Lcur=L;Lcur;Lcur=gL(Lcur)->ln)
				{
					Tl* lcur=gL(Lcur);
					if (lcur->Prev) gL(lcur->Prev)->Next=lcur->Next;else gA(lcur->a)->l0=lcur->Next;
					if (lcur->Next) gL(lcur->Next)->Prev=lcur->Prev;else gA(lcur->a)->l1=lcur->Prev;
					DelL(Lcur);
				}
			}
		}
	}

	for (A=n->adown;A;A=gA(A)->a1n)
	{
		for (L=gA(A)->l0;L;L=Lnext) 
		{
			Lnext=gL(L)->Next;

			if (!(l=gL(L))->ln) 
			{
				for (uint Lcur=L;Lcur;Lcur=gL(Lcur)->lp)
				{
					Tl* lcur=gL(Lcur);
					if (lcur->Prev) gL(lcur->Prev)->Next=lcur->Next;else gA(lcur->a)->l0=lcur->Next;
					if (lcur->Next) gL(lcur->Next)->Prev=lcur->Prev;else gA(lcur->a)->l1=lcur->Prev;
					DelL(Lcur);
				}
			}
		}
	}

	//special case for regular point. A node is regular if it has one
	//arch down and one arch up. In this case it can disappear
	if (isRegular(n))
	{
		vCollapse(N,n);
	}
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
uint ReebGraph::findUpLabel(uint N,uint64 Label) const
{
	for (uint A=gN(N)->aup;A;A=gA(A)->a0n)
	for (uint L=gA(A)->l0;L;L=gL(L)->Next)
	{
		if (gL(L)->label==Label)
			return L; 
	}
	return 0;
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
uint ReebGraph::findDwLabel(uint N,uint64 Label) const
{
	for (uint A=gN(N)->adown;A;A=gA(A)->a1n)
	for (uint L=gA(A)->l0;L;L=gL(L)->Next)
	{
		if (gL(L)->label==Label)
			return L; 
	}
	return 0;
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */
void ReebGraph::addPath(uint n,uint* Nodes,uint64 Label)
{
	uint i,Lprev;
	memory_l(n-1);
	memory_a(n-1);

	for (Lprev=0,i=0;i<(n-1);i++)
	{
		uint N0=Nodes[i  ];Tn* n0=gN(N0);
		uint N1=Nodes[i+1];Tn* n1=gN(N1);

		CHECKPOINT(Vless(n0,n1));

		CHECKPOINT(!findUpLabel(N0,Label) && !findDwLabel(N1,Label));

		uint A;NewA(A);Ta* arch =gA(A);
		uint L;NewL(L);Tl* label=gL(L);

		arch->n0=N0;
		arch->n1=N1;
		arch->l0=arch->l1=L;
		arch->a0p=arch->a0n=arch->a1p=arch->a1n=0;

		label->a=A;
		label->label=Label;
		label->lp=Lprev;
		label->ln=0;
		label->Next=0;
		label->Prev=0;

		addUA(N0,A);
		addDA(N1,A);

		//buckets
		if (bucket_samples)
		{
			uint i0=bIndex(N0);
			uint i1=bIndex(N1);

			memory_b(1+i1-i0);

			uint BPrev=0;

			uint B;
			for (uint i=i0;i<=i1;i++)
			{
				NewB(B);Tb* b=gB(B);
				b->Prev=BPrev;
				//b->Next=0;
				//b->X=0;
				//b->Y=0;
				//b->Z=0;
				//b->Num=0;
				if (!arch->b0) arch->b0=B;
				if (BPrev) gB(BPrev)->Next=B;
				BPrev=B;
			}
			arch->b1=B;
		}

		if (Lprev) gL(Lprev)->ln=L;
		Lprev=L;
	}
}

/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */

void ReebGraph::collapse(uint Nstart,uint Nend,uint64 Labela,uint64 Labelb)
{
	uint L0,L0n,L1,L1n;
	int cont[3]={0,0,0},Case;

	//static float times[4]={0,0,0};
	//TTime t1,t2,t3,t4,t5,t6;
	

	CHECKPOINT(!bNFree(Nstart) && !bNFree(Nend));

	Tn* nstart=gN(Nstart);
	Tn* nend  =gN(Nend  );

	CHECKPOINT(Vless(nstart,nend));

	L0=findUpLabel(Nstart,Labela);
	L1=findUpLabel(Nstart,Labelb);

	CHECKPOINT(L0 && L1 && gA(gL(L0)->a)->n0==Nstart && gA(gL(L1)->a)->n0==Nstart);
	
	ncollapse++;

	//getclock(&t1);

	while (1)
	{
		uint A0=gL(L0)->a;Ta* a0=gA(A0);
		uint A1=gL(L1)->a;Ta* a1=gA(A1);

		CHECKPOINT(a0->n0==a1->n0 && A0 && A1);

		/* it is the same arch, no semplification is done */
		if (A0==A1) 
		{
			Case=0;

			L0n=gL(L0)->ln;
			L1n=gL(L1)->ln;
			
		}
		/* there are two arch connecting the same start-end node */
		else if (A0!=A1 && a0->n1==a1->n1)
		{
			Case=1;

			if (bucket_samples)
			{
				uint B0=gA(A0)->b0,B0Next;
				uint B1=gA(A1)->b0,B1Next;

				for (;B0 && B1;)
				{	
					B0Next=gB(B0)->Next;
					B1Next=gB(B1)->Next;
					bMerge2(B0,B1);
					DelB(B1);
					B0=B0Next;
					B1=B1Next;
				}
				CHECKPOINT(!B0 && !B1);
				gA(A1)->b0=gA(A1)->b1=0;
			}

			remUA(a0->n0,A1);
			remDA(a0->n1,A1);

			//move labels from A1 to A0
			CHECKPOINT(gA(A1)->l0 && gA(A0)->l0);

			for (uint Lcur=gA(A1)->l0;Lcur;Lcur=gL(Lcur)->Next)
				gL(Lcur)->a=A0;

			gL(gA(A1)->l0)->Prev=gA(A0)->l1;
			gL(gA(A0)->l1)->Next=gA(A1)->l0;
			gA(A0)->l1=gA(A1)->l1;
			
			gA(A1)->l0=0;
			gA(A1)->l1=0;
			DelA(A1);
			
			L0n=gL(L0)->ln;
			L1n=gL(L1)->ln;
		}
		/* a more complicate situation, collapse reaching the less ending point of the archs */
		else 
		{
			Case=2;
			{
				Tn* a0n1=gN(a0->n1);
				Tn* a1n1=gN(a1->n1);
				if (!Vless(a0n1,a1n1))
				{
					SWAP(uint,A0,A1);
					SWAP(uint,L0,L1);
					SWAP(Ta* ,a0,a1);
				}
			}

			if (bucket_samples)
			{
				uint B0=a0->b0,B0Next;
				uint B1=a1->b0,B1Next;
				for (;B0!=a0->b1;B0=B0Next,B1=B1Next)
				{
					B0Next=gB(B0)->Next;
					B1Next=gB(B1)->Next;

					bMerge2(B0,B1);

					a1->b0=B1Next;
					gB(a1->b0)->Prev=0;
					
					DelB(B1);
				}
			}


			remUA(a0->n0,A1);
			a1->n0=a0->n1;
			addUA(a0->n1,A1);

			//"replicate" labels from A1 to A0 
			for (uint Lcur=gA(A1)->l0;Lcur;Lcur=gL(Lcur)->Next)
			{	
				uint Lnew;memory_l(1);NewL(Lnew);Tl* lnew=gL(Lnew);
				Tl* lcur=gL(Lcur);
				lnew->a     = A0;
				lnew->lp    = lcur->lp;
				if (lcur->lp) gL(lcur->lp)->ln=Lnew;
				lcur->lp    = Lnew;
				lnew->ln    = Lcur;
				lnew->label = lcur->label;

				lnew->Next  = 0;
				lnew->Prev  = gA(A0)->l1;
				gL(gA(A0)->l1)->Next=Lnew;
				gA(A0)->l1=Lnew;
			}

			L0n=gL(L0)->ln;
			L1n=L1;
		}


		++cont[Case];

		uint N0=a0->n0;Tn* n0=gN(N0);
		if (n0->finilized && isRegular(n0))
			vCollapse(N0,n0);

		/* end condition */
		if (a0->n1==Nend)
		{
			//getclock(&t2);
			//times[0]+=getdiffclock(&t2,&t1);
			//printf("(%.2f %.2f %d %d %d) ",times[0],times[1],cont[0],cont[1],cont[2]);fflush(stdout);
			return;
		}

		L0=L0n;
		L1=L1n;
	}
	
	CHECKPOINT(0);
	EXIT(-1);
}


/* ------------------------------------------------------------- */
/* ------------------------------------------------------------- */


#define stack_size()  (nstack)
#define stack_top()   (stack[nstack-1])
#define stack_pop()   (--nstack)

#define stack_push(N) \
{\
	if (nstack==mstack)\
	{\
		mstack=max2(128,mstack*2);\
		stack=(uint*)realloc(stack,sizeof(uint)*mstack);\
		CHECKPOINT(stack);\
	} \
	stack[nstack++]=(N);\
}


void ReebGraph::numLoops() const
{
	uint  N;
	uint  totloops=0;
	uint  ncomponent=0;
	
	uint  nstack=0,mstack=0;
	uint* stack=0;


	for (N=1;N<this->n.max;N++)
	{
		if (!bNFree(N)) 
			gN(N)->etc=0;
	}

	for(N=1;N<this->n.max;N++)
	{
		if (bNFree(N)) continue;

		if (!gN(N)->etc)
		{
			uint nloops=0;

			if (!gN(N)->adown && !gN(N)->aup)
				continue;

			++ncomponent;

			for (uint A=1;A<this->a.max;A++)
				if (!bAFree(A)) gA(A)->etc=0;

			++(gN(N)->etc);

			nstack=0;
			stack_push(N);

			while (stack_size())
			{
				CHECKPOINT(gN(N)->etc==1);N=stack_top();stack_pop();

				uint A;
				for (A=gN(N)->adown;A;A=gA(A)->a1n)
				{
					uint M=gA(A)->n0;
					if (!gN(M)->etc) {gA(A)->etc=1;++(gN(M)->etc);stack_push(M);}
				}

				for (A=gN(N)->aup;A;A=gA(A)->a0n)
				{
					uint M=gA(A)->n1;
					if (!gN(M)->etc) {gA(A)->etc=1;++(gN(M)->etc);stack_push(M);}
				}
			}
			++(gN(N)->etc);stack_push(N);

			while (stack_size())
			{
				N=stack_top();stack_pop();CHECKPOINT(gN(N)->etc==2);

				uint A;
				for (A=gN(N)->adown;A;A=gA(A)->a1n)
				{
					if (!gA(A)->etc) {++nloops;gA(A)->etc=1;}
					uint M=gA(A)->n0;
					if (gN(M)->etc==1) {++(gN(M)->etc);stack_push(M);} else{CHECKPOINT(gN(M)->etc==2);}
				}

				for (A=gN(N)->aup;A;A=gA(A)->a0n)
				{
					if (!gA(A)->etc) {++nloops;gA(A)->etc=1;}
					uint M=gA(A)->n1;
					if (gN(M)->etc==1) {++(gN(M)->etc);stack_push(M);} else{CHECKPOINT(gN(M)->etc==2);}
				}
			}

			totloops+=nloops;
			
		}
	}
	printf("reeb graph number_of_components=%d total_loops=%d\n",(int)ncomponent,(int)totloops);

	free(stack);
}

#undef stack_top
#undef stack_pop 
#undef stack_push
#undef stack_size












