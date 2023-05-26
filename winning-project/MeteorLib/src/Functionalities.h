
#pragma once
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;

extern void start_time();
extern void start_communication();
extern void end_time(string str);
extern void end_communication(string str);



void funcReconstructBit(const RSSVectorSmallType &a, vector<smallType> &b, size_t size, string str, bool print);
void funcReconstruct(const RSSVectorMyType &a, vector<myType> &b, size_t size, string str, bool print);
void funcReconstruct(const RSSVectorSmallType &a, vector<smallType> &b, size_t size, string str, bool print);

void Meteor_funcMatMul(const MEVectorType &a, const MEVectorType &b, MEVectorType &c, 
					size_t rows, size_t common_dim, size_t columns,
				 	size_t transpose_a, size_t transpose_b, size_t truncation);
void Meteor_funcDotProduct(const MEVectorType &a, const MEVectorType &b, MEVectorType &c, size_t size, bool truncation, size_t precision);

void Meteor_funcDotProduct(const MEVectorSmallType &a, const MEVectorSmallType &b, MEVectorSmallType &c, size_t size);

void Meteor_funcDotProductBits(const MEVectorSmallType &a, const MEVectorSmallType &b, MEVectorSmallType &c, size_t size);
void Meteor_funcMultiplyNeighbors(const MEVectorSmallType &c_1, MEVectorSmallType &c_2, size_t size);
void Meteor_funcCrunchMultiply(const MEVectorSmallType &c, vector<smallType> &betaPrime, size_t size);

void Meteor_funcPrivateCompare(const MEVectorSmallType &share_m, const vector<myType> &r, const MEVectorSmallType &beta, vector<smallType> &betaPrime, size_t size);

void Meteor_funcRELUPrime(const MEVectorType &a, MEVectorSmallType &b, size_t size);

void Meteor_funcRELU(const MEVectorType &a, MEVectorSmallType & temp, MEVectorType &b, size_t size);

void Meteor_funcMaxpool(const MEVectorType &a, MEVectorType &max, size_t rows, size_t columns);

void aggregateCommunication();

void Meteor_funcAvepool(MEVectorType &a, MEVectorType &max, size_t rows, size_t columns);		
//funcAveBackShares		
void Meteor_funcAveBackShares(const MEVectorType &a, MEVectorType &selected, size_t size);



//Test
void testMeteorMatMul(size_t rows, size_t common_dim, size_t columns, size_t iter);
void testMeteorDotProduct(size_t size, size_t iter);
void testMeteorSmallDotProduct(size_t size, size_t iter);
void testMeteorBitProduct(size_t size, size_t iter);
void testMeteorNeighborMultly(size_t size, size_t iter);
void testMeteorfuncCrunchMultiply(size_t size, size_t iter);
void testMeteorPC(size_t size, size_t iter);
void testMeteorRELUPrime(size_t size, size_t iter);
void testMeteorRelu(size_t size, size_t iter);
void testMeteorMaxpool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter);
void testMeteorPredictAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter);
void testMeteorConvolution(size_t iw, size_t ih, size_t Din, size_t Dout, size_t f, size_t S, size_t P, size_t B, size_t iter);
void test_MeteorBatchNorm(size_t numBatches, size_t inputSize, size_t iter);
void testThunderNMult(size_t size, size_t N,  size_t iter);
void testMeteorTrainAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter);
void testMeteorBackAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter);