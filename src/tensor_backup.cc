#include <tensor.h>
//#define VERBOSE_DEBUG

extern char *InitType_names[];

bool *G_isMatched, *H_isMatched;
double *rows, *cols, *edge_weights, *mi, *mj; // matching aux. vectors



/**
 * n the number of nodes
 * m the number of nodes
 * nedges the number of edges
 * vv1 is the source for each of the nedges 
 * vv2 is the target for each of the nedges
 * weight is the weight of each of the nedges
 * out1 is a vector of length at most min(n,m),
 * out2 is a vector of length at most min(n,m),
 * noutedges is the number of out edges
 */

double match(int n, int m, int nedges, double *vv1, double *vv2, double *weight, double *out1, double *out2, int *noutedges) {	
	
/*	double **adj = (double **)calloc(n, sizeof(double *));
	for(register int ii = 0; ii < n; ii++) {
		adj[ii] = (double *) calloc(m, sizeof(double));
	}
	for(register int ii = 0; ii < nedges; ii++) {
		adj[(int)(vv1[ii])][(int)(vv2[ii])] = weight[ii];
	}
	
	char path[1024];
	sprintf(path, "%s/match_%d.log", output_path, ++counter);
	FILE *log = fopen(path, "w");
	for(register int ii = 0; ii < n; ii++) {
		for(register int jj = 0; jj < m; jj++) {
			fprintf(log, "%e\t", adj[ii][jj]);
		}
		fprintf(log, "\n");
	}	
	fclose(log);
*/
	
	double ret, al;
	double *l1, *l2, *w;
	int *match1, *match2, *v1, *v2;
	int i, j, k, p, q, r, t1, t2;
	int *s, *t, *deg, *offset, *list;

	l1 = new double[n];
	l2 = new double[n+m];
	v1 = new int[nedges];
	v2 = new int[nedges];
	s = new int[n];
	t = new int[n+m];
	match1 = new int[n];
	match2 = new int[n+m];
	offset = new int[n];
	deg = new int[n];
	list = new int[nedges + n];
	w = new double[nedges + n];
		

	for (i = 0; i < nedges; i++) {
		v1[i] = (int)(vv1[i] + .5);
		v2[i] = (int)(vv2[i] + .5);
	}
	for (i = 0; i < n; i++) {
		offset[i] = 0;
		deg[i] = 1;
	}
	for (i = 0; i < nedges; i++) deg[v1[i]]++;
	for (i = 1; i < n; i++) offset[i] = offset[i-1] + deg[i-1];
	for (i = 0; i < n; i++) deg[i] = 0;
	for (i = 0; i < nedges; i++) {
		list[offset[v1[i]] + deg[v1[i]]] = v2[i];
		w[offset[v1[i]] + deg[v1[i]]] = weight[i];
		deg[(int)v1[i]]++;
	}
	for (i = 0; i < n; i++) {
		list[offset[i] + deg[i]] = m + i;
		w[offset[i] + deg[i]] = 0;
		deg[i]++;
	}
	for (i = 0; i < n; i++) {
		l1[i] = 0;
		for (j = 0; j < deg[i]; j++) {
			if (w[offset[i]+j] > l1[i]) l1[i] = w[offset[i] + j];
		}
	}
	for (i = 0; i < n; i++) {
		match1[i] = -1;
	}
	for (i = 0; i < n + m; i++) {
		l2[i] = 0;
		match2[i] = -1;
	}
	for (i = 0; i < n; i++) {
		for(j = 0; j < n + m; j++) t[j] = -1;
		s[p = q = 0] = i;
		for(; p <= q; p++) {
			if (match1[i] >= 0) break;
			k = s[p];
			for (r = 0; r < deg[k]; r++) {
				if (match1[i] >= 0) break;
				j = list[offset[k] + r];
				if (w[offset[k] + r] < l1[k] + l2[j] - 1e-8) continue;
				if (t[j] < 0) {
					s[++q] = match2[j];
					t[j] = k;
					if (match2[j] < 0) {
						for(; j>=0 ;) {
							k = match2[j] = t[j];
							p = match1[k];
							match1[k] = j;
							j = p;
						}
					}
				}
			}
		}
		if (match1[i] < 0) {
			al = 1e20;
			for (j = 0; j < p; j++) {
				t1 = s[j];
				for (k = 0; k < deg[t1]; k++) {
					t2 = list[offset[t1] + k];
					if (t[t2] < 0 && l1[t1] + l2[t2] - w[offset[t1] + k] < al) {
						al = l1[t1] + l2[t2] - w[offset[t1] + k];
					}
				}
			}
			for (j = 0; j < p; j++) l1[s[j]] -= al;
			for (j = 0; j < n + m; j++) if (t[j] >= 0) l2[j] += al;
			i--;
			continue;
		}
	}
	ret = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < deg[i]; j++) {
			if (list[offset[i] + j] == match1[i]) {
				ret += w[offset[i] + j];
			}
		}
	}
    *noutedges = 0;
    for (i = 0; i < n; i++) {
        if (match1[i] < m) (*noutedges)++;
    }
    *noutedges = 0;
    for (i = 0; i < n; i++) {
        if (match1[i] < m) {
            out1[*noutedges] = i;
            out2[*noutedges] = match1[i];
            (*noutedges)++;
        }
    }

    delete [] l1;
    delete [] l2;
    delete [] v1;
    delete [] v2;
    delete [] s;
    delete [] t;
	delete [] match1;
	delete [] match2;
    delete [] offset;
    delete [] deg;
    delete [] list;
    delete [] w;	
    
	return ret;
}

