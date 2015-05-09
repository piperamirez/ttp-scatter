#include "ttp-scatter.h"

int main (int argc, char *argv[]) {
	FILE * instanceFile;
	instanceFile = fopen(argv[1],"r");
	int p;
	vector<int> row;
	while(!feof(instanceFile)) {
		fscanf(instanceFile, "%d", &p);
		row.push_back(p);
	}
	fclose(instanceFile);
	int N = sqrt((int)row.size()-1);
	
	int i, j;
	vector< vector<int> > D(N, vector<int>(N));
	instanceFile = fopen(argv[1],"r");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			fscanf(instanceFile, "%d", &D[i][j]);
		}
	}
	fclose(instanceFile);
	
	unsigned long nEval = 0;
	
	srand(time(NULL));
	vector<Solution> P(POPSIZE);
	Solution BestSolution;
	for (i = 0; i < POPSIZE; i++) {
		vector<int> initialCod(N,0);
		for (j = 0; j < N; j++) {
			initialCod[j] = j + 1;
		}
		random_shuffle(initialCod.begin(),initialCod.end());
		P[i] = decode(initialCod);
		P[i].evaluate(D,nEval);
	}
	
	for (i = 0; i < POPSIZE; i++) {
		P[i] = localSearch(P[i], D, nEval);
	}
	
	vector<Solution> RefSet;
	for (i = 0; i < b / 2; i++) {
		int bestFitness = INT_MAX;
		int best = 0;
		for (j = 0; j < (int)P.size(); j++) {
			if (bestFitness > P[j].getFitness()) {
				bestFitness = P[j].getFitness();
				best = j;
			}
		}
		RefSet.push_back(P[best]);
		P.erase(P.begin()+best);
	}
	int k;

	vector<int> hist((int)P.size(),0);
	for (k = 0; k < b / 2; k++) {
		for (i = 0; i < (int)RefSet.size(); i++) {
			int maxdist = INT_MAX;
			int nearest = 0;
			for (j = 0; j < (int)P.size(); j++) {
				int auxd = levenshteinDistance(RefSet[i].getValue(), P[j].getValue());
				if (maxdist > auxd) {
					nearest = j;
					maxdist = auxd;
				}
			}
			hist[nearest]++;
		}
		int ind = 0;
		int max = 0;
		for (i = 0; i < (int)hist.size(); i++) {
			if (hist[i] > max) {
				ind = i;
				max = hist[i];
			}
			hist[i] = 0;
		}
		RefSet.push_back(P[ind]);
		P.erase(P.begin() + ind);
	}
	
	int bestFitness = INT_MAX;
	int bestind = 0;
	for (i = 0; i < (int)RefSet.size(); i++) {
		if (RefSet[i].getFitness() < bestFitness) {
			bestFitness = RefSet[i].getFitness();
			bestind = i;
		}
	}
	BestSolution = RefSet[bestind];

	vector<Solution> Offspring;
	vector<Solution> TempRefSet;
	do {
		int newSolutionsAdded = 0;
		do {
			for (i = 0; i < (int)RefSet.size() / 2; i++) {
				for (j = i; j < (int)RefSet.size(); j++) {
					Offspring.push_back(crossOver(RefSet[i], RefSet[j], D, nEval));
				}
			}
			
			for (i = 0; i < (int)Offspring.size(); i++) {
				for (j = 0; j < LSITERATIONS; j++)
					Offspring[i] = localSearch(Offspring[i], D, nEval);
			}
			
			newSolutionsAdded = 0;
			for (i = 0; i < b; i++) {
				int bestFitness = INT_MAX;
				int bestind = 0;
				int fromRefSet = 1;
				for (j = 0; j < (int)RefSet.size(); j++) {
					if (RefSet[j].getFitness() < bestFitness) {
						bestFitness = RefSet[j].getFitness();
						bestind = j;
					}
				}
				for (j = 0; j < (int)Offspring.size(); j++) {
					if (Offspring[j].getFitness() < bestFitness) {
						bestFitness = Offspring[j].getFitness();
						bestind = j;
						newSolutionsAdded = 1;
						fromRefSet = 0;
					}
				}
				if (fromRefSet) {
					TempRefSet.push_back(RefSet[bestind]);
					RefSet.erase(RefSet.begin() + bestind);
				}
				if (!fromRefSet) {
					TempRefSet.push_back(Offspring[bestind]);
					Offspring.erase(Offspring.begin() + bestind);
				}
			}
			RefSet = TempRefSet;
			Offspring.clear();
			TempRefSet.clear();
		} while (newSolutionsAdded && nEval < MaxEval);
		
		bestFitness = INT_MAX;
		bestind = 0;
		for (i = 0; i < (int)RefSet.size(); i++) {
			if (RefSet[i].getFitness() < bestFitness) {
				bestFitness = RefSet[i].getFitness();
				bestind = i;
			}
		}
		BestSolution = RefSet[bestind];
		
		for (i = 0; i < b / 2; i++) {
			int worstFitness = 0;
			int worstind = 0;
			for (j = 0; j < (int)RefSet.size(); j++) {
				if (RefSet[j].getFitness() > worstFitness) {
					worstFitness = RefSet[j].getFitness();
					worstind = j;
				}
			}
			RefSet.erase(RefSet.begin() + worstind);
		}
		vector<Solution> Q(POPSIZE);
		for (i = 0; i < POPSIZE; i++) {
			vector<int> initialCod(N, 0);
			for (j = 0; j < N; j++) {
				initialCod[j] = j + 1;
			}
			random_shuffle(initialCod.begin(), initialCod.end());
			Q[i] = decode(initialCod);
			Q[i].evaluate(D,nEval);
		}
		for (i = 0; i < POPSIZE; i++) {
			for (j = 0; j < LSITERATIONS; j++) {
				Q[i] = localSearch(Q[i], D, nEval);
			}
		}
		for (k = 0; k < b / 2; k++) {
			for (i = 0; i < (int)RefSet.size(); i++) {
				int maxdist = INT_MAX;
				int nearest = 0;
				for (j = 0; j < (int)Q.size(); j++) {
					int auxd = levenshteinDistance(RefSet[i].getValue(), Q[j].getValue());
					if (maxdist > auxd) {
						nearest = j;
						maxdist = auxd;
					}
				}
				hist[nearest]++;
			}
			int ind = 0;
			int max = 0;
			for (i = 0; i<(int)hist.size(); i++) {
				if (hist[i] > max) {
					ind = i;
					max = hist[i];
				}
				hist[i] = 0;
			}
			RefSet.push_back(Q[ind]);
			Q.erase(Q.begin()+ind);
		}
	} while (nEval < (long unsigned)MaxEval);
	 
	printf("Final Solution: %d ", BestSolution.getFitness());
	if(BestSolution.isFeasible()) {
		printf("[Feasible]\n");
	}
	
	i = 0;
	char filename[50];
	while (argv[1][i] != '\0') {
		filename[i] = argv[1][i];
		if (argv[1][i] == '.') {
			filename[i+1] = 'o';
			filename[i+2] = 'u';
			filename[i+3] = 't';
			filename[i+4] = '\0';
			argv[1][i+1] = '\0';
		}
		i++;
	}
	FILE * outputFile;
	outputFile = fopen(filename,"w");
	vector< vector<int> > M1;
	M1 = BestSolution.getValue();
	for (i = 0; i < (int)M1.size(); i++) {
		for (j = 0; j < (int)M1[0].size(); j++) {
			fprintf(outputFile, "\t%d", M1[i][j]);
		}
		fprintf(outputFile,"\n");
	}
	fprintf(outputFile, "\nFitness: %d", BestSolution.getFitness());
	fclose(outputFile);
	return 0;
}

