#pragma once
#include "AvepoolConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;


class AvepoolLayer : public Layer
{
private:
	AvepoolConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	//RSSVectorSmallType maxPrime;

public:
	//Constructor and initializer
	AvepoolLayer(AvepoolConfig* conf, int _layerNum);

	//Functions
	void printLayer() override;
	void forward(const MEVectorType& inputActivation) override;
	void computeDelta(MEVectorType& prevDelta) override;
	void updateEquations(const MEVectorType& prevActivations) override;

	//Getters
	MEVectorType* getActivation() {return &activations;};
	MEVectorType* getDelta() {return &deltas;};
};