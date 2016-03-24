import matplotlib as mpl
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
from pylab import rcParams
rcParams['figure.figsize'] = 25, 15

mpl.rc('ytick', labelsize=20)
mpl.rc('xtick', labelsize=20)
plt.xlabel('Number of elements', fontsize=24)
plt.ylabel('Time (seconds)', fontsize=24)


def cleanup(file_name):
    with open('Results/fixed.csv', 'w') as dest:
        with open(file_name, 'r') as f:
            for line in f:
                dest.write(' '.join(line.split()))
                dest.write('\n')


def plot_graph(file_name, bytes):
    plt.suptitle('memory_insert -- EpicStruct of {0} Bytes'.format(bytes), fontsize=24)
    cleanup(file_name)

    data = pd.read_csv('Results/fixed.csv', names=['Count', 'MMAnagerComplete', 'MMAnagerArray', 'MMAnagerList', 'MMAnagerMalloc'], delimiter=' ')
    x = data['Count']
    vector, = plt.plot(x, data['MMAnagerComplete']/1000.0, label='MManagerComplete')
    list, = plt.plot(x, data['MMAnagerArray']/1000.0, label='MManagerArray')
    myvector, = plt.plot(x, data['MMAnagerList']/1000.0, label='MManagerList')
    myvector_2, = plt.plot(x, data['MMAnagerMalloc']/1000.0, label='MManagerMalloc')
    plt.legend(handles=[vector, list, myvector, myvector_2], prop={'size': 24}, loc='upper left')
    plt.savefig('memory_insert_{0}_byte.png'.format(bytes))
    plt.show()

plot_graph('Results/memory_insert_256_byte.csv', 256)
