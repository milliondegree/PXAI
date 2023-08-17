# PXAI

Run ```make all``` to compile three executable files in ./bin.

Run ```./bin/cprov-maintain-query-overhead``` to estimate PXAI's provenance maintainance and query overheads.

Run ```./bin/cprov-approx-ice-counter-overhead``` to estimate how PXAI accelerate XAI computation.

Run a study case of PGM: ```./bin/pxai -o ./data/hypertext-class/sample7/sample71.obs -p ./data/hypertext-class/sample7/prov/sample71.txt -q topic_Department_29 -i all ``` It computes the influences of <font face=sc> topic(Department, 29)</font> through Individual Conditional Expectation (ICE).

Run ```make``` to rebuild the project and run the study case above.


