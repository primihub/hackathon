
#pragma once
#include "BNConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;


class BNLayer : public Layer
{
private:
	BNConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	MEVectorType gamma;
	MEVectorType beta;
	MEVectorType xhat;
	MEVectorType sigma;

public:
	//Constructor and initializer
	BNLayer(BNConfig* conf, int _layerNum);
	void initialize();

	//Functions
	void printLayer() override;
	void forward(const MEVectorType& inputActivation) override;
	void computeDelta(MEVectorType& prevDelta) override;
	void updateEquations(const MEVectorType& prevActivations) override;

	//Getters
	MEVectorType* getActivation() {return &activations;};
	MEVectorType* getDelta() {return &deltas;};
};