Solution crossOver(Solution Sa, Solution Sb, vector< vector<int> > D, unsigned long &nEval) {
	int i;
	vector< vector<int> > A;
	vector< vector<int> > B;
	Solution S;
	A = Sa.getValue();
	B = Sb.getValue();
	vector<int> a_signature((int)A.size());
	vector<int> b_signature((int)B.size());
	vector<int> temp((int)B.size());
	vector<int> c_signature;
	for (i = 0; i < (int)A.size(); i++) {
		a_signature[i] = abs(A[i][0]);
		b_signature[i] = abs(B[i][0]);
	}
	if (Sa.getFitness() > Sb.getFitness()){
		temp = a_signature;
		a_signature = b_signature;
		b_signature = temp;
	}
	vector<int> used((int)a_signature.size(), 0);
	for (i = 0; i < (int)a_signature.size() / 2; i++) {
		c_signature.push_back(a_signature[i]);
		used[a_signature[i]-1] = 1;
	}
	for (i = 0; i < (int)b_signature.size(); i++) {
		if (!used[b_signature[i]-1]) {
			c_signature.push_back(b_signature[i]);
			used[b_signature[i]-1] = 1;
		}
	}
	S = decode(c_signature);
	S.evaluate(D,nEval);
	return S;
}

Solution localSearch(Solution S,vector< vector<int> > D, unsigned long &nEval) {
	int i, j;
	Solution R;
	vector< vector<int> > M;
	M = S.getValue();
	for (i = 0; i < (int)M.size(); i++) {
		for (j = i+1; j < (int)M.size(); j++) {
			int opc = rand() % 3;
			if (opc == 0) {
				R = swapTeams(S,i+1,j+1);
			}
			else if (opc == 1) {
				R = swapHomeAwayAt(S,i,j);
			}
			else if (opc == 2) {
				R = swapRounds(S,i,j);
			}
			R.evaluate(D,nEval);
			if (R.getFitness() < S.getFitness() && R.isFeasible()) {
				return R;
			}
		}
	}
	return S;
}

