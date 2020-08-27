#pragma once
#include "cvector.h"

#define first 0x41
#define second 0x42
typedef struct Vertex {
	int index;
	char subset;
	float def;
	float gain;
} Vertex;
struct Graph {
	int V, E;
	float* Matrix;
	cvector_vector_type(Vertex*) defs;
	cvector_vector_type(Vertex*) A;
	cvector_vector_type(Vertex*) B;
};
void test_init(struct Graph* G) {
	G->defs = NULL;
	G->A = NULL;
	G->B = NULL;
	G->V = 8;
	G->E = 13;
	G->Matrix = (float*)malloc(G->V * G->V * sizeof(float));
	for (int i = 0; i < G->V; i++) {
		for (int j = 0; j < G->V; j++) {
			*(G->Matrix + i * G->V + j) = 0;
		}
	}
	*(G->Matrix + 0 * G->V + 1) = 1;
	*(G->Matrix + 0 * G->V + 4) = 1;
	*(G->Matrix + 0 * G->V + 5) = 1;

	*(G->Matrix + 1 * G->V + 0) = 1;
	*(G->Matrix + 1 * G->V + 4) = 1;
	*(G->Matrix + 1 * G->V + 5) = 1;

	*(G->Matrix + 2 * G->V + 3) = 1;
	*(G->Matrix + 2 * G->V + 5) = 1;
	*(G->Matrix + 2 * G->V + 6) = 1;
	*(G->Matrix + 2 * G->V + 7) = 1;

	*(G->Matrix + 3 * G->V + 2) = 1;
	*(G->Matrix + 3 * G->V + 6) = 1;
	*(G->Matrix + 3 * G->V + 7) = 1;

	*(G->Matrix + 4 * G->V + 0) = 1;
	*(G->Matrix + 4 * G->V + 1) = 1;
	*(G->Matrix + 4 * G->V + 5) = 1;

	*(G->Matrix + 5 * G->V + 0) = 1;
	*(G->Matrix + 5 * G->V + 1) = 1;
	*(G->Matrix + 5 * G->V + 4) = 1;
	*(G->Matrix + 5 * G->V + 2) = 1;

	*(G->Matrix + 6 * G->V + 2) = 1;
	*(G->Matrix + 6 * G->V + 3) = 1;
	*(G->Matrix + 6 * G->V + 7) = 1;

	*(G->Matrix + 7 * G->V + 2) = 1;
	*(G->Matrix + 7 * G->V + 3) = 1;
	*(G->Matrix + 7 * G->V + 6) = 1;


}
void graph_init(struct Graph* G, char** argv) {
	G->defs = NULL;
	G->A = NULL;
	G->B = NULL;
	unsigned char buffer[4];
	FILE* connection = fopen(argv[1], "rb");
	for (int i = 0; i < 4; i++)
		buffer[i] = getc(connection);
	G->V = *(int*)& buffer;
	for (int i = 0; i < 4; i++)
		buffer[i] = getc(connection);
	G->E = *(int*)& buffer;
	G->Matrix = (float*)malloc(G->V * G->V * sizeof(float));
	for (int i = 0; i < G->V; i++) {
		for (int j = 0; j < G->V; j++) {
			for (int i = 0; i < 4; i++) {
				buffer[i] = getc(connection);
			}
			*(G->Matrix + i * G->V + j) = *(float*)& buffer;
		}
	}
	fclose(connection);
}
void print_graph(struct Graph* G) {
	float edges_sum = 0;
	for (int i = 0; i < G->V; i++) {
		for (int j = 0; j < G->V; j++) {
			printf("%.2f ", *(G->Matrix + j * G->V + i));
			edges_sum += *(G->Matrix + j * G->V + i);
		}	
		printf("\n");
	}
	printf("\nEdges:%d\tsum%.2f\n",G->E, edges_sum);
	system("pause");
}
void random_cut(struct Graph* G, int thread) {//Случайное разделение необходимо для разных потоков

	for (int i = 0; i < G->V; i+=2) {
		Vertex* v = (Vertex*)malloc(sizeof(Vertex));
		Vertex* v2 = (Vertex*)malloc(sizeof(Vertex));
		v->index = i;
		v2->index = i+1;
		if (rand() % 100 < 50) {
			v->subset = first;
			v2->subset = second;
		}
		else {
			v->subset = second;
			v2->subset = first;
		}
		cvector_push_back(G->defs, v);
		cvector_push_back(G->defs, v2);
	}
	//int half = G->V / 2;
	//for (int i = 0; i < G->V; i++) {
	//	Vertex* v = (Vertex*)malloc(sizeof(Vertex));
	//	v->index = i;
	//	v->subset = (i < half) ? first : second;
	//	cvector_push_back(G->defs, v);
	//}
	//if (thread < half * half) {
	//	char t = G->defs[thread / half]->subset;
	//	G->defs[thread / half]->subset = G->defs[half + thread % half]->subset;
	//	G->defs[half + thread % half]->subset = t;
	//}
}
void print_defs(struct Graph* G) {
	printf("Defs:\n");
	for (int i = 0; i < G->V; i++) {
		printf("%d\t%f\n", G->defs[i]->index, G->defs[i]->def);
	}
	getchar();
	
}