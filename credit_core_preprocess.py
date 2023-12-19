import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
import scipy.stats as stats
import sys

import re
import time
import random
import tempfile


train_raw_path = "./data/credit-score/train_raw.csv"
train_path = "./data/credit-score/train.csv"

test_raw_path = "./data/credit-score/test_raw.csv"
test_path = "./data/credit-score/test.csv"


def fix_nums(num):
    try : 
        return float(num.replace("_",""))
    except :
        return np.nan

def History_age(age):
    try : 
        years = int("".join(re.findall('[0-9]',''.join(age.split("and")[0]))))
        month = int("".join(re.findall('[0-9]',''.join(age.split("and")[1]))))
        return years*12 + month
    except :
        return np.nan

def data_clean(file_path, save_path):

    df = pd.read_csv(file_path)
    d_col = ['ID','Month','Name','SSN']
    df = df.drop(d_col , axis=1).copy()
    # df = df[:1000]

    N_to_fix = ['Age', 'Annual_Income', 'Num_of_Loan', 'Num_of_Delayed_Payment', 'Changed_Credit_Limit', 'Amount_invested_monthly', 'Outstanding_Debt' , 'Monthly_Balance']
    for col in N_to_fix :
        df[col] = df[col].apply(fix_nums)

    ## Rebuild Type of loans Columns 
    for i in df['Type_of_Loan'].value_counts().head(9).index[1:]: 
        df[i] = df['Type_of_Loan'].str.contains(i)

    del df['Type_of_Loan']

    df['Num_Bank_Accounts'] = df['Num_Bank_Accounts'].apply(lambda x :abs (x))

    df['Num_Credit_Card'].replace(0,1,inplace=True)

    df['Credit_History_Age'] = df['Credit_History_Age'].apply(History_age)

    df['Payment_of_Min_Amount'].replace("NM","No",inplace=True)

    df['Payment_Behaviour']= df['Payment_Behaviour'].replace("!@9#%8",np.nan)

    df = df.dropna().copy()
    id_count = 0
    count = df[df['Occupation'] == "_______"]['Customer_ID'].count()
    for id_ in df[df['Occupation'] == "_______"]['Customer_ID']: 
        oc = df[df['Customer_ID'] == id_]['Occupation'].mode()[0]
        df[df['Customer_ID'] == id_] = df[df['Customer_ID'] == id_].replace("_______",oc)
        id_count += 1
        # print("("+str(id_count)+", "+str(count)+") \r")
        sys.stdout.write("("+str(id_count)+", "+str(count)+") \r")
        sys.stdout.flush()
    df['Occupation'] = df['Occupation'].replace("_______",df['Occupation'].mode()[0])
    # print(df['Occupation'].value_counts())

    m = {
        "Bad":0,
        "Standard":1,
        "Good":2,
        "_":np.nan
    }
    df['Credit_Mix'] = df['Credit_Mix'].map(m)

    # Edit Columns from bool to int 
    for col in list(df.columns[-8:]):
        df[col] = df[col].astype(float)

    # del df['Customer_ID']
    # df.insert(0, 'Customer_ID', range(0, 0 + len(df)))
    # IDs = 1 
    # for ID in df['Customer_ID'].unique() :
    #     df['Customer_ID'] = df['Customer_ID'].replace(ID,IDs)
    #     IDs += 1 
    #     print(IDs)
    # from sklearn.imputer import KNNImputer
    # imputer = KNNImputer(n_neighbors=1)
    Numericals = df.select_dtypes(exclude='object').columns[1:]
    # for col in Numericals[1:]:
    #     imputer.fit(df[['Customer_ID',col]])
    #     df[['Customer_ID',col]] = imputer.transform(df[['Customer_ID',col]])
    
    # imputer = SimpleImputer(strategy="most_frequent")
    # imputer.fit(df[['Payment_Behaviour']])
    # df[['Payment_Behaviour']] = imputer.transform(df[['Payment_Behaviour']])

    df = df.dropna().copy()
    
    # ## replace Outliers with median 
    # for col in Numericals :
    #     outliers_indecies = detect_outliers(df,0,[col])
    #     median = df[col].median()
    #     df[col].iloc[outliers_indecies] = median

    m = {
        "Poor":0,
        "Standard":1,
        "Good":2
    }
    df['Credit_Score'] = df['Credit_Score'].map(m)
    print(df['Credit_Score'].value_counts())


    del df['Customer_ID']
    df = pd.get_dummies(df,drop_first=True)

    col = df.pop('Credit_Score')
    df=(df-df.min())/(df.max()-df.min())
    df.insert(len(df.columns), 'Credit_Score', col)

    m = {
        0:'Poor',
        1:'Standard',
        2:'Good'
    }
    df['Credit_Score'] = df['Credit_Score'].map(m)

    print(df.info())
    print(df.shape)
    df.to_csv(save_path, index=False)


def print_info(file_name):
    df = pd.read_csv(file_name)
    print df.info()
    print "shape of data: ", df.shape


if __name__=="__main__":
    # data_clean(train_raw_path, train_path)
    # data_clean(test_raw_path, test_path)
    print_info(train_path)
