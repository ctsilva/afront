#ifndef _REEB_GRAPH_H__
#define _REEB_GRAPH_H__

#include "generaldef.h"

class ReebGraph
{
public:

	//constructor
	ReebGraph();

	//destructor
	~ReebGraph();

	void setBuckets(float minf,float maxf,int Num);

	//tell to terminate all hanging data structure, write statistics to stdout and cancel labels
	void  terminate();

	//add a vertex, returns the unique (return) <id> of the node in the reeb graph
	//you must use the return <id> for addPath,endVertex,collapse and so on
	//the input <id> is the id you give for that vertex from outside (used to enforce consistency to
	//compare two vertices/nodes)
	//
	//CONSISTENCY CHECK::= (v_a<v_b) iif value(v_a)<value(v_b) || (value(a)==value(b) && id(a)<id(b))
	
	uint  addVertex(uint id,float X,float Y,float Z,Tvalue value);

	//finilize a node <N>
	//make sure to call this function since it simplify the reeb graph
	//deleting useless labels and removing regular points (connectivity down,up 1-1)
	
	void  endVertex(const int N);

	//find if a Node<N> has an arch (up,down) with a label <Label> in it
	
	uint findUpLabel(uint N,uint64 Label) const;
	uint findDwLabel(uint N,uint64 Label) const;

	//add a monotonic path between nodes 
	//MAKE SURE the monocity of Nodes nodes[i]<nodes[i+1] (see CONSISTENCY CHECK)
	//MAKE SURE there isn't any link from node[i]->node[i+1] foreach(i) in[0..n) with label <Label>
	//    otherwise the library is going to crash,repetition are not allowed!
	
	void addPath(uint n,uint* Nodes,uint64 Label);
	
	//join (collapse) two path both departing from Nstart
	//the first path is labelled <Labela>, the second is labelled <Labelb>
	//MAKE SURE Nstart<Nend
	
	void  collapse(uint Nstart,uint Nend,uint64 Labela,uint64 Labelb);

	//print
	void print(FILE* file=stdout) const;

	//check (for debug mode)
	void check() const;

	//display (OpenGL display of the reeb graph)
	void glDisplay() const;

	//count (and print) number of loops
	void numLoops() const;

	//do not care (and do not MODIFY) internal representation
#include "rgpriv.h"

};

/* macro to access the scalar field of a node */
#define rg_gN(rg,N) rg->n.mem[N]

#endif





