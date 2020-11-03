import numpy as np
from scipy.io import loadmat
import matplotlib.pyplot as plt
import utide
import datetime as d
# import sklearn.gaussian_process.kernels as ke

# read all data files in one file for later processing
def read_data(base_path):
    name = ["t1", "t2", "t3", "t4", "v1", "v2", "v3", "v4"]
    data = []
    for i in name:
        path = base_path + i + ".mat"
        tmp = loadmat(path)
        tmp_data = tmp[i]
        
        data.append(tmp_data.reshape(-1))

    return data


# read corresponding location to node
def get_locations(node_number):
    f = open("./Data/fort.14")
    f.readline()
    f.readline()

    all_data = []
    for i in range(5200):
        read_txt = f.readline().split()
        for i in range(4):
            read_txt[i] = float(read_txt[i])

        all_data.append(read_txt)

    # returen the longitutde and latitude corresponds to the node
    return all_data[node_number][1], all_data[node_number][2]

# retrun all t u v location for one file
def node(base_path, node_number):
    long_node, lat_node = get_locations(node_number)
    
    data = read_data(base_path)
    
    u_node = np.array([])
    v_node = np.array([])
    for i in range(4):
        u_node = np.concatenate((u_node, data[4+i][192:].real))
        v_node = np.concatenate((v_node, data[4+i][192:].imag))
    
    for i in range(4):
        data[i] = data[i] / 86400
        
    # 直接计算四个日期间用小数表示的形式直接进行计算！！！后期整合再全部使用datetime
    t1 = (data[0][192:])
    t2 = (data[1][192:] + (np.datetime64('2014-02-28') - np.datetime64('2013-12-30')).astype(int))
    t3 = (data[2][192:] + (np.datetime64('2014-04-29') - np.datetime64('2013-12-30')).astype(int))
    t4 = (data[3][192:] + (np.datetime64('2014-06-28') - np.datetime64('2013-12-30')).astype(int))
    
    
    
    t_raw_node = np.concatenate((t1,t2,t3,t4))
    u_raw_node = u_node
    v_raw_node = v_node
    
    return t_raw_node, u_raw_node, v_raw_node, long_node, lat_node

