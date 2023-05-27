
#pragma once
#include "ReLUConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;

extern int partyNum;


class ReLULayer : public Layer
{
private:
	ReLUConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	MEVectorSmallType reluPrime;

public:
	//Constructor and initializer
	ReLULayer(ReLUConfig* conf, int _layerNum);

	//Functions
	void printLayer() override;
	void forward(const MEVectorType& inputActivation) override;
	void computeDelta(MEVectorType& prevDelta) override;
	void updateEquations(const MEVectorType& prevActivations) override;

	//Getters
	MEVectorType* getActivation() {return &activations;};
	MEVectorType* getDelta() {return &deltas;};
};