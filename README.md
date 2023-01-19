# How to run
## Input directories (used in study)
- Inclusive: `/nfs/slac/g/ldmx/data/validation/v12/4gev_{n}e_inclusive/pro_edge/`
- Signal: `/nfs/slac/g/ldmx/data/validation/v12/4gev_{n}e_signal/pro_edge/Ap{mass}GeV/`
- Ecal PN: `/nfs/slac/g/ldmx/data/validation/v12/4gev_{n}e_ecalPN/pro_edge/`

Where `{n}` is the electron multiplicity $1, 2, 3, 4$ and `{mass}` is the mass point of signal $0.001, 0.01, 0.1, 1$.
## Creating root files
#### Simulated electron number
1) `triggerSums.py` takes pre-processed root file inputs and uses Trigger Scintillator package functions to add the trigger sums for layers $0-20, 0-22, 0-24, ..., 0-34$.
    * Run batch files using `bsub.sh`, configure input and output directories in bash file.
    * `bsub_signal.sh` used for signal files due to extra mass point parameter.
2) `drawFromTree.C` extracts the trigger sum data calculated previously from the variable `fArray` in the root files.
    * Run batch files using `runthroughlist.sh`, configure input and output directories in bash file.
    * `runthroughlist.sh` takes `.txt` list inputs with the naming convention `list_{process}{n}e.txt`.
    * `runthroughlistsignal.sh` for signal files as above.
    * Outputs files in a folder with file name `{descriptor}_run{#}.root`
3) Use `hadd` to merge ROOT histogram files into one file: `hadd {descriptor}_allruns.root {descriptor}_run*.root`
    * This is the format that is accepted by the histogram plotting files.
#### Tracks
Step 2) differs from simulated electron number. Run `drawTracksFromTree.C` instead. `runthroughlist.sh` can be adapted easily to tracks by changing the input and output file names.
Files are denoted with suffix `_trk` to differentiate between other files.
## Plotting and analysis of data
**Notes: some input file names may have prefixes or suffixes (eg. `large`, `Layer0`). This is to differentiate between different versions of root files being run. File names/directories (input and output) may need to be changed in each file. Pass names can be different for each type of file.**
Run files with command `root -l -b -q '{fileName}.C+({input1}, {input2}, ..., {inputn})'`
* `cumulativeAllProcesses.C` plots cumulative histogram of signal, inclusive and ecal PN files for an input electron multiplicity, and outputs a `.tex` file of energy thresholds, signal rates and errors.
* `poissonReweighting.C` For input process (ecal PN/inclusive) and variance, will reweight track distributions according to the Poisson distribution and multiplicity. Track histograms from file multiplicities 1-4e are compiled in output file `{process}_allruns_{variance}poisson.root`.
* `poissonReweightingSignal.C` same as above but for signal files only and additional input mass point.
* `cumulativeTracks.C` plots cumulative Poisson reweighted histograms for an input track number and variance.
* `cumulativeTracksSignal.C` same as above but for signal files only and additional input mass point.
