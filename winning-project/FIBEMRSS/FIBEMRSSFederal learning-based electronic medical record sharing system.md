# FIBEMRSS:Federal learning-based electronic medical record sharing system

## Project Background

The traditional medical record objectively records the work content and process of clinical medical personnel, and its efficiency is low and the data is insecure and inaccurate. The existing electronic medical record also has three problems: ① data security, the data stored in the electronic medical record is very sensitive, if it is illegally obtained, it will have a serious impact on the patient's privacy; ② data accuracy: the data in the electronic medical record is input by doctors and nurses and other personnel, it is easy to make input errors, resulting in inaccurate data; ③ data compatibility: the electronic medical record systems between different hospitals are usually not mutually compatible, and it is difficult to share and dock the patient's medical record data.

## Main Ideas

To solve the above problems, a federated learning-based electronic medical record sharing system is proposed. Its workflow is as follows: First, each hospital needs to establish an electronic medical record system locally, and clean and standardize the local data to ensure the quality and availability of the data. Secondly, by means of federal learning, each hospital can participate local data sources together in model training, so as to achieve data sharing and integration. Specifically, each hospital will upload the intermediate parameters of the local model to the central server, which will aggregate and encrypt all the collected parameters to generate new global model parameters and redistribute the updated global model parameters to each hospital. In this process, the original data does not need to go out of the local area, so the original data does not need to be circulated and privacy protection of the data is achieved. Then, individual hospitals can retrain their local models based on the new global model parameters and apply the new model parameters locally for prediction and diagnosis. Finally, the results after federal learning are used to make judgments on current cases and provide references for diagnosis.

## Environmental Configuration

Spring+SpringMVC+Mybitas

Python V3.7

pytorch V1.4

Tomcat V9.0

