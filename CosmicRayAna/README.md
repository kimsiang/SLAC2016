# Introduction 
This is the source code for the SLAC cosmic ray analysis.

# Candidate selection

Candidate selection is done in several steps.

* Cluster energy and time

A list of clusters belonging to the beam and the laser is stored (list A).

* Crystal hit energy and time

Crystal hits that belong to the list A are skipped.
Crystal hits with energy between 20 to 500 pe are stored and the number of hits is stored (nCandidateHits).

* nCandidateHits

If nCandidateHits is betwen 4 and 30, dump information about this event.

# Candidates

Candidate number | Run number | Event number | Island number
---------------- | ---------- | ------------ | -------------
1 | 1792 | 1049 | 4
2 | 2159 | 1212 | 13
3 | 2167 | 656 | 0
4 | 2167 | 1411 | 58
5 | 2274 | 2988 | 39
6 | 2348 | 630 | 57
7 | 2688 | 1945 | 27
8 | 2867 | 96 | 7
9 | 2994 | 2363 | 64
10 | 3019 | 293 | 57
11 | 3071 | 937 | 33

# Plots

Plots corresponding to the candidates in the table above are stored under folder _plots_.
