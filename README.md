# PXAI

Run ```make all``` to compile three executable files in ./bin.

Run ```./bin/cprov-maintain-query-overhead``` to estimate PXAI's provenance maintenance and query performance when applied to probabilistic graphical models (PGM).

Run ```./bin/cprov-approx-ice-counter-overhead``` to estimate how PXAI's XAI performance when applied to PGM.

Run ```./bin/credit-score-mlp-test``` to estimate PXAI's performance when applied to multi-layer perceptron (MLP).

Run ```./bin/mnist-kmeans-maintain-query-test``` to estimate PXAI's provenance maintenance performance when applied to k-Means.

Run ```./bin/mnist-kmeans-test``` to estimate PXAI's ML deletion performance when applied to k-Mneas.

Run a test case of PGM: ```./bin/pxai -o ./data/hypertext-class/sample7/sample71.obs -p ./data/hypertext-class/sample7/prov/sample71.txt -q topic_Department_29 -i all ``` It computes the influences of the output to explain __topic(Department, 29)__ through Individual Conditional Expectation (ICE).

Run ```make``` to rebuild the project and run a test case.


