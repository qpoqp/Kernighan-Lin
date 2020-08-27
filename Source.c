//#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cvector.h"
#include "Graph.h"
//a ∈ A и b ∈ B
//Дефицит вершины = (сумма весов ребер между 'a' и другой вершиной из B) - (сумма весов ребер между 'a' и другой вершиной из A)
void def_calc(struct Graph* G) {
	float def = 0;
	for (int i = 0; i < G->V; i++) {
		def = 0;
		for (int j = 0; j < G->V; j++) {
			float weight = *(G->Matrix + G->defs[i]->index * G->V + G->defs[j]->index);
			if (weight) {//Если существует ребро между вершинами i j
				if (G->defs[i]->subset == G->defs[j]->subset)
					def -= weight;
				else
					def += weight;
			}
		}
		G->defs[i]->def = def;
	}
}
void select_pair(struct Graph* G) {
	int _a = -1; int _b = -1; float m_gain = INT_MIN;
	for (int i = 0; i < cvector_size(G->defs); i++) {
		for (int j = 0; j < cvector_size(G->defs); j++) {
			if (G->defs[i]->subset != G->defs[j]->subset) {//Если вершины из разных подмножеств
				float c_gain = G->defs[i]->def + G->defs[j]->def - 2 * (*(G->Matrix + G->defs[i]->index * G->V + G->defs[j]->index));//текущая выгода
				if (c_gain > m_gain) {
					_a = i;
					_b = j;
					m_gain = c_gain;
				}
			}
		}
	}
	if (cvector_size(G->A) > 0) {//Если есть предыдущие выгоды
		m_gain += G->A[cvector_size(G->A) - 1]->gain;
	}
	G->defs[_a]->gain = G->defs[_b]->gain = m_gain;
	if (G->defs[_a]->subset == first) {
		cvector_push_back(G->A, G->defs[_a]);
		cvector_push_back(G->B, G->defs[_b]);
	}
	else {
		cvector_push_back(G->A, G->defs[_b]);
		cvector_push_back(G->B, G->defs[_a]);
	}
	cvector_erase(G->defs, _a);
	if (_a < _b)//Если a находится раньше, b смещается на 1
		_b--;
	cvector_erase(G->defs, _b);

	//Пересчет дефицитов
	for (int i = 0; i < cvector_size(G->defs); i++) {
		float weight_ia = *(G->Matrix + G->defs[i]->index * G->V + G->A[cvector_size(G->A) - 1]->index);//Вес между i и последней вершиной из  A
		float weight_ib = *(G->Matrix + G->defs[i]->index * G->V + G->B[cvector_size(G->B) - 1]->index);//Вес между i и последней вершиной из  B
		if (G->defs[i]->subset == first) {
			G->defs[i]->def += 2 * weight_ia - 2 * weight_ib;
		}
		else {
			G->defs[i]->def += 2 * weight_ib - 2 * weight_ia;
		}
	}
}
int main(int argc, char* argv[]) {//1) filename with graph
	struct Graph G;
	srand(time(NULL));
	graph_init(&G, argv);
	random_cut(&G, 0);
	float g_max;
	int k;
	int count = 0;
	do {
		g_max = 0;
		k = -1;
		def_calc(&G);
		for (int i = 0; i < G.V / 2; i++) {
			select_pair(&G);
		}
		for (int i = 0; i < G.V / 2; i++) {
			if (G.A[i]->gain > g_max) {
				g_max = G.A[i]->gain;
				k = i;
			}
		}
		if ((int)g_max > 0) {
			while (!cvector_empty(G.A)) {
				if (k >= 0) {
					G.A[0]->subset = second;
					G.B[0]->subset = first;
					k--;
				}
				G.A[0]->def = G.B[0]->def = INT_MIN;
				G.A[0]->gain = G.B[0]->gain = INT_MIN;
				cvector_push_back(G.defs, G.A[0]);
				cvector_push_back(G.defs, G.B[0]);
				cvector_erase(G.A, 0);
				cvector_erase(G.B, 0);
			}
			float a = 0; float b = 0; float deleted = 0;
			for (int i = 0; i < G.V; i++) {
				for (int j = 0; j < G.V; j++) {
					if (G.defs[i]->subset == G.defs[j]->subset)
						if (G.defs[i]->subset == first)
							a += *(G.Matrix + G.defs[i]->index * G.V + G.defs[j]->index);
						else
							b += *(G.Matrix + G.defs[i]->index * G.V + G.defs[j]->index);
					else
						deleted += b += *(G.Matrix + G.defs[i]->index * G.V + G.defs[j]->index);
				}
			}
			printf("\n%d)Gain:%f\tA:%.2f\tB:%.2f\tD:%.2f", count, g_max, a, b, a - b);
		}
		
	} while ((int)g_max > 0);
	cvector_free(G.defs);
	G.defs = NULL;

	return 0;
}