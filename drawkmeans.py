import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import gridspec
import numpy as np


def drawMaintenanceQuery():
  
  fig = plt.figure(figsize=(10, 5.5))
  gs = gridspec.GridSpec(1, 2, width_ratios=[4, 3])
  ax1 = plt.subplot(gs[0])
  ax2 = plt.subplot(gs[1])

  xList1 = ["2000", "4000", "6000", "8000", "10000"]
  lw = 2
  x1 = np.arange(len(xList1))

  with_prov1 = [27.9004, 77.3078, 149.174, 155.986, 259.437]
  with_prov1 = np.array(with_prov1)
  ax1.bar(x1+0.15, with_prov1, 0.3, label='With provenance', color="tab:orange", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  without_prov1 = [5.05479, 15.56, 0, 0, 45.4808]
  without_prov1 = np.array(without_prov1)
  ax1.bar(x1-0.15, without_prov1, 0.3, color="tab:blue", label="Without provenance", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  xList2 = ["10", "40", "70", "100"]
  lw = 2
  x2 = np.arange(len(xList2))

  with_prov2 = [295.131, 525.367, 666.441, 1007.15]
  with_prov2 = np.array(with_prov2)
  ax2.bar(x2+0.15, with_prov2, 0.3, label='With provenance', color="tab:orange", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  without_prov2 = [256.935, 442.065, 544.753, 847.345]
  without_prov2 = np.array(without_prov2)
  ax2.bar(x2-0.15, without_prov2, 0.3, color="tab:blue", label="Without provenance", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  fs1 = 20
  fs2 = 16
  ax1.set_ylabel('Running time (s))', fontsize=fs1)
  ax1.set_xlabel('# of points\n (a)', fontsize=fs1)
  ax2.set_ylabel('Running time (s)', fontsize=fs1)
  ax2.set_xlabel('# of clusters\n (b)', fontsize=fs1)
  ax1.set_xticks(x1)
  ax1.set_xticklabels(xList1, fontsize=fs2)
  ax2.set_xticks(x2)
  ax2.set_xticklabels(xList2, fontsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.legend(fontsize=fs2)
  ax2.legend(fontsize=fs2)
  ax1.grid(axis='y')
  ax2.grid(axis='y')
  plt.tight_layout()
  plt.savefig("./data/mnist/images/kmeans_pxai_maintain_query.pdf")



def overallComparison2():
  delete = [24.6336, 77.3078, 134.21, 134.697, 276.032]
  delete = np.array(delete)
  
  fig = plt.figure(figsize=(10, 5.5))
  gs = gridspec.GridSpec(1, 2, width_ratios=[4, 3])
  # ax1 = fig.add_subplot(1, 2, 1)
  ax1 = plt.subplot(gs[0])
  ax2 = plt.subplot(gs[1])

  xList1 = ["2000", "4000", "6000", "8000", "10000"]
  w = 0.3
  lw = 2
  x1 = np.arange(len(xList1))

  ax1.plot(x1, delete, label='Baseline', marker="D", markersize=15, color="tab:blue", linewidth=4, alpha=0.9)

  pxai_delete = [5.05479, 15.56, 23.0603, 20.1936, 45.0959]
  ax1.bar(x1, pxai_delete, 0.5, color="tab:orange", label="PXAI", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  xList2 = ["10", "40", "70", "100"]
  w = 0.3
  lw = 2
  x2 = np.arange(len(xList2))

  delete2 = [134.21, 239.986, 226.456, 320.61]
  ax2.plot(x2, delete2, label='Baseline', marker="D", markersize=15, color="tab:blue", linewidth=4, alpha=0.9)

  pxai_delete1 = [23.0603, 104.576, 82.9445, 76.1995]
  ax2.bar(x2, pxai_delete1, 0.5, color="tab:orange", label="PXAI", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  fs1 = 20
  fs2 = 16
  ax1.set_ylabel('Running time of ML deletion (s)', fontsize=fs1)
  ax1.set_xlabel('# of points\n (a)', fontsize=fs1)
  ax2.set_ylabel('Running time of ML deletion (s)', fontsize=fs1)
  ax2.set_xlabel('# of clusters\n (b)', fontsize=fs1)
  ax1.set_xticks(x1)
  ax1.set_xticklabels(xList1, fontsize=fs2)
  ax2.set_xticks(x2)
  ax2.set_xticklabels(xList2, fontsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.legend(fontsize=fs2)
  ax2.legend(fontsize=fs2)
  ax1.grid(axis='y')
  ax2.grid(axis='y')
  plt.tight_layout()
  plt.savefig("./data/mnist/images/kmeans_pxai_approx_ice_3.pdf")


def drawAll():
  fig = plt.figure(figsize=(24, 4.75))
  gs = gridspec.GridSpec(1, 6, width_ratios=[5.5, 4, 4, 4, 3, 4])


  ax1 = plt.subplot(gs[0])
  xList = ["2k", "4k", "6k", "8k", "10k"]
  origin = [27.7635, 81.3695, 142.966, 190.122, 242.7]
  origin = np.array(origin) 
  maintenance_fine = [31.5434, 91.9212, 161.606, 213.326, 272.925]
  maintenance_fine = np.array(maintenance_fine) 
  x = np.arange(len(xList))
  w = 0.25
  lw = 2
  ax1.bar(x-w, origin, w, label='W/o provenance', color="tab:blue", edgecolor="black", linewidth=lw)
  ax1.bar(x, maintenance_fine, w, label='W/ provenance', color="tab:orange", edgecolor="black", linewidth=lw)

  ax4 = plt.subplot(gs[3])
  xList2 = ["10", "20", "50", "100"]
  origin = [28.7788, 36.1709, 67.8558, 162.261]
  origin = np.array(origin) 
  maintenance_fine = [32.5852, 42.7043, 76.4814, 174.536]
  maintenance_fine = np.array(maintenance_fine) 
  x2 = np.arange(len(xList2))
  w = 0.25
  lw = 2
  ax4.bar(x2-w, origin, w, label='W/o provenance', color="tab:blue", edgecolor="black", linewidth=lw)
  ax4.bar(x2, maintenance_fine, w, label='W/ provenance', color="tab:orange", edgecolor="black", linewidth=lw)

  fs1 = 18
  fs2 = 16
  ax1.set_ylabel('K-means running time (s)', fontsize=fs1)
  ax1.set_yscale("linear")
  ax1.set_xlabel('Number of points\n(a)', fontsize=fs1)
  ax1.set_xticks(x)
  ax1.set_xticklabels(xList, fontsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax4.set_ylabel('K-means running time (s)', fontsize=fs1)
  ax4.set_yscale("linear")
  ax4.set_xlabel('Number of clusters\n(d)', fontsize=fs1)
  ax4.set_xticks(x2)
  ax4.set_xticklabels(xList2, fontsize=fs2)
  ax4.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.grid()
  ax4.grid()
  ax1.legend(fontsize=fs2)
  ax4.legend(fontsize=fs2)

  
  ax3 = plt.subplot(gs[2])
  ax6 = plt.subplot(gs[5])
  
  w = 0.5
  lw = 2
  

  xList = ["2k", "4k", "6k", "8k", "10k"]
  x = np.arange(len(xList))
  baseline = [27.997, 75.7149, 135.234, 190.122, 242.7]
  baseline = np.array(baseline)
  ax3.plot(x, baseline, label='Baseline', marker="D", markersize=12, color="tab:blue", linewidth=4, alpha=0.9)

  pxai_times = [5.78048, 15.1784, 25.912, 36.1866, 45.4948]
  pxai_times = np.array(pxai_times)
  ax3.bar(x, pxai_times, w, label='PXAI', color="tab:orange", hatch='.', edgecolor="black", linewidth=lw, alpha=0.7)

  xList2 = ["10", "20", "50", "100"]
  x2 = np.arange(len(xList2))
  baseline = [27.997, 36.1709, 67.8558, 162.261]
  baseline = np.array(baseline)
  ax6.plot(x2, baseline, label='Baseline', marker="D", markersize=12, color="tab:blue", linewidth=4, alpha=0.9)

  pxai_times = [5.78048, 9.67031, 7.39733, 4.61501]
  pxai_times = np.array(pxai_times)
  ax6.bar(x2, pxai_times, w, label='PXAI', color="tab:orange", hatch='.', edgecolor="black", linewidth=lw, alpha=0.7)

  ax3.set_ylabel('Running time of \nk-means deletion (s)', fontsize=fs1)
  ax3.set_xlabel('Number of points\n (c)', fontsize=fs1)
  ax6.set_ylabel('Running time of \nk-means deletion (s)', fontsize=fs1)
  ax6.set_xlabel('Number of clusters \n (f)', fontsize=fs1)
  ax3.set_xticks(x)
  ax3.set_xticklabels(xList, fontsize=fs2)
  ax6.set_xticks(x2)
  ax6.set_xticklabels(xList2, fontsize=fs2)
  ax3.tick_params(axis="y", which="both", labelsize=fs2)
  ax6.tick_params(axis="y", which="both", labelsize=fs2)
  ax3.legend(fontsize=fs2)
  ax6.legend(fontsize=fs2)
  ax3.grid()
  ax6.grid()



  ax2 = plt.subplot(gs[1])
  ax5 = plt.subplot(gs[4])
  
  w = 0.5
  lw = 2
  

  xList = ["2k", "4k", "6k", "8k", "10k"]
  x = np.arange(len(xList))

  pxai_storage = [27.1101, 68.915, 115.704, 154.347, 199.826]
  pxai_storage = np.array(pxai_storage)
  ax2.bar(x, pxai_storage, w, label='PXAI', color="tab:orange", edgecolor="black", linewidth=lw, alpha=0.9)

  xList2 = ["10", "20", "50", "100"]
  x2 = np.arange(len(xList2))

  pxai_storage = [27.8016, 47.3311, 76.3657, 112.572]
  pxai_storage = np.array(pxai_storage)
  ax5.bar(x2, pxai_storage, w, label='PXAI', color="tab:orange", edgecolor="black", linewidth=lw, alpha=0.9)

  ax2.set_ylabel('In-memory storage (MB)', fontsize=fs1)
  ax2.set_xlabel('Number of points\n (b)', fontsize=fs1)
  ax5.set_ylabel('In-memory storage (MB)', fontsize=fs1)
  ax5.set_xlabel('Number of clusters \n (e)', fontsize=fs1)
  ax2.set_xticks(x)
  ax2.set_xticklabels(xList, fontsize=fs2)
  ax5.set_xticks(x2)
  ax5.set_xticklabels(xList2, fontsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  ax5.tick_params(axis="y", which="both", labelsize=fs2)
  # ax2.legend(fontsize=fs2)
  # ax5.legend(fontsize=fs2)
  ax2.grid()
  ax5.grid()

  plt.tight_layout()
  plt.savefig("./data/mnist/images/kmeans_pxai_all.pdf")

if __name__=="__main__":
  # drawMaintenanceQuery()
  # overallComparison2()

  drawAll()