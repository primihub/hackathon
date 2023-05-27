
#pragma once
#include "globals.h"

class Layer
{
public: 
	int layerNum = 0;
	Layer(int _layerNum): layerNum(_layerNum) {};

//Virtual functions	
	virtual void printLayer() {};
	virtual void forward(const MEVectorType& inputActivation) {};
	virtual void computeDelta(MEVectorType& prevDelta) {};
	virtual void updateEquations(const MEVectorType& prevActivations) {};

//Getters
	virtual MEVectorType* getActivation() {};
	virtual MEVectorType* getDelta() {};
};