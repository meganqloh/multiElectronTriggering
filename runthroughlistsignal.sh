# argument 1: electron multiplicity, argument 2: process (inclusive, signal, PN), argument 3: script being run
# $ . runthroughlist.sh [n_electrons] [process] [mass point] [running_script]
if [ -z $1 ] 
then
    mult=1
else
    mult=$1
fi

if [ -z $2 ]
then
    process="signal"
else
    process="$2"
fi

if [ -z $3 ]
then
    massPoint="0.001"
else
    massPoint="$3"
fi

if [ -z $4 ]
then
    configToRun="drawFromTree.C"
else
    configToRun="$4"
fi

fileList=list_$2$1eAp$3GeV.txt
echo "Running mergefiles script with $mult electrons, $2 process, mass point $3, $4"

outDir="${HOME}/multiElectronTriggering/4gev_triggersums/$2_$1eAp$3GeV_trk"

if [ ! -d ${outDir} ]
then
    echo "Creating output directory ${outDir}"
    mkdir -p ${outDir}
fi

let iterator=1
for fileName in $(cat $fileList) ;
    do root -l -b -q ''$configToRun'+("'$fileName'","'$mult'","'$outDir/$2$1e_run$iterator.root'")' ;
    ((iterator++))
done