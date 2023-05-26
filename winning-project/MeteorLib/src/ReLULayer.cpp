
#pragma once
#include "ReLULayer.h"
#include "Functionalities.h"
using namespace std;

ReLULayer::ReLULayer(ReLUConfig* conf, int _layerNum)
:Layer(_layerNum),
 conf(conf->batchSize, conf->inputDim),
 activations(conf->batchSize * conf->inputDim), 
 deltas(conf->batchSize * conf->inputDim),
 reluPrime(conf->batchSize * conf->inputDim)
{}


void ReLULayer::printLayer()
{
	cout << "----------------------------------------------" << endl;  	
	cout << "(" << layerNum+1 << ") ReLU Layer\t\t  " << conf.batchSize << " x " << conf.inputDim << endl;
}


void ReLULayer::forward(const MEVectorType &inputActivation)
{
	log_print("ReLU.forward");

	size_t rows = conf.batchSize;
	size_t columns = conf.inputDim;
	size_t size = rows*columns;

	if (FUNCTION_TIME)
		cout << "Meteor_funcRELU: " << funcTime(Meteor_funcRELU, inputActivation, reluPrime, activations, size) << " milliseconds" << endl;
	else
		Meteor_funcRELU(inputActivation, reluPrime, activations, size);
	//pause_communication();
	//end_communication("ReLU" + to_string(layerNum));
	//resume_communication();

}


void ReLULayer::computeDelta(MEVectorType& prevDelta)
{
	log_print("ReLU.computeDelta");
/*
	//Back Propagate	
	size_t rows = conf.batchSize;
	size_t columns = conf.inputDim;
	size_t size = rows*columns;

	if (FUNCTION_TIME)
		cout << "funcSelectShares: " << funcTime(funcSelectShares, deltas, reluPrime, prevDelta, size) << endl;
	else
		funcSelectShares(deltas, reluPrime, prevDelta, size);
*/
}


void ReLULayer::updateEquations(const MEVectorType& prevActivations)
{
	log_print("ReLU.updateEquations");
}
