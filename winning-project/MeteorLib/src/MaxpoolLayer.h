
#pragma once
#include "MaxpoolConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;


class MaxpoolLayer : public Layer
{
private:
	MaxpoolConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	//MEVectorSmallType maxPrime;

public:
	//Constructor and initializer
	MaxpoolLayer(MaxpoolConfig* conf, int _layerNum);

	//Functions
	void printLayer() override;
	void forward(const MEVectorType& inputActivation) override;
	void computeDelta(MEVectorType& prevDelta) override;
	void updateEquations(const MEVectorType& prevActivations) override;

	//Getters
	MEVectorType* getActivation() {return &activations;};
	MEVectorType* getDelta() {return &deltas;};
};