double ProdTensor::norm(double *v, unsigned long n) { 
	unsigned register long int i;  
	register double sqsum = 0;
	
	for (i=0; i < n; i++) {
		sqsum += (v[i]*v[i]);
	}
    
    return sqrt(sqsum);
}

/* v_norm = v / norm(v, 2);
* Input) v: a vector of size n, n: len of vector v
* Output) v_norm
*/
double ProdTensor::normalize(double *v_norm, double *v, unsigned long n) { 
    double Norm =  norm(v, n);
    if(Norm < 1e-52) 
		return 0;		
    double scale = 1 / Norm;    

	for (register unsigned int i = 0; i < n; i++) {
		v_norm[i] = v[i]*scale;
	}
	
	return Norm;
}




int ProdTensor::initMatchingDatasets() {
	rows = (double *)calloc(n, sizeof(double));
	if(rows == NULL) {
		fprintf(stderr, "ProdTensor::initMatchingDatasets Can't allocate memory for 'rows'\n");
		return -3;
	}
	
	
	cols = (double *)calloc(n, sizeof(double));
	if(cols == NULL) {
		fprintf(stderr, "ProdTensor::initMatchingDatasets Can't allocate memory for 'cols'\n");
		return -4;
	}

	edge_weights = (double *)calloc(n, sizeof(double));
	if(edge_weights == NULL) {
		fprintf(stderr, "ProdTensor::initMatchingDatasets Can't allocate memory for 'edge_weights'\n");
		return -5;
	}
		
    
	mi = (double *)calloc(min(n1, n2), sizeof(double));
	if(mi == NULL) {
		fprintf(stderr, "ProdTensor::initMatchingDatasets Can't allocate memory for 'mi'\n");
		return -6;
	}
	
	mj = (double *)calloc(min(n1, n2), sizeof(double));
	if(mj == NULL) {
		fprintf(stderr, "ProdTensor::initMatchingDatasets Can't allocate memory for 'mj'\n");
		return -7;
	}	
	
	return 0;
}


ProdTensor::ProdTensor(Graph *G, TriangleCounts *T_G, Graph *H, TriangleCounts *T_H, double *w, Sparsity_Type sparsity_type, char *output_path, char *prefix) {
	//tensor parameters
	this->m = 3;
	this->G = G;
	this->H = H;
	this->T_G = T_G;
	this->T_H = T_H;

	this->n1 = this->G->n;
	this->n2 = this->H->n;
	this->n = n1*n2; /* number of edges in L */

	memcpy(this->prefix, prefix, 1024);	
	memcpy(this->output_path, output_path, 1024);	

	if(initMatchingDatasets() != 0) {
		fprintf(stderr, "ProdTensor:: Can't initialize matching datasets.\n");
		exit(-1);
	}

	this->sparsity_type = sparsity_type;
	printf("Sparsity flag = %d\n", sparsity_type);
	unsigned register int i, j;

	// WARNING! This has to be set even in sparse case since we need it currently. This feature is developed for fixing "known" alignments, but needs revision
	unmatched_rows.resize(n1);
	for(i = 0; i < n1; i++) {
		unmatched_rows[i] = i;
	}
	unmatched_cols.resize(n2);
	for(i = 0; i < n2; i++) {
		unmatched_cols[i] = i;
	}


	if( this->sparsity_type != NoSparsity ) {
		int total_matches = 0;
		for(i = 0; i < n1; i++) {
			for(j = 0; j < n2; j++) {
				if( w[i*n2 + j] != 0 ) {
					total_matches ++;
					unmatched_pairs_row.push_back(i);
					unmatched_pairs_col.push_back(j);
				}
			}
		}
		printf("Total matches in L = %d, size unmatched_pairs = %d\n", total_matches, (int)unmatched_pairs_row.size());
	}
}


