
#include "Functionalities.h"
#include "secondary.h"


void runTest(string str, string whichTest, string &network)
{
	if (str.compare("Debug") == 0)
	{
		cout << "debug deleted" << endl;
	}
	else if (str.compare("Test") == 0)
	{
		
		if(whichTest.compare("MeteorRELU") == 0){
			network = "Test Meteor RELU";
			testMeteorRelu(1024, 10);
		}
		else if(whichTest.compare("MeteorRELUPrime") == 0){
			network = "Test Meteor RELUPrime";
			testMeteorRELUPrime(20000, 2);
		}
		else if(whichTest.compare("MeteorPC") == 0){
			network = "Test Meteor PC";
			testMeteorPC(10, 3);
		}
		else if(whichTest.compare("BitProduct")==0){
			network = "Test BitProduct";
			testMeteorBitProduct(10, NUM_ITERATIONS);
		}	
		else if (whichTest.compare("MeteorDotProduct") == 0){
			network = "Test Meteor DotProduct";
			testMeteorDotProduct(10, 3);
		}			
		else if(whichTest.compare("MeteorMaxpool") == 0){
			network = "Test Meteor Maxpool";
			testMeteorMaxpool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		}
		else if(whichTest.compare("MeteorPredictAvepool") == 0){
			network = "Test Meteor PredictAvepool";
			testMeteorPredictAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		}
		// else if(whichTest.compare("MeteorConv") == 0){
		// 	network = "Test Meteor Convolution";
		// 	testMeteorTrainAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		// }
		else if(whichTest.compare("MeteorMatMul") == 0){
			network = "Test Meteor MatMul";
			testMeteorMatMul(784, 128, 10, NUM_ITERATIONS);
		}
		else if(whichTest.compare("MeteorTrainAvepool") == 0){
			network = "Test Meteor TrainAvepool";
			testMeteorTrainAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		}
		else if(whichTest.compare("MeteorTrainAvepool") == 0){
			network = "Test Meteor TrainAvepool";
			testMeteorTrainAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		}
		// else if(whichTest.compare("MeteorMaxpool") == 0){
		// 	network = "Test Meteor Maxpool";
		// 	testMeteorMaxpool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
		// }
		else if (whichTest.compare("Conv") == 0)
		{
			network = "Test Meteor Conv";
			testMeteorConvolution(28, 28, 1, 20, 3, 1, 0, MINI_BATCH_SIZE, NUM_ITERATIONS);
		}
		else if (whichTest.compare("BN") == 0)
		{
			network = "Test Meteor BN";
			test_MeteorBatchNorm(MINI_BATCH_SIZE, 784, NUM_ITERATIONS);
		}
		else
			assert(false && "Unknown test mode selected");
	}
	else
		assert(false && "Only Debug or Test mode supported");
}
void runTestAll(string str, string whichTest, string &network)
{
	for(int i=1;i<10;i++){
		start_m();
		if (str.compare("TestInference") == 0)
		{
			cout << "debug deleted" << endl;
		}
		else if (str.compare("Test") == 0)
		{
		
			if(i == 1){
				
				network = "Test Meteor RELU";
				testMeteorRelu(1024, 10);
			}
			else if(i == 2){
				network = "Test Meteor RELUPrime";
				testMeteorRELUPrime(20000, 2);
			}
			else if(i == 3){
				network = "Test Meteor PC";
				testMeteorPC(10, 3);
			}
			else if(whichTest.compare("BitProduct")==0){
				network = "Test BitProduct";
				testMeteorBitProduct(10, NUM_ITERATIONS);
			}	
			else if (i == 4){
				network = "Test Meteor DotProduct";
				testMeteorDotProduct(10, 3);
			}			
			else if(i == 5){
				network = "Test Meteor Maxpool";
				testMeteorMaxpool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
			}
			else if(whichTest.compare("MeteorPredictAvepool") == 0){
				network = "Test Meteor PredictAvepool";
				testMeteorPredictAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
			}
			else if(i == 6){
				network = "Test Meteor MatMul";
				testMeteorMatMul(784, 128, 10, NUM_ITERATIONS);
			}
			else if(i == 7){
				network = "Test Meteor TrainAvepool";
				testMeteorTrainAvepool(24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);
			}
			else if (i == 8)
			{
				network = "Test Meteor Conv";
				testMeteorConvolution(28, 28, 1, 20, 3, 1, 0, MINI_BATCH_SIZE, NUM_ITERATIONS);
			}
			else if (i == 9)
			{
				network = "Test Meteor BN";
				test_MeteorBatchNorm(MINI_BATCH_SIZE, 784, NUM_ITERATIONS);
			}
			else
				assert(false && "Unknown test mode selected");
		}
		end_m(network);
	}
}