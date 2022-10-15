# argument 1: electron multiplicity, argument 2: process (inclusive, signal, PN), argument 3: script being run
# $ . runthroughlist.sh [n_electrons] [process] [running_script]
if [ -z $1 ] 
then
    mult=1
else
    mult=$1
fi

if [ -z $2 ]
then
    process="inclusive"
else
    process="$2"
fi

if [ -z $3 ]
then
    configToRun="drawFromTree.C"
else
    configToRun="$3"
fi

fileList=list_$2$1e.txt
echo "Running mergefiles script with $mult electrons, $2 process, $3"


outDir="${HOME}/multiElectronTriggering/4gev_triggersums/$2_$1e_trk" # CHANGE FOR TRACK AND NOT TRACK

if [ ! -d ${outDir} ]
then
    echo "Creating output directory ${outDir}"
    mkdir -p ${outDir}
fi

let iterator=1
for fileName in $(cat $fileList) ;
    do root -l -b -q ''$configToRun'+("'$fileName'","'$mult'","'$outDir/$2$1e_run$iterator.root'")';
    ((iterator++))
done