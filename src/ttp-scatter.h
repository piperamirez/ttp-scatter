#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <math.h>

#define POPSIZE             50
#define b                   10
#define MaxEval         500000
#define LSITERATIONS        20 

using namespace std;

class Solution {
	vector< vector<int> > S;
	int fitness;
public:
	void setValue(vector< vector<int> >);
	void evaluate(vector< vector<int> >,unsigned long &);
	vector< vector<int> > getValue() {return S;}
	int isFeasible();
	int getFitness() {return fitness;}
};
	
void Solution::setValue(vector< vector<int> > solution){
	S = solution;
}

void Solution::evaluate(vector< vector<int> > D,unsigned long &nEval){
	int i, j;
	fitness = 0;
	for (i=0; i<(int)S.size(); i++) {
		if(S[i][0]<0) fitness += D[i+1-1][abs(S[i][0])-1];
		for (j=0; j<(int)S[i].size()-1; j++) {
			if (S[i][j]>0 && S[i][j+1]>0) fitness += 0;
			if (S[i][j]<0 && S[i][j+1]>0) fitness += D[i+1-1][abs(S[i][j])-1];
			if (S[i][j]>0 && S[i][j+1]<0) fitness += D[i+1-1][abs(S[i][j+1])-1];
			if (S[i][j]<0 && S[i][j+1]<0) fitness += D[abs(S[i][j])-1][abs(S[i][j+1])-1];
		}
		if(S[i][j]<0) fitness += D[abs(S[i][j])-1][i+1-1];
	}
	
	nEval++;
}

int Solution::isFeasible(){
	int i,j;
	for (i=0; i<(int)S.size(); i++) {
		for (j=0; j<(int)S[i].size()-1 ; j++) {
			if (j<(int)S[i].size()-3) {
				if (S[i][j]>0&&S[i][j+1]>0&&S[i][j+2]>0&&S[i][j+3]>0) return 0;
				if (S[i][j]<0&&S[i][j+1]<0&&S[i][j+2]<0&&S[i][j+3]<0) return 0;
			}
			if (abs(S[i][j]) == abs(S[i][j+1])) return 0;
		}
	}
	return 1;
}

Solution  decode               (vector<int>);
void      printMatrix          (vector< vector<int> >);
int       levenshteinDistance  (vector< vector<int> >, vector< vector<int> >);
int       scheduleDistance     (Solution, vector< vector<int> >, unsigned long &);
Solution  localSearch          (Solution, vector< vector<int> >, unsigned long &);
Solution  crossOver            (Solution, Solution, vector< vector<int> >, unsigned long &);
Solution  swapTeams            (Solution, int, int);
Solution  swapHomeAwayAt       (Solution, int, int);
Solution  swapRounds           (Solution, int, int);