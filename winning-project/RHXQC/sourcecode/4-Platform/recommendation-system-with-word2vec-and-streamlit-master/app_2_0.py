import streamlit as st
import time

import pandas as pd
import numpy as np
import random
from tqdm import tqdm
from gensim.models import Word2Vec 
#import matplotlib.pyplot as plt

import pickle
import joblib

from google_images_search import GoogleImagesSearch
import os
import sys
import inspect
current_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parent_dir = os.path.dirname(current_dir)
sys.path.insert(0, parent_dir) 
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import streamlit_authenticator as stauth

#importing google_images_download module 
from google_images_download import google_images_download

#create object
response = google_images_download.googleimagesdownload()

#to resize images
from PIL import Image
from resizeimage import resizeimage
import time

from yaml.loader import SafeLoader
import yaml

# print(stauth.Hasher(['abc', 'def']).generate())
if 'key' not in st.session_state:
    st.session_state['key'] = '$$GENERATED_WIDGET_ID-036dbddadb201dcd4ab9b130aef1d101-None'

# Session State also supports the attribute based syntax

with open("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/_config.yml") as file:
    config = yaml.load(file, Loader=SafeLoader)

authenticator = stauth.Authenticate(
    config['credentials'],
    config['cookie']['name'],
    config['cookie']['key'],
    config['cookie']['expiry_days'],
    config['preauthorized']
)
name, authentication_status, username = authenticator.login('Login', 'main')
if authentication_status:
    authenticator.logout('Logout', 'main')
    st.write(f'Welcome *{name}*')
    st.title('Recommendation system app')

    data = ('C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/df2.csv')
    "THE DATABASE"

    @st.cache_data
    def load_data(nrows):
        df = pd.read_csv(data,nrows=nrows)
        return df
    df = load_data(5000)

    st.dataframe(df)

    #load pre_trained model
    open_model = open('C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/Rec_sys_model.pkl','rb')
    model = joblib.load(open_model)

    #As we do not plan to train the model any further, we are calling init_sims(), which will make the model much more memory-efficient.
    model.init_sims(replace=True)

    # extract all vectors
    X = model[model.wv.vocab]

    ###UMAP

    #create a product-ID and product-description dictionary to easily map a product's description to its ID and vice versa
    train_df = pd.read_csv("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/train_df.csv")

    products = train_df[["StockCode", "Description"]]

    # remove duplicates
    products.drop_duplicates(inplace=True, subset='StockCode', keep="last")

    # create product-ID and product-description dictionary
    products_dict = products.groupby('StockCode')['Description'].apply(list).to_dict()

    #generate with one product's vector as input
    def similar_products(v, n = 6):
        
        # extract most similar products for the input vector
        ms = model.similar_by_vector(v, topn= n+1)[1:]
        
        # extract name and similarity score of the similar products
        new_ms = []
        for j in ms:
            pair = (products_dict[j[0]][0], round(j[1],2))
            new_ms.append(pair)
            
        return new_ms 

    #checkbox to execute code
    checkbox_1 = st.sidebar.checkbox('Execute recommendation based on a single item')

    #add a selectbox
    option = st.sidebar.selectbox(
        'Select a product ID',
        df['StockCode'])

    product_name = df.loc[df['StockCode'] == option, 'Description'].tolist()[0]

    st.sidebar.text(product_name)

    if not os.path.exists('C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py'):
        os.makedirs('C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py')

    #check if product folder aready exist, else download image
    async def item(product_name):
        arguments = {"keywords":product_name,"limit":1,"print_urls":True} #creating list of arguments
        os.chdir("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py")
        if os.path.isdir(product_name) :
            os.chdir(product_name)
            image = os.listdir()
            img=mpimg.imread(image[0]) 
            imgplot = plt.imshow(img)
            plt.axis('off')
            st.sidebar.pyplot()
            os.chdir("../..")
            
        
        else : 
            print(arguments, '=========================')
            os.chdir("..")
            paths = await response.download(arguments)   #passing the arguments to the function
            print(paths, '=========================')
            os.chdir("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py")
            os.chdir(product_name)
            image = os.listdir()
            os.rename(image[0], product_name+'.jpg')
            image = os.listdir()
            img=mpimg.imread(image[0])
            imgplot = plt.imshow(img)
            plt.axis('off')
            st.sidebar.pyplot()
            os.chdir("../..")
            

    async def reco(product_name):
        arguments = {"keywords":product_name,"limit":1,"print_urls":True} #creating list of arguments
        os.chdir("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py")
        if os.path.isdir(product_name) :
            try: #try to read the image, if no image delete remove the folder and download the image
                os.chdir(product_name)
                image = os.listdir()
                if image[0] == '.DS_Store' : #Mac creates an .DS_store hidden file in any folder which is counted in listdir()
                    del image[0]
                img=mpimg.imread(image[0]) #normally image[0] but jupyter add a .ipynb_checkpoints in 1st position, put image[0] for streamlit
                list_img.append(img)
                os.chdir("../..")
            except IndexError:
                os.chdir("..")
                shutil.rmtree(product_name, ignore_errors=True) #because mac refuses to delete a directory that is not empty
                os.chdir("..")
                paths = await response.download(arguments)   #passing the arguments to the function
                os.chdir("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py")
                os.chdir(product_name)
                image = os.listdir()
                img=mpimg.imread(image[0])
                list_img.append(img)
                os.chdir("../..")
        
        else : 
            os.chdir("..")
            paths = await response.download(arguments)   #passing the arguments to the function
            os.chdir("C:/Users/Jupiter/Desktop/Hack Privacy Computing/Demo Code/4-模型接口平台与广告任务发布平台/recommendation-system-with-word2vec-and-streamlit-master/app_2_0.py")
            os.chdir(product_name)
            image = os.listdir()
            img=mpimg.imread(image[0])
            list_img.append(img)
            os.chdir("../..")

    item(product_name)

    if checkbox_1 == True:
        st.write("YOUR RECOMMENDATIONS")
        prediction = similar_products(model[option])
        prediction2 = pd.DataFrame(similar_products(model[option]), columns = ["Product Name","% similarity"])
        st.table(prediction2)
        
        #create en empty list to store all images
        list_img = []
        for i in prediction:
            reco(str(i[0]))

        #plot images
        # j=0
        # fig = plt.figure()
        # fig.set_size_inches(30, 20)
        # for pic in list_img:
        #     ax = fig.add_subplot(2,3,j+1)
        #     plt.axis('off')
        #     plt.grid(b=None)
        #     ax.imshow(pic)
        #     plt.title(j, fontsize=100, y=1)
        #     j+=1
        # st.pyplot(fig)


    #checkbox to execute code
    checkbox_2 = st.sidebar.checkbox("Execute recommendation based on all customer's purchases")

    # extract 90% of customer ID's
    customers = df["CustomerID"].unique().tolist()
    # shuffle customer ID's
    random.shuffle(customers)
    # extract 90% of customer ID's
    customers_train = [customers[i] for i in range(round(0.9*len(customers)))]

    #validation set
    validation_df = df[~df['CustomerID'].isin(customers_train)]

    # list to capture purchase history of the customers
    purchases_val = []

    # populate the list with the product codes
    for i in tqdm(validation_df['CustomerID'].unique()):
        temp = validation_df[validation_df["CustomerID"] == i]["StockCode"].tolist()
        purchases_val.append(temp)

    value = st.sidebar.slider('Select a customer', 0, len(purchases_val), 0)

    #execute recommendation if checkbox_2 is...well...checked
    if checkbox_2 == True : 
        #function to aggregate all products of a customer
        def aggregate_vectors(products):
            product_vec = []
            for i in products:
                try:
                    product_vec.append(model[i])
                except KeyError:
                    continue
                
            return np.mean(product_vec, axis=0)


        st.write("YOUR RECOMMENDATIONS")

        #Make recommendations
        #start_time = time.time()

        prediction2 = similar_products(aggregate_vectors(purchases_val[value]))
        prediction2_bis = pd.DataFrame(prediction2, columns = ["Product Name","Related"])
        st.table(prediction2_bis)

        #seconds = round((time.time() - start_time),2)
        #st.write("Table --- %s seconds ---" % (seconds))

        #create en empty list to store all images
        list_img = []
        for k in prediction2:
            reco(str(k[0]))

        print(list_img, '????????111111')
        #plot images
        #start_time = time.time()
        # j=0
        # fig = plt.figure()
        # fig.set_size_inches(30, 20)
        # for pic2 in list_img:
        #     ax = fig.add_subplot(2,3,j+1)
        #     plt.axis('off')
        #     plt.grid(b=None)
        #     ax.imshow(pic2)
        #     plt.title(j, fontsize=100, y=1)
        #     j+=1
        # st.pyplot(fig)

        #seconds = round((time.time() - start_time),2)
        #st.write("Images --- %s seconds ---" % (seconds))

        #st.write('END OF PAGE')

elif authentication_status is False:
    st.error('Username/password is incorrect')
elif authentication_status is None:
    st.warning('Please enter your username and password')



