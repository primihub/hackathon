
#pragma once
#include "FCConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;

extern int partyNum;


class FCLayer : public Layer
{
private:
	FCConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	MEVectorType weights;
	MEVectorType biases;


public:
	//Constructor and initializer
	FCLayer(FCConfig* conf, int _layerNum);
	void initialize();

	//Functions
	void printLayer() override;
	void forward(const MEVectorType& inputActivation) override;
	void computeDelta(MEVectorType& prevDelta) override;
	void updateEquations(const MEVectorType& prevActivations) override;

	//Getters
	MEVectorType* getActivation() {return &activations;};
	MEVectorType* getDelta() {return &deltas;};
	MEVectorType* getWeights() {return &weights;};
	MEVectorType* getBias() {return &biases;};
};