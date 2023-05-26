#include <iostream>
#include <string>
#include "AESObject.h"
#include "Precompute.h"
#include "secondary.h"
#include "connect.h"
#include "NeuralNetConfig.h"
#include "NeuralNetwork.h"
#include "unitTests.h"


int partyNum;
AESObject* aes_indep;
AESObject* aes_next;
AESObject* aes_prev;
Precompute PrecomputeObject;


int main(int argc, char** argv)
{
/****************************** PREPROCESSING ******************************/ 
	parseInputs(argc, argv);
	NeuralNetConfig* config = new  NeuralNetConfig(NUM_ITERATIONS);//A Class For numIterations addLayer checkNetwork.
	string network, dataset, security;
	bool PRELOADING = true;
	NeuralNetwork* net = new NeuralNetwork(config);



/****************************** Test all operator performance and neural network inference performance at once ******************************/ 
/****************************** Test all operator performance and neural network inference performance at once ******************************/ 
/****************************** Test all operator performance and neural network inference performance at once ******************************/ 


	// int i;
	// cout<<"Please input your choose\n. If you choose 1,then we will start the Test of MeteorOperator\n. else we will start the Test of MeteorInference\n";
	// cin>>i;
	// // while(i){
	// 	if(i==1){
	// 		/****************************** SELECT NETWORK ******************************/ 
	// 		if (argc == 9)
	// 		{network = argv[6]; dataset = argv[7]; security = argv[8];}
	// 		else
	// 		{
	// 			// network = "LeNet";
	// 			// dataset = "MNIST";
	// 			// SecureML:MiniONN;Sarda;LeNet
	// 			network = "SecureML";
	// 			dataset = "MNIST";
	// 			//AlexNet;VGG16
	// 			// CIFAR10;ImageNet
	// 			security = "Semi-honest";
	// 		}//Never mind
	// 		selectNetwork(network, dataset, security, config);//design a frame of network.
	// 		config->checkNetwork();//never mind
	// 		net = new NeuralNetwork(config);
	// 		// NeuralNetwork* net = new NeuralNetwork(config);//refer to a config

	// 		/****************************** AES SETUP and SYNC ******************************/ 
	// 		aes_indep = new AESObject(argv[3]);//A new cryp Key
	// 	 	aes_next = new AESObject(argv[4]);
	// 		aes_prev = new AESObject(argv[5]);

	// 		initializeCommunication(argv[2], partyNum);//Three Ip build the port connection
	// 		synchronize(2000000);//build the thread!
	// 		cout << "connected success" << endl;
	// 		runTestAll("Test", "MeteorOperator", network);
	// 		/****************************** CLEAN-UP ******************************/ 
	// 		delete aes_indep;
	// 		delete aes_next;
	// 		delete aes_prev;
	// 		delete config;
	// 		delete net;
	// 		deleteObjects();
	// 		}
	// 	else{
	// 			for(int j=1;j<5;j++){
	// 				if(j==1){
	// 					network = "SecureML";
	// 					dataset = "MNIST";
	// 					security = "Semi-honest";
	// 				}else if(j==2){
	// 					network = "MiniONN";
	// 					dataset = "MNIST";
	// 					security = "Semi-honest";
	// 				}else if(j==3){
	// 					network = "Sarda";
	// 					dataset = "MNIST";
	// 					security = "Semi-honest";
	// 				}else if(j==4){
	// 					network = "LeNet";
	// 					dataset = "MNIST";
	// 					security = "Semi-honest";
	// 				}else if(j==5){
	// 					network = "AlexNet";
	// 					dataset = "CIFAR10";
	// 					security = "Semi-honest";
	// 				}else if(j==6){
	// 					network = "AlexNet";
	// 					dataset = "ImageNet";
	// 					security = "Semi-honest";
	// 				}else if(j==7){
	// 					network = "VGG16";
	// 					dataset = "CIFAR10";
	// 					security = "Semi-honest";
	// 				}else if(j==8){
	// 					network = "VGG16";
	// 					dataset = "ImageNet";
	// 					security = "Semi-honest";
	// 				}
	// 				selectNetwork(network, dataset, security, config);//design a frame of network.
	// 				config->checkNetwork();//never mind
	// 				NeuralNetwork* net = new NeuralNetwork(config);//refer to a config

	// 				/****************************** AES SETUP and SYNC ******************************/ 
	// 				aes_indep = new AESObject(argv[3]);//A new cryp Key
	// 				aes_next = new AESObject(argv[4]);
	// 				aes_prev = new AESObject(argv[5]);

	// 				initializeCommunication(argv[2], partyNum);//Three Ip build the port connection
	// 				synchronize(2000000);//build the thread!
	// 				cout << "connected success" << endl;
	// 				start_m();
	// 				network += " test";
	// 	 			test(PRELOADING, network, net);
	// 				end_m(network);
	// 				sleep(2);
	// 			}
	// 		}

	// // }

/************************************************************************************************************************************************/ 
/************************************************************************************************************************************************/ 
/************************************************************************************************************************************************/ 



/****************************** Test each operator performance and neural network inference performance separately ******************************/ 
/****************************** Test each operator performance and neural network inference performance separately ******************************/
/****************************** Test each operator performance and neural network inference performance separately ******************************/  



	/****************************** SELECT NETWORK ******************************/ 
	if (argc == 9)
	{network = argv[6]; dataset = argv[7]; security = argv[8];}
	else
	{
		// network = "LeNet";
		// dataset = "MNIST";
		// SecureML:MiniONN;Sarda;LeNet
		network = "AlexNet";
		dataset = "CIFAR10";
		//AlexNet;VGG16
		// CIFAR10;ImageNet
		security = "Semi-honest";
	}//Never mind
	selectNetwork(network, dataset, security, config);//design a frame of network.
	config->checkNetwork();//never mind
	net = new NeuralNetwork(config);//refer to a config

/****************************** AES SETUP and SYNC ******************************/ 
	aes_indep = new AESObject(argv[3]);//A new cryp Key
	aes_next = new AESObject(argv[4]);
	aes_prev = new AESObject(argv[5]);

	initializeCommunication(argv[2], partyNum);//Three Ip build the port connection
	synchronize(2000000);//build the thread!
	cout << "connected success" << endl;
	

/****************************** RUN NETWORK/UNIT TESTS ******************************/ 
	//Run these if you want a preloaded network to be tested
	//assert(NUM_ITERATION == 1 and "check if readMiniBatch is false in test(net)")
	//First argument {SecureML, Sarda, MiniONN, or LeNet}
	// network += " preloaded"; PRELOADING = true;
	// preload_network(PRELOADING, network, net);

	start_m();
	// runTest("Test", "MeteorPredictAvepool", network);
	// runTest("Test", "MeteorTrainAvepool", network);
	// end_m(network);

	// start_m();
	// // runTest("Test", "MeteorPredictAvepool", network);
	// runTest("Test", "MeteorRELU", network);
	// end_m(network);
	//start_communication();means initial
	//start_time();means initial
	
	//Run unit tests in two modes: 
	//	1. Debug {Mat-Mul, DotProd, PC, Wrap, ReLUPrime, ReLU, Division, BN, SSBits, SS, and Maxpool}
	//	2. Test {Mat-Mul1, Mat-Mul2, Mat-Mul3 (and similarly) Conv*, ReLU*, ReLUPrime*, and Maxpool*} where * = {1,2,3}
	// runTest("Debug", "Division", network);


	runTest("Test", "MeteorRELU", network);//This is really important for me to understand all the process!!!
	
	// MeteorRELU,Conv,BN,MeteorTrainAvepool,MeteorMaxpool,MeteorPC,MeteorDotProduct,MeteorMatMul,MeteorRELUPrime......
       
	// 

	// runTest("Test", "MeteorPredictAvepool", network);
	// runTest("Test", "MeteorTrainAvepool", network);
	//runTest("Test", "MeteorRELU", network);//This is really important for me to understand all the process!!!
	//MeteorAvepool
	// runTest("Test", "MeteorMaxpool", network);
	// Run forward/backward for single layers
	//  1. what {F, D, U}
	// 	2. l {0,1,....NUM_LAYERS-1}
	// size_t l = 0;
	// string what = "F";
	// runOnly(net, l, what, network);

	//Run training
	// network += " train";
	// train(net);

	//Run inference (possibly with preloading a network);
	// network += " test";
	// test(PRELOADING, network, net);

	end_m(network);//just get the answer and output it!
	cout << "----------------------------------------------" << endl;  	
	cout << "Run details: " << NUM_OF_PARTIES << "PC (P" << partyNum 
		 << "), " << NUM_ITERATIONS << " iterations, batch size " << MINI_BATCH_SIZE << endl 
		 << "Running " << security << " " << network << " on " << dataset << " dataset" << endl;
	cout << "----------------------------------------------" << endl << endl;  

	printNetwork(net);

 /****************************** CLEAN-UP ******************************/ 
	delete aes_indep;
	delete aes_next;
	delete aes_prev;
	delete config;
	delete net;
	deleteObjects();

	return 0;
}