Solution swapRounds(Solution S, int x, int y) {
	int i, temp;
	vector< vector<int> > M;
	M = S.getValue();
	for (i = 0; i < (int)M.size(); i++) {
		temp = M[i][x];
		M[i][x] = M[i][y];
		M[i][y] = temp;
	}
	S.setValue(M);
	return S;
}

Solution swapTeams(Solution S, int t1, int t2){
	int i;
	int temp;
	vector< vector<int> > M;
	M = S.getValue();
	for (i = 0; i < (int)M[t1-1].size(); i++) {
		if (abs(M[t1-1][i]) != t2 && abs(M[t2-1][i]) != t1) {
			temp = M[t1-1][i];
			M[t1-1][i] = M[t2-1][i];
			M[t2-1][i] = temp;
			temp = abs(M[abs(M[t1-1][i])-1][i]);
			M[abs(M[t1-1][i])-1][i] = abs(M[abs(M[t2-1][i])-1][i]);
			M[abs(M[t2-1][i])-1][i] = temp;
			if (M[t1-1][i] > 0) {
				M[abs(M[t1-1][i])-1][i] *= -1;
			}
			if (M[t2-1][i] > 0) {
				M[abs(M[t2-1][i])-1][i] *= -1;
			}
		}
	}
	S.setValue(M);
	return S;
}

Solution swapHomeAwayAt(Solution S, int x, int y) {
	Solution R;
	vector< vector<int> > M = S.getValue();
	int i;
	for (i = 0; i < (int)M[x].size(); i++) {
		if (M[x][i] == M[x][y]*(-1)) {
			M[x][i] *= -1;
			M[x][y] *= -1;
			M[abs(M[x][i])-1][i] *= -1;
			M[abs(M[x][y])-1][y] *= -1;
			R.setValue(M);
			if (R.isFeasible()) {
				return R;
			}
		}
	}
	return S;
}

Solution decode(vector<int> code) {
	int i, j;
	int t = (int)code.size();
	int r = 2*(t-1);
	Solution solution;
	
	vector< vector<int> > decode(r/2, vector<int>(t)); 
	decode[0] = code;
	for(i = 0; i < r/2; i++) {
		decode[i][0] = code[0];
	}
	code.erase(code.begin());
	for (i = 1; i < r/2; i++) {
		for (j = 1; j < t; j++) {
			decode[i][j] = code[(i+j-1)%(int)code.size()];
		}
	}
	
	do {
		vector< vector<int> > S(t, vector<int>(r));
		int alt = 1;
		for (i = 0; i < r/2; i++) {
			alt *= -1;
			S[decode[i][0]-1][i]     = (-1)* alt * decode[i][1];
			S[decode[i][1]-1][i]     =       alt * decode[i][0];
			S[decode[i][0]-1][i+t-1] =       alt * decode[i][1];
			S[decode[i][1]-1][i+t-1] = (-1)* alt * decode[i][0];
			for (j = 2; j < t/2+1; j++) {
				S[decode[i][j]-1][i]         = (-1) * alt * decode[i][t-j+1];
				S[decode[i][t-j+1]-1][i]     =        alt * decode[i][j];
				S[decode[i][j]-1][i+t-1]     =        alt * decode[i][t-j+1];
				S[decode[i][t-j+1]-1][i+t-1] = (-1) * alt * decode[i][j];
			}
		}
		solution.setValue(S);
	} while(!solution.isFeasible());
	return solution;
}

int levenshteinDistance(vector< vector<int> > M1, vector< vector<int> > M2) {
	int i, j, distance = 0;
	for(i = 0; i < (int)M1.size(); i++) {
		for(j = 0; j < 2*((int)M1.size()-1); j++) {
			if(M1[i][j] != M2[i][j]) {
				distance++;
			}
		}
	}
	return distance;
}

void printMatrix(vector< vector<int> > M1) {
	printf("A %d x %d matrix\n", (int)M1.size(), (int)M1[0].size());
	int i,j;
	for (i = 0; i < (int)M1.size(); i++) {
		for (j = 0; j < (int)M1[0].size(); j++) {
			printf("\t%d",M1[i][j]);
		}
		printf("\n");
	}
	
}