import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import gridspec
import numpy as np


def drawMaintenanceQuery():
  fig = plt.figure(figsize=(13, 5.5))
  gs = gridspec.GridSpec(1, 2, width_ratios=[4.4, 3.6])
  # ax1 = fig.add_subplot(1, 2, 1)
  ax1 = plt.subplot(gs[0])
  # xList = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
  # xList = ["1", "2", "3", "4", "5"]
  xList = ["256", "512", "1024", "2048", "4096"]
  origin = []
  maintenance_fine = []
  query_fine = []
  # origin = [0.000687, 0.0101, 0.018671, 0.028236, 0.038523]
  # maintenance_fine = [0.006961, 0.054,  0.101907, 0.152218, 0.197581]
  # query_fine = [0.006967, 0.04, 0.08362,  0.124492, 0.165602]
  # query_fine = np.array(query_fine)*1000
  # maintenance_coarse = [0.206752, 0.476931, 0.766481, 0.98802, 1.23495, 1.54035, 1.76545, 1.99586, 2.22024, 2.41902]
  # query_coarse = [0.000264171, 0.000294356, 0.000330009, 0.000365438, 0.0003871, 0.000420653, 0.000438427, 0.000464096, 0.000474362, 0.000492065]
  # query_coarse = np.array(query_coarse)*1000
  x = np.arange(len(xList))
  w = 0.25
  lw = 2
  ax1.bar(x-w, origin, w, label='W/o provenance', color="tab:blue", edgecolor="black", linewidth=lw)
  ax1.bar(x, maintenance_fine, w, label='W/ provenance', color="tab:orange", edgecolor="black", linewidth=lw)
  # ax1.bar(x+w, maintenance_coarse, w, label="W/ coarser provenance", color="tab:green", edgecolor="black", linewidth=lw)

  ax2 = plt.subplot(gs[1])
  ax2.plot(x, query_fine, marker="^", markersize=15, color="tab:orange", linewidth=3, alpha=0.9)
  # ax2.plot(x, query_coarse, label="Coarser provenance", marker="s", markersize=15, color="tab:green", linewidth=3, alpha=0.9)
  # box = ax.get_position()
  # ax.set_position([box.x0, box.y0+box.height*0.1, box.width, box.height*0.8])

  fs1 = 24
  fs2 = 20
  ax1.set_ylabel('Model inference time (s)', fontsize=fs1)
  ax1.set_yscale("linear")
  ax1.set_xlabel('Number of nodes per layer\n(a)', fontsize=fs1)
  ax1.set_xticks(x)
  ax1.set_xticklabels(xList, fontsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.set_ylabel('Backward trace time (s)', fontsize=fs1)
  ax2.set_yscale("linear")
  ax2.set_xlabel('Number of nodes per layer\n(b)', fontsize=fs1)
  ax2.set_xticks(x)
  ax2.set_xticklabels(xList, fontsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  # ax.set_yticks(list(range(0, 301, 50)))
  # ax.set_yticklabels([str(ele) for ele in list(range(0, 301, 50))], fontsize=fs2)
  # ax2.set_yticks(list(range(0, 9, 1)))
  # ax2.set_yticklabels([str(ele) for ele in list(range(0, 8))], fontsize=fs2)
  # fig.legend(fontsize=19, loc='upper center', bbox_to_anchor=(0.3,0.8),fancybox=True)
  ax1.grid()
  ax2.grid()
  ax1.legend(fontsize=fs2)
  ax2.legend(fontsize=fs2)
  plt.tight_layout()
  plt.savefig("./data/credit-score/images/mlp_pxai_maintain_query.png")



def overallComparison2():
  # mlp = [95, 121*2, 131*3, 146*4, 154*5, 175*6, 187*7, 211*8, 229*9, 269*10]
  # mlp = [ 0.000687*46*2, 0.01*46*2,  0.019637*46*2, 0.028236*92, 0.038523*92, 0.049059*92]
  mlp = []
  mlp = np.array(mlp)
  
  fig = plt.figure(figsize=(13, 5.5))
  gs = gridspec.GridSpec(1, 3, width_ratios=[4.4, 2.6, 2.6])
  # ax1 = fig.add_subplot(1, 2, 1)
  ax = plt.subplot(gs[0])
  ax1 = plt.subplot(gs[1])
  ax2 =  plt.subplot(gs[2])

  # xList = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
  # xList = ["1", "2", "3", "4", "5"]
  xList = ["256", "512", "1024", "2048", "4096"]
  w = 0.3
  lw = 2
  x = np.arange(len(xList))

  ax.plot(x, mlp, label='ICE on MLP models', marker="D", markersize=15, color="tab:blue", linewidth=4, alpha=0.9)

  # approx_times = [0.00547228, 0.00469242, 0.00479458, 0.00544853, 0.00569537, 0.00629687, 0.00644849, 0.00663826, 0.00715675, 0.00755559, ]
  # ax1.bar(x-w/2, approx_times, w, color="tab:purple", label="Subset search", edgecolor="black", linewidth=lw, hatch='x', alpha=0.7)

  # approx_time_prune = [0.766496, 14.23, 23.9478, 39.2783, 45.4645, 57.0926]
  approx_time_prune = []
  ax1.bar(x, approx_time_prune, 0.5, color="tab:green", label="Prune", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  # influ_times = [0.001396*46*2, 0.012*46*2, 0.022598*92, 0.03339*92, 0.044629*92, 0.055715*92]
  influ_times = []
  ax.bar(x-0.5*w, influ_times, w, label='ICE on original PROV graph', color="tab:orange", hatch='.', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times = [0.04, 0.1, 0.2, 0.45, 1, 1.9, 2.3, 3, 5.5, 9.061]
  # ax.bar(x+w/2, counter_times, w, bottom=influ_times, label='CFE w/o Approx Query', color="tab:green", hatch='++', edgecolor="black", linewidth=0, alpha=0.7)
  
  # influ_times_approx = [0.000157281, 0.000232689, 0.000351128, 0.000627773, 0.000951316, 0.00250335, 0.00387824, 0.00494315, 0.00629363, 0.00786202, ]
  # ax.bar(x, influ_times_approx, w, label="ICE on approx subgraph (subset search)", color="tab:purple", hatch='x', edgecolor="black", linewidth=lw, alpha=0.7)


  # influ_time_approx_prune = [0.001297*46*2, 0.003*46*2,  0.017573*92, 0.024968*92, 0.043729*92, ]
  influ_time_approx_prune = []
  ax.bar(x+0.5*w, influ_time_approx_prune, w, label="ICE on approx subgraph (prune)", color="tab:green", hatch='-', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times_approx = [4, 8, 12, 15, 24, 50, 56, 85, 100, 123]
  # ax.bar(x-w/2, counter_times_approx, w, bottom=[approx_times[i]+influ_times_approx[i] for i in range(10)], label="CFE w/ Approx Query", color="tab:green", hatch='xx', edgecolor="black", linewidth=0, alpha=0.7)
  
  # box = ax.get_position()
  # ax.set_position([box.x0, box.y0+box.height*0.05, box.width, box.height*0.95])

  origin_model_size = [78336, 287744, 1099776, 4296704, 16982016]
  ax2.plot(x, origin_model_size, label="original PROV", marker="D", markersize=15, color="tab:orange", linewidth=4, alpha=0.9)

  approx_model_size = []
  ax2.plot(x, approx_model_size, label="approx PROV", marker="D", markersize=15, color="tab:green", linewidth=4, alpha=0.9)

  fs1 = 20
  fs2 = 16
  ax.set_ylabel('Running time of ICE (s)', fontsize=fs1)
  ax.set_xlabel('Number of nodes per layer\n (a)', fontsize=fs1)
  ax1.set_ylabel('Running time of approx search (s)', fontsize=fs1)
  ax1.set_xlabel('Number of nodes per layer\n (b)', fontsize=fs1)
  ax2.set_ylabel('Number of parameters', fontsize=fs1)
  ax2.set_xlabel('Number of nodes per layer\n (b)', fontsize=fs1)
  # ax.set_yscale("log")
  # ax1.set_yscale("log")
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  # ax.set_yticks([0.001, 0.01, 0.1, 1, 10, 100, 1000])
  # ax.set_yticklabels(["1e-3","1e-2","1e-1","1","10","1e2", "1e3"], fontsize=fs2)
  ax1.set_xticks(x)
  ax1.set_xticklabels(xList, fontsize=fs2)
  # ax1.set_yticks([0.01, 0.1, 1, 10])
  # ax1.set_yticklabels(["1e-2","1e-1", "1", "10"], fontsize=fs2)
  ax2.set_xticks(x)
  ax2.set_xticklabels(xList, fontsize=fs2)
  # ax2.set_yticks([0.01, 0.1, 1, 10])
  # ax2.set_yticklabels(["1e-2","1e-1", "1", "10"], fontsize=fs2)
  ax.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  # fig.legend(fontsize=26, ncol=1, loc='upper center', bbox_to_anchor=(0.7,0.76),fancybox=True)
  ax.legend(fontsize=fs2)
  ax1.legend(fontsize=fs2)
  ax2.legend(fontsize=fs2)
  ax.grid(axis='y')
  ax1.grid(axis='y')
  ax2.grid(axis='y')
  plt.tight_layout()
  plt.savefig("./data/credit-score/images/mlp_pxai_approx_ice_2.png")

if __name__=="__main__":
  drawMaintenanceQuery()
  overallComparison2()