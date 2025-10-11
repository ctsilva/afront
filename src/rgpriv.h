public:


	//node structure
	typedef struct
	{
		uint   id;
		float  X,Y,Z;
		Tvalue value;
		uint   adown;
		uint   aup;
		uchar  etc;
		uchar  finilized;
	} 
	Tn;

	//arch structure
	typedef struct
	{
		uint  n0,a0p,a0n;
		uint  n1,a1p,a1n;
		uint  l0,l1;
		uint  b0,b1;
		uchar etc;
	} 
	Ta;

	//label structure
	typedef struct
	{
		uint   a;
		uint   Prev,Next;
		uint64 label;
		uint   lp,ln;
	} 
	Tl;

	//bucket structure
	typedef struct
	{
		float  X,Y,Z;
		uint   Num;
		uint   Prev,Next;
	} 
	Tb;


	struct {uint max,num,ff;Ta* mem;} a;void memory_n(uint n);
	struct {uint max,num,ff;Tn* mem;} n;void memory_a(uint n);
	struct {uint max,num,ff;Tl* mem;} l;void memory_l(uint n);
	struct {uint max,num,ff;Tb* mem;} b;void memory_b(uint n);

	//buckets
	float minf,maxf;
	uint  bucket_samples;

	//number of vertices and number of collapsing operation
	uint nvertices,ncollapse; 

	//number of vertices finilized
	uint nendvertex;

	//calculate time to finish
	TTime t1,t2;

	//find a label
	void findUpLabel(uint& L,uint N0,uint N1);

	//first init function
	void init();

	//last dispose function
	void dispose();



