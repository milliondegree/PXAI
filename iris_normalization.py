import pandas as pd

def scale_data(file_path, save_path):
    df = pd.read_csv(file_path, header=None)
    print(df.info())

    col = df.pop(4)
    df=(df-df.min())/(df.max()-df.min())
    df.insert(len(df.columns), 4, col)

    print(df.info())
    df.to_csv(save_path, index=False, header=False)



if __name__=='__main__':
    scale_data("./data/iris/iris.data", "./data/iris/iris_normal.data")