
#pragma once
#include "CNNConfig.h"
#include "Layer.h"
#include "tools.h"
#include "connect.h"
#include "globals.h"
using namespace std;


class CNNLayer : public Layer
{
private:
	CNNConfig conf;
	MEVectorType activations;
	MEVectorType deltas;
	MEVectorType weights;
	MEVectorType biases;

public:
	//Constructor and initializer
	CNNLayer(CNNConfig* conf, int _layerNum);
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