void ProdTensor::impTTV(double *new_x, double *x) {

	if( this->sparsity_type == NoSparsity ) {
		unsigned int *Ni1, *Ni2, *p, *q;
		register unsigned int i1, j1, k1, i2, j2, k2, idx, cur_row, cur_col;

		// Ensure fixed submatrices are untouched.
		memcpy(new_x, x, n*sizeof(double));


		for(cur_row = 0; cur_row < unmatched_rows.size(); cur_row++) {
			i1 = unmatched_rows[cur_row];
			Ni1 = T_G->getAdjTriangles(i1);
			for(cur_col = 0; cur_col < unmatched_cols.size(); cur_col++) {
				i2 = unmatched_cols[cur_col];
				Ni2 = T_H->getAdjTriangles(i2);
				idx = n2*i1 + i2; // X[i1][i2];
				new_x[idx] = 0; // resets new_x[idx]
				for(p = Ni1+1; p <= Ni1 + *Ni1; ) {
					j1 = *p++; k1 = *p++;
					for(q = Ni2+1; q <= Ni2 + *Ni2; ) {
						j2 = *q++; k2 = *q++;
						new_x[idx] += (x[n2*j1 + j2] * x[n2*k1 + k2]) + (x[n2*j1 + k2] * x[n2*k1 + j2]); // (m-1)!*( (m-1)! - 1 ) non-redundant combinations
					}
				}
				new_x[idx] *= 2; // To account for the symmetry in T, i.e., X[j1][j2]*X[k1][k2] vs X[k1][k2]*X[j1][j2], (m-1)! redundancy in permutations.
			}
		}
	}
	else {
		bzero(new_x, this->n*sizeof(double));
		unsigned int *Ni1, *Ni2, *p, *q;

		register unsigned int cur_pair, i1, i2, j1, j2, k1, k2, idx;
		for(cur_pair = 0; cur_pair < unmatched_pairs_row.size(); cur_pair++) {
			i1 = unmatched_pairs_row[cur_pair];
			i2 = unmatched_pairs_col[cur_pair];
			idx = n2*i1 + i2; // X[i1][i2];

			Ni1 = T_G->getAdjTriangles(i1);
			Ni2 = T_H->getAdjTriangles(i2);

			for(p = Ni1+1; p <= Ni1 + *Ni1; ) {
				j1 = *p++; k1 = *p++;
				for(q = Ni2+1; q <= Ni2 + *Ni2; ) {
					j2 = *q++; k2 = *q++;
					new_x[idx] += (x[n2*j1 + j2] * x[n2*k1 + k2]) + (x[n2*j1 + k2] * x[n2*k1 + j2]); // (m-1)!*( (m-1)! - 1 ) non-redundant combinations
				}
			}
			new_x[idx] *= 2; // To account for the symmetry in T, i.e., X[j1][j2]*X[k1][k2] vs X[k1][k2]*X[j1][j2], (m-1)! redundancy in permutations.
		}
	}
}

// x is row-order linearized version of matrix X
stats ProdTensor::evalX(double *x) {
	stats myStats;
	int noutedges;

	unsigned register int j, k, l;


	double v1 = 0, v2 = 0, v4 = 0, temp;
	for(j = 0; j < (unsigned int)n; j++) {
		temp = x[j];
		v1 += temp;
		temp *= temp;
		v2 += temp;
		temp *= temp;
		v4 += temp;
	}
	myStats.locality = round(v2*v2/v4);
	

	setupBipartiteGraph(x);
	myStats.matching_score = match(n1, n2, unmatched_rows.size()*unmatched_cols.size(), rows, cols, edge_weights, mi, mj, &noutedges);	
	myStats.num_matches = noutedges;

	myStats.correctness = 0;
	int common_subgraph_size = 2000;
	for(j = 0; j < (unsigned)noutedges; j++) {
		if(mi[j] < common_subgraph_size && mj[j] < common_subgraph_size && mi[j] == mj[j])
			myStats.correctness ++;
	}
	myStats.correctness = 100 * myStats.correctness / common_subgraph_size;

	wall_clock timer;
	timer.tic();

	// Construct alignment graph
	bool **alignGraph = (bool **) calloc(noutedges, sizeof(bool *));
	if(alignGraph == NULL) {
		fprintf(stderr, "\t\tissHOPM:: Can't allocate memory for alignGraph.\n");
		exit(-1);
	}
	for(j = 0; j < (unsigned)noutedges; j++) {
		alignGraph[j] = (bool *) calloc(noutedges, sizeof(bool));
		if(alignGraph[j] == NULL) {
			fprintf(stderr, "\t\tissHOPM:: Can't allocate memory for alignGraph[%d].\n", j);
			exit(-1);
		}
	}

	register unsigned int i1, i2, j1, j2;
	for(j = 0; j < (unsigned)noutedges; j++) {
		i1 = mi[j];
		i2 = mj[j];		
		for(k = 0; k < (unsigned)noutedges; k++) {
			j1 = mi[k];
			j2 = mj[k];		
			if(G->getEdge(i1, j1) && H->getEdge(i2, j2)) {
				alignGraph[j][k] = true;
			}
		}		
	}

	timer.tic();
	// Count conserved triangles/edges
	myStats.T = myStats.M = 0;
	for(j = 0; j < (unsigned)noutedges; j++) {
		for(k = j+1; k < (unsigned)noutedges; k++) {
			if( !alignGraph[j][k] )
				continue;	
			myStats.M++;				
			for(l = k+1; l < (unsigned)noutedges; l++) {
				if( alignGraph[j][l] && alignGraph[k][l] ) {
						myStats.T++;
				}										
			}
		}
	}							

	return myStats;	
}

//void ProdTensor::Shift(double *x_hat, double *x, double *w, double alpha, double beta, int shift_type) {
void ProdTensor::Shift(int shift_type) {

/*
	unsigned register int j;
	switch(shift_type) {
		case(VARIABLE_SHIFT):
			// Shift using x (inertia idea)
			for(j = 0; j < n; j++) {
				x_hat[j] += alpha*x[j];
			}
			break;
		case(AFFINE_SHIFT):
			// Shift using SeqSim
			for(j = 0; j < n; j++) {
				x_hat[j] += (beta*this->rho*w[j]);
			}
			break;
		case(NORM_AFFINE_SHIFT):
			// Normalize first:: x_hat <-- ( x_hat / norm(x_hat, 2) )
			if(normalize(x_hat, x_hat, n)  == 0)  {
			  fprintf(stderr, "issHOPM:: x_hat vector has norm 0 after first norm.\n");
			  res->flag = -1;
			  return;
			}
			// Then shift using SeqSim
			for(j = 0; j < n; j++) {
				x_hat[j] = beta*x_hat[j] + (1-beta)*w[j];
			}
			break;
		case(COMBINED_SHIFT):
			for(j = 0; j < n; j++) {
				x_hat[j] += ((1 - beta)*arma::norm(x_hat_vec)*w[j] + alpha*x[j]);
			}
			break;
	}
	*/
	printf("\t\t\t\tShift:: Shift type = %d, alpha = %.2e, beta=%.2e, sigma = %.2e, rho =%.2f\n", shift_type, alpha, beta, sigma, rho);
	printf("\t\t\t\tShift:: ||x_hat_vec|| = %.2f, Effective x_vec = %.2e, Effective weight w = %.2e\n", arma::norm(x_hat_vec), this->alpha / arma::norm(x_hat_vec), this->sigma / arma::norm(x_hat_vec));

/*	register int i;
	vec x_norm = x_hat_vec;
	double x_hat_sum = 0, w_sum = 0;
	for (i = 0; i < 2000; i++) {
		x_hat_sum += x_norm(i*n2+i);
		w_sum += w_vec(i*n2+i);
	}
	printf("\t\t\t\tSum(diag(w)) = %e, sum(diag(x_norm)) = %e\n", x_hat_sum, w_sum);
*/

	switch(shift_type) {
		case(VARIABLE_SHIFT):
			x_hat_vec = x_vec + this->alpha*x_vec;
			break;
		case(AFFINE_SHIFT):
			// Shift using SeqSim
			x_hat_vec = x_hat_vec + this->sigma*this->rho*w_vec;
			break;
		case(NORM_AFFINE_SHIFT):
			x_hat_vec = (x_hat_vec / arma::norm(x_hat_vec)) + this->sigma*w_vec;
			break;
		case(COMBINED_SHIFT):
			if(this->beta == 0) // Topological weight = 0 -> Only SeqSim
				x_hat_vec = w_vec + this->alpha*x_vec;
			else if(this->beta == 1) // Topological weight = 1 -> SS-HOPM
				x_hat_vec = x_hat_vec + this->alpha*x_vec;
			else {
				x_hat_vec = x_hat_vec + this->alpha*x_vec + this->sigma*w_vec;//this->sigma*arma::norm(x_hat_vec)*w_vec;
				/*
				x_hat_vec = x_hat_vec + this->alpha*x_vec;
				x_hat_vec = x_hat_vec % (this->sigma*w_vec);
				*/
			}
			break;
	}
}

alignment* ProdTensor::postprocess(double *x_final, int max_iter, int max_degree) {
/*
	this->n1 = this->G->n;
	this->n2 = this->H->n;
	this->n = n1*n2;  number of edges in L
*/
	printf("Post-processing ... \n");
	int bMatching_alg = 13;
    bool verbose = false;
	int fixed_b = max(0, max_degree - 1); // Hungarian edge + (b-1)

	register unsigned int i, i_prime, j, j_prime, k, l, s, t, it;
	double *temp_x = new double[this->n];
	memcpy(temp_x, x_final, this->n*sizeof(double));

	alignment *align = new alignment;

	stats myStats = evalX(temp_x);
	align->conserved_edges = myStats.M;
	align->conserved_triangles = myStats.T;
	align->match_no = myStats.num_matches;
	printf("\tInitial edges = %ld, triangles = %ld\n", align->conserved_edges, align->conserved_triangles);

	align->left_match.resize(align->match_no);
	align->right_match.resize(align->match_no);

	align->PrefH = new vector<int>[this->n1];
	align->right_project = new int[this->n1];
	for(i = 0; i < this->n1; i++) {
		align->right_project[i] = -1;
	}

	align->PrefG = new vector<int>[this->n2];
	align->left_project = new int[this->n2];
	for(i = 0; i < this->n2; i++) {
		align->left_project[i] = -1;
	}

	for (i = 0; i < align->match_no; i++) {
		align->left_match[i] = (int)mi[i];
		align->right_match[i] = (int)mj[i];

		align->PrefG[align->right_match[i]].push_back(align->left_match[i]);
		align->PrefH[align->left_match[i]].push_back(align->right_match[i]);

		align->left_project[align->right_match[i]] = align->left_match[i];
		align->right_project[align->left_match[i]] = align->right_match[i];

		temp_x[align->left_match[i]*this->n2+align->right_match[i]] = 0;
	}


	printf("\tConstructing initial graph for bMatching\n"); fflush(stdout);
    CSR G;
    G.DenseVec2CSR(temp_x, this->n1, this->n2);
    int *b=(int*)malloc(G.nVer*sizeof(int));
    for(i = 0; i < (unsigned int)G.nVer; i++) {
		int card=0;
		for(j = G.verPtr[i]; j < (unsigned int)G.verPtr[i+1]; j++)
			if(G.verInd[j].weight>0)
				card++;

		if(card>fixed_b)
			b[i]=fixed_b;
		else b[i]=max(1, card);
    }


	/*******************************
	 *    Other datastructures
	 ******************************/
    int* nlocks=(int*)malloc(G.nVer*sizeof(int));    //required for all schemes
    int* start=(int*)malloc(G.nVer*sizeof(int));     //required for sorted and part. sorted
    int* end=(int*)malloc(G.nVer*sizeof(int));       //required for part sorted
    char* mark=(char*)malloc(G.nEdge*sizeof(char));     //required for unsorted and part sorted


    Walker* W = NULL;
    if(bMatching_alg==0 || bMatching_alg==6)
    {
        W=(Walker*)malloc(sizeof(Walker));
        W->M1=(float*)malloc(G.nVer*3*sizeof(float));
        W->M2=(float*)malloc(G.nVer*3*sizeof(float));
    }


    Node* S1;
    if(bMatching_alg==0 || bMatching_alg== 5)
        S1=(Node*)malloc(G.nVer*sizeof(Node));      //Heap data structure
    Node* S=(Node*)malloc(G.nVer*sizeof(Node));      //Heap data structure
    printf("Size = %d, sum = %d\n", G.nVer, this->n1+this->n2);
	for(i = 0; i < (unsigned int)G.nVer; i++)
	{
		S[i].heap=(Info*)malloc(max(1, b[i])*sizeof(Info));      //Each heap of size b
		S[i].curSize = 0;
		if(bMatching_alg==5 || bMatching_alg ==0)
			S1[i].heap=(Info*)malloc(max(1, b[i])*sizeof(Info));      //Each heap of size b
	}

    stack_pcl<Param>* St=NULL;
    int* order=NULL;
    float M[2];
    int best;
	int stepM = 3; // step size for partial sorting

    /*******************************
     *    Other datastructures
     ******************************/
	printf("\tRunning bMatching\n"); fflush(stdout);
    switch(bMatching_alg)
    {
        case 0:
                break;

        case 1: //greedy(&G,S,b,verbose);
                break;

        case 2: localDom(&G,S,b,start,end,nlocks,stepM,1,verbose);
                break;

        case 3: localDom(&G,S,b,start,end,nlocks,stepM,2,verbose);
                break;

        case 4: localDom(&G,S,b,start,end,nlocks,stepM,4,verbose);
                break;

        case 5: best=PG(&G,S,S1,b,M,mark,end);

                if(best==1)
                    S=S1;
                break;

        case 6: PGDP(&G,S,b,W,end,verbose);
                break;

        case 7: //heavyMatch(&G,S,start,verbose);
                break;

        case 8: bSuitorBPQ(&G,b,nlocks,S,start,end,mark,order,1,stepM,St,verbose);
                break;

        case 9: bSuitorBPQ(&G,b,nlocks,S,start,end,mark,order,2,stepM,St,verbose);
                break;

        case 10: bSuitorBPQ(&G,b,nlocks,S,start,end,mark,order,4,stepM,St,verbose);
                break;

        case 11: bSuitorBPQD(&G,b,nlocks,S,start,end,mark,order,1,stepM,St,verbose);
                break;

        case 12: bSuitorBPQD(&G,b,nlocks,S,start,end,mark,order,2,stepM,St,verbose);
                break;

        case 13: bSuitorBPQD(&G,b,nlocks,S,start,end,mark,order,4,stepM,St,verbose);
                break;

    }


   printf("\tAdding bMatches to H preferred set\n"); fflush(stdout);
	for (i = 0; i < this->n1; i++) {
        for(j = 0; j < (unsigned int) S[i].curSize; j++) {
    		align->PrefH[i].push_back((S[i].heap[j].id) - this->n1);
        }
	}


    printf("\tAdding bMatches to G preferred set\n"); fflush(stdout);
	for (i = 0; i < this->n2; i++) {
        for(j = 0; j < (unsigned int) S[i+this->n1].curSize; j++) {
    		align->PrefG[i].push_back(S[i+this->n1].heap[j].id);
        }
	}


	/*for(i = 0; i < align->match_no; i++) {
		printf("Mi[%d] =  %d (%s) in G\n", i, align->left_match[i], this->G->getVertexName(align->left_match[i]));
		for(j = 0; j < align->PrefH[align->left_match[i]].size(); j++) {
			printf("\tVertex %d (%s) in H\n", align->PrefH[align->left_match[i]][j], this->H->getVertexName(align->PrefH[align->left_match[i]][j]));
		}
	}


	for(i = 0; i < align->match_no; i++) {
		printf("Mj[%d] =  %d (%s) in H\n", i, align->right_match[i], this->H->getVertexName(align->right_match[i]));
		for(j = 0; j < align->PrefG[align->right_match[i]].size(); j++) {
			printf("\tVertex %d (%s) in G\n", align->PrefG[align->right_match[i]][j], this->G->getVertexName(align->PrefG[align->right_match[i]][j]));
		}
	}*/


	// Prune Pref sets to only have homogenously high SeqSim entries (remove ones that are significantly lower that the others)

	Move new_move, best_move;
	new_move.e.resize(2); // We only consider moves of size up to 2 (size 4 paths)
	new_move.m_id.resize(2);

	vector<int> right_unmatched, left_unmatched, matched_candidates;
	for(it = 0; it < (unsigned int)max_iter; it++) {
		for(k = 0; k < align->match_no; k++) {
			i = align->left_match[k]; i_prime = align->right_match[k];

			left_unmatched.clear(); // Nodes on the left side (G) that are in Pref set of i' and are either matched/unmatched
			for(l = 0; l < align->PrefG[i_prime].size(); l++) {
				j = align->PrefG[i_prime][l];
				if(align->right_project[j] == -1) {
					left_unmatched.push_back(j);
				}
			}

			right_unmatched.clear();
			for(l = 0; l < align->PrefH[i].size(); l++) {
				j_prime = align->PrefH[i][l];
				if(align->left_project[j_prime] == -1) {
					right_unmatched.push_back(j_prime);
				}
			}

			new_move.move_no = 1;
			new_move.m_id[0] = k;

			new_move.e[0][1] = i_prime;
			for(l = 0; l < left_unmatched.size(); l++) {
				j = left_unmatched[l];
				new_move.e[0][0] = j;
			}

			new_move.e[0][0] = i;
			for(l = 0; l < right_unmatched.size(); l++) {
				j_prime = right_unmatched[l];
				new_move.e[0][1] = j_prime;
			}

			// Now look at already matched edges and evaluate them as swap candidates
			matched_candidates.clear();
			new_move.move_no = 2;
			for(l = 0; l < align->match_no; l++) {
				if(l == k)
					continue;
				j = align->right_match[l];
				j_prime = align->left_match[l];

				new_move.e[0][0] = i;
				new_move.e[0][1] = j_prime;
				new_move.e[1][0] = j;
				new_move.e[1][1] = i_prime;

				// Prune based on preferred sets
				// 1) Make sure j' \in PrefH(i)
				for(s = 0; s < align->PrefH[i].size(); s++) {
					if(align->PrefH[i][s] == (int)j_prime)
						break;
				}
				if(s == align->PrefH[i].size())
					continue;

				// 1) Make sure j \in PrefG(i')
				for(s = 0; s < align->PrefG[i_prime].size(); s++) {
					if(align->PrefG[i_prime][s] == (int)j)
						break;
				}
				if(s == align->PrefG[i].size())
					continue;

				// prune based on seqsim, if needed

				// Prune based on edges

				evaluateMove(new_move);
			}
		}

	}
	return align;
}

double 	ProdTensor::evaluateMove(Move &new_move) {
	double score = 0;

	return score;
}

eigen *ProdTensor::issHOPM(int max_it, double shift_param, double weight_param, double epsilon, double *w, double *x0, int init_type) {
	
	register unsigned int i, j;
	char x_path[1024];
	
	this->best_x = vec(x0, this->n);
	this->x_vec = vec(x0, this->n); //.set_size(this->n, 1);
	this->x_hat_vec.set_size(this->n, 1);
	this->w_vec = vec(w, this->n);
	
	this->X.set_size(this->n1, this->n2);


	this->res = new eigen;
	this->res->flag = -2;

	// Misuse of notation in the paper. Let's change the order to avoid confusion
	this->alpha = shift_param;

	printf("ISS-HOPM::Max Iter = %d, Alpha (shift) = %e, Beta (weight) = %e, Epsilon = %e\n", max_it, this->alpha, this->beta, epsilon); 	fflush(stdout);


	stats myStats, bestStats; bestStats.T = bestStats.M = -1;
	wall_clock timer;

	
	double old_lambda = -datum::inf, delta_lambda;

	for(i = 0; i < (unsigned int)max_it; i++) {
		timer.tic();
		printf("\t\tIteration %d ... \n", i+1); fflush(stdout);


		/*************************************
		 * Eigen-pair computations starts here
		 *************************************/
		impTTV(x_hat_vec.memptr(), x_vec.memptr()); // x_hat_vec(i) <-- T*x_(i-1)^2
		printf("\t\t\tdt impTTV = %f (norm x_hat = %.2f)\n", timer.toc(), arma::norm(x_hat_vec));

		// Compute Lambda first: x_(i-1) * x_hat_(i)
		timer.tic();
		res->lambda = dot(x_hat_vec, x_vec);
		printf("\t\t\tdt dot = %f\n", timer.toc());

		// Then shift x_hat_vec(i) ...
		timer.tic();
		printf("\t\t\tShifting now:\n");
		//Shift(x_hat_vec.memptr(), x_vec.memptr(), w, alpha, beta, shift_type);
		//Shift(shift_type);
		x_hat_vec += this->alpha*x_vec;			
		printf("\t\t\tdt Shift = %f (norm x_hat_vec = %e)\n", timer.toc(), arma::norm(x_hat_vec));



		// And finally normalize x_hat_vec(i) wrt previous vectors to have ||x_vec(i)|| = 1
		timer.tic();
		x_vec = x_hat_vec / arma::norm(x_hat_vec);
		printf("\t\t\tdt normalization = %f (norm(x_vec) = %.2f)\n", timer.toc(), arma::norm(x_vec));
		fflush(stdout);



		/*
		printf("\t\tMixing x_vec with SeqSim\n");
		timer.tic();

		double topo_median = arma::median(nonzeros(x_vec));
		printf("\t\t\tdt median = %f, median = %e\n", timer.toc(), topo_median);

		timer.tic();
		for(ii = 0; ii < this->n; ii++) {
			if(0 < (x_vec.memptr())[ii])
				topo_absolute_diff[ii] = abs((x_vec.memptr())[ii] - topo_median);
		}
		printf("\t\t\tdt abs diff = %f\n", timer.toc());

		timer.tic();
		double topo_mad = arma::median(nonzeros(topo_absolute_diff));
		printf("\t\t\tdt MAD = %f, MAD = %e\n", timer.toc(), topo_mad);

		timer.tic();
		for(ii = 0; ii < this->n; ii++) {
			if(0 < (x_vec.memptr())[ii]) {
				double topo_vec_z_neg = 0.6745*(topo_median - (x_vec.memptr())[ii]) / topo_mad;
				(topo_sigmoid.memptr())[ii] = 1 / (1 + exp(topo_vec_z_neg));
			}
		}
		printf("\t\t\tdt computing sigmoid %f\n", timer.toc());
		mixed_sim = this->beta*topo_sigmoid + (1-this->beta)*seqsim_sigmoid;
*/


		/*********************************************************
		 * Done with computation, time to evaluate and export now!
		 ********************************************************/
		// Evaluate current results
		timer.tic();
		delta_lambda = abs(res->lambda-old_lambda);
		//myStats = evalX(x_vec.memptr());
		myStats = evalX(x_vec.memptr()); // also sets mi, mj because it runs matching
		printf("\t\t\tmatchin_score = %e, matched edges = %ld, matched tries = %ld, locality = %.2e, dt = %f\n", myStats.matching_score, myStats.M, myStats.T, myStats.locality, timer.toc());
		fflush(stdout);

		//x_vec = (this->beta/myStats.matching_score)*x_vec + (1-this->beta)*w_vec;
		

		/*
		// Run 2nd round of matching after scaling
		timer.tic();		
		mixed_x = (this->beta/myStats.matching_score)*arma::log10(1+100*x_vec) + (1-this->beta)*w_vec;
		myStats = evalX(mixed_x.memptr()); // also sets mi, mj because it runs matching
		printf("\t\t\tmatchin_score (2nd round) = %e, matched edges = %ld, matched tries = %ld, dt = %f\n", myStats.matching_score, myStats.M, myStats.T, timer.toc());
		fflush(stdout);
		*/

		// Export binarized matrix X
		timer.tic();
		sprintf(x_path, "%s/%s_alpha=%.2e_beta=%.2e_it%d_X.smat", output_path, prefix, alpha, beta, i+1);
		FILE *fd = fopen(x_path, "w");
		fprintf(fd, "%d\t%d\t%d\n", this->n1, this->n2, myStats.num_matches);
		for(j = 0; j < myStats.num_matches; j++) {
			fprintf(fd, "%d\t%d\t1\n", (int)mi[j], (int)mj[j]);
		}
		fclose(fd);
		printf("\t\t\tdt binary export = %f\n", timer.toc());


		if(bestStats.T < myStats.T) {
			bestStats.matching_score = myStats.matching_score;
			bestStats.M = myStats.M;
			bestStats.T = myStats.T;
			this->best_x = x_vec;
		}			
					



		if(delta_lambda < epsilon) {
			break;
		}
		else {
			old_lambda = res->lambda;
		}
	}


	/**********************************************
	 * 			Clean output
	 * *******************************************/	 
	// Export full matrix X
/*
	timer.tic();
	X = (reshape(best_x, n2, n1)).t();
	sprintf(x_path, "%s/%s_alpha=%.2e_beta=%.2e_X.mat", output_path, prefix, alpha, beta);
	X.save(x_path, raw_ascii);
	printf("\t\t\tdt full export = %f\n", timer.toc());
*/

	alignment* result = postprocess(best_x.memptr(), 1, 10);
	printf("After post processing:: Triangles = %ld, edges = %ld\n ", result->conserved_triangles, result->conserved_edges);
	

/*	// Export binarized matrix X
	myStats = evalX(best_x.memptr()); // also sets mi, mj because it runs matching
	printf("\t\t\tFINAL matchin_score = %e, matched edges = %ld, matched tries = %ld, dt = %f\n", myStats.matching_score, myStats.M, myStats.T, timer.toc());
	fflush(stdout);

	timer.tic();
	sprintf(x_path, "%s/%s_alpha=%.2e_beta=%.2e_X.smat", output_path, prefix,  alpha, beta);
	FILE *fd = fopen(x_path, "w");
	fprintf(fd, "%d\t%d\t%d\n", this->n1, this->n2, myStats.num_matches);
	for(j = 0; j < myStats.num_matches; j++) {
		fprintf(fd, "%d\t%d\t1\n", (int)mi[j], (int)mj[j]);
	}
	fclose(fd);
*/


/*	vector<int> MI, MJ;
	for(i = 0; i < myStats.num_matches; i++) {
		MI.push_back(mi[i]);
		MJ.push_back(mj[i]);
	}
	printf("Tris = %ld\n", countTrianglesUnderAlignment(MI, MJ));

	long Tsum = 0;
	for(i = 0; i < myStats.num_matches; i++) {
		long curr_sum = DeltaT_removeMatch(MI, MJ, i);
		printf("%d- %ld\n", i, curr_sum);
		Tsum += curr_sum;
	}
	printf("Total sum = %ld\n", Tsum);
*/

	res->flag = 0;
	return res;
}


long ProdTensor::DeltaT_removeMatch(vector<int> mi, vector<int> mj, unsigned int i) {
	register unsigned int j, k;

	long tri_count = 0;

	for(j = 0; j < mi.size(); j++) {
		if(i == j)
			continue;
		if(!G->getEdge(mi[i], mi[j]) || !H->getEdge(mj[i], mj[j]) )
			continue;
		for(k = j+1; k < mi.size(); k++) {
			if(i == k)
				continue;
			if( (G->getEdge(mi[i], mi[k]) && H->getEdge(mj[i], mj[k])) &&
				(G->getEdge(mi[j], mi[k]) && H->getEdge(mj[j], mj[k])) ) {
				tri_count ++;
			}
		}
	}

	return tri_count;
}


long ProdTensor::DeltaT_addMatch(vector<int> mi, vector<int> mj, unsigned int i, vector<unsigned int> e) {
	register unsigned int j, k;

	long tri_count = 0;

	for(j = 0; j < mi.size(); j++) {
		if(i == j)
			continue;
		if(!G->getEdge(e[0], mi[j]) || !H->getEdge(e[1], mj[j]) )
			continue;
		for(k = j+1; k < mi.size(); k++) {
			if(i == k)
				continue;
			if( (G->getEdge(e[0], mi[k]) && H->getEdge(e[1], mj[k])) &&
				(G->getEdge(mi[j], mi[k]) && H->getEdge(mj[j], mj[k])) ) {
				tri_count ++;
			}
		}
	}

	return tri_count;
}


