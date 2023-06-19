# recommendation-system-with-word2vec-and-streamlit
step 1 - Download df2 and train_df datastes at https://www.kaggle.com/steph2019/sample-data-of-customers-purchases/download

step 2 - Clone this repository 

step 3 - Add the two datasets to the repository you've just cloned

step 4 - In your terminal, navigate to the repository and enter 
```pip3 install -r requirements.txt```

step 5 - Type the following in your terminal to run the app: 
```streamlit run app_2_0.py```

Your browser should automatically open, otherwise copy-paste the localhost address in your browser.

On the app you can allow (check or uncheck) the two recommendations on the sidebar. 
The first one recommends items based on one individual input item. 
The second that comes with a slider, does some recommendations based on all a customer's purchases. Each number on the slider is an individual customer.
Then, the algorithm downloads those items's pictures on Google Image and show you what the recommendations look like (might take a minute).
