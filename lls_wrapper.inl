

template <typename real_type>
LLSWrapper<real_type>::LLSWrapper(int nrows, int ncols) : rows(nrows), cols(ncols), b(nrows), x(ncols) {
	for (unsigned i=0; i<b.size(); i++) { b[i]=0; }
	for (unsigned i=0; i<x.size(); i++) { x[i]=0; }
}


template <typename real_type>
LLSWrapper<real_type>::~LLSWrapper() {
}


template <typename real_type>
void LLSWrapper<real_type>::InsertA(int r, int c, real_type val) {
	cols[c].push_back(IndexValPair(r, val));
	rows[r].push_back(IndexValPair(c, val));
}


template <typename real_type>
void LLSWrapper<real_type>::InsertB(int r, real_type val) {
	b[r] = val;
}


template <typename real_type>
void LLSWrapper<real_type>::InsertX(int r, real_type val) {
	x[r] = val;
}


template <typename real_type>
real_type LLSWrapper<real_type>::GetX(int r) {
	return x[r];
}


template <typename real_type>
void LLSWrapper<real_type>::Solve() {
	lsqr_input *input;
	lsqr_output *output;
	lsqr_work *work;
	lsqr_func *func;
	alloc_lsqr_mem(&input, &output, &work, &func, rows.size(), cols.size());

	input->cond_lim = 1.0e10; //1.0e5;	 // 1.0/(10.0*sqrt(1.0e-16));
	input->damp_val = 0.0;
	input->lsqr_fp_out = NULL;
	input->max_iter = cols.size();
	input->num_cols = cols.size();
	input->num_rows = rows.size();
	input->rel_mat_err = 1.0e-6;
	input->rel_rhs_err = 1.0e-6;
	for (int i=0; i<input->rhs_vec->length; i++) { input->rhs_vec->elements[i] = b[i]; }
	for (int i=0; i<input->sol_vec->length; i++) { input->sol_vec->elements[i] = x[i]; }
	for (int i=0; i<output->sol_vec->length;i++) { output->sol_vec->elements[i] = x[i]; }


	func->mat_vec_prod = static_mult;

	lsqr(input, output, work, func, this);
	if (output->term_flag > 2) {
		cerr<<"output->term_flag: "<<output->term_flag<<endl;
		BREAK;
	}
	for (int i=0; i<output->sol_vec->length; i++) { x[i] = output->sol_vec->elements[i]; }

	free_lsqr_mem(input, output, work, func);
}


// If MODE = 0, compute  y = y + A*x,
template <typename real_type>
void LLSWrapper<real_type>::multA(dvec *x, dvec *y) {
	for (unsigned r=0; r<rows.size(); r++) {
		for (unsigned ci=0; ci<rows[r].size(); ci++) {
			IndexValPair &ivp = rows[r][ci];
/*
			if (!isNUM(y->elements[r])) {
				BREAK;
			}
*/
			y->elements[r] += ivp.v * x->elements[ivp.i];
/*
			if (!isNUM(y->elements[r])) {
				BREAK;
			}
*/
		}
	}
}


// If MODE = 1, compute  x = x + A^T*y.
template <typename real_type>
void LLSWrapper<real_type>::multAt(dvec *x, dvec *y) {
	for (unsigned c=0; c<cols.size(); c++) {
		for (unsigned ri=0; ri<cols[c].size(); ri++) {
			IndexValPair &ivp = cols[c][ri];
			x->elements[c] += ivp.v * y->elements[ivp.i];
		}
	}
}


template <typename real_type>
void LLSWrapper<real_type>::static_mult(long mode, dvec *x, dvec *y, void *ptr) {
	if (mode==0)
		((LLSWrapper*)ptr)->multA(x,y);
	else
		((LLSWrapper*)ptr)->multAt(x,y);
}