long ProdTensor::countTrianglesUnderAlignment(vector<int> mi, vector<int> mj) {
	register unsigned int i, j, k;
	 
	 long tri_count = 0;

	 // First count the triangles that fall exclusively with the current block 
     for(i = 0; i < mi.size(); i++) {
		for(j = i+1; j < mi.size(); j++) {
			if(!G->getEdge(mi[i], mi[j]) || !H->getEdge(mj[i], mj[j]) )
				continue;				
			for(k = j+1; k < mi.size(); k++) {				
				if( (G->getEdge(mi[i], mi[k]) && H->getEdge(mj[i], mj[k])) &&
					(G->getEdge(mi[j], mi[k]) && H->getEdge(mj[j], mj[k])) ) {
					tri_count ++;
				}
			}
		}
	}	
	
	return tri_count;
}

int ProdTensor::InitX(double *x, int init_type, double *w) {
	register unsigned int i, j, cur_row, cur_col;
	printf("InitX:: Initializing x0 with type %d\n", init_type);
	
	bzero(x, n*sizeof(double));	
	printf("\tInitX::Sparsity flag = %d\n", this->sparsity_type);
	switch(init_type) {
		case(Uniform):	{
			double uniform_val = 1 / sqrt(n);
			for(cur_row = 0; cur_row < unmatched_rows.size(); cur_row++) {
				i = unmatched_rows[cur_row];
				for(cur_col = 0; cur_col < unmatched_cols.size(); cur_col++) {		
					j = unmatched_cols[cur_col];
					x[i * n2 + j] = uniform_val;
				}
			}
			break;		
		}	
		case(Random): {  // Warning:: Using random initialization with sparse formulation is highly discouraged.
			fprintf(stderr, "\tInitX: *Warning* Using random initialization with sparse formulation is highly discouraged.\n");
			srand(time(NULL));
			for(cur_row = 0; cur_row < unmatched_rows.size(); cur_row++) {
				i = unmatched_rows[cur_row];
				for(cur_col = 0; cur_col < unmatched_cols.size(); cur_col++) {		
					j = unmatched_cols[cur_col];
					x[i * n2 + j] = (rand() % n);
				}
			}		
			break;		
		}	
		case(SeqSim): {
			for(cur_row = 0; cur_row < unmatched_rows.size(); cur_row++) {
				i = unmatched_rows[cur_row];
				for(cur_col = 0; cur_col < unmatched_cols.size(); cur_col++) {		
					j = unmatched_cols[cur_col];
					x[i * n2 + j] = w[i*n2 + j];
				}
			}		
			break;		
		}
		default:
			fprintf(stderr, "InitX:: Unknown initialization type: %d\n", init_type);
			return -1;
	}
	normalize(x, x, n);

		
	return 0;
}

// Initializes the bipartite graph between vertices in G and H over the set of unmatched rows/cols
unsigned int ProdTensor::setupBipartiteGraph(double *x_in) {
	register unsigned int i, j, cur_row, cur_col;
	unsigned int num_edges = 0;
	if( this->sparsity_type == NoSparsity ) {
		num_edges = unmatched_rows.size() * unmatched_cols.size();

		// <i, j>: original vertex<row, col> index in G and H, <cur_row, cur_col>: Index in the unmatched submatrix
		for(cur_row = 0; cur_row < unmatched_rows.size(); cur_row++) {
			i = unmatched_rows[cur_row];
			for(cur_col = 0; cur_col < unmatched_cols.size(); cur_col++) {
				j = unmatched_cols[cur_col];
				rows[cur_row*unmatched_cols.size()+ cur_col] = i;
				cols[cur_row*unmatched_cols.size()+ cur_col] = j;
				edge_weights[cur_row*unmatched_cols.size()+ cur_col] = x_in[i*n2+j];
			}
		}
	}
	else {
		register unsigned int i;
		num_edges = unmatched_pairs_row.size();
		for (i = 0; i < num_edges; i++) {
			rows[i] = unmatched_pairs_row[i];
			cols[i] = unmatched_pairs_col[i];
			edge_weights[i] = x_in[unmatched_pairs_row[i]*n2 + unmatched_pairs_col[i]];
		}
	}

	return num_edges;
}

ProdTensor::~ProdTensor() {   
	
	free(G_isMatched);
	free(H_isMatched);		
	G_isMatched = H_isMatched = NULL;
				
	free(rows);
	free(cols);
	free(edge_weights);
	free(mi);
	free(mj);
	rows = cols = edge_weights = mi = mj = NULL;
}
					