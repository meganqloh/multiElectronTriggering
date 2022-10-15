#!/bin/python

import os
import sys
import json

from LDMX.Framework import ldmxcfg

# set a 'pass name'; avoid sim or reco(n) as they are apparently overused
# "rereco" for 1e, "overlay" for 2e-4e, "v12" for ecalPN
inputPassName="overlay"
passName = "triggerSums"
p=ldmxcfg.Process(passName)

#import all processors

# Ecal hardwired/geometry stuff
import LDMX.Ecal.EcalGeometry
import LDMX.Ecal.ecal_hardcoded_conditions

# Hcal hardwired/geometry stuff
import LDMX.Hcal.HcalGeometry
import LDMX.Hcal.hcal_hardcoded_conditions

from LDMX.Recon.electronCounter import ElectronCounter
from LDMX.Recon.simpleTrigger import TriggerProcessor

from LDMX.TrigScint.trigScint import TrigScintDigiProducer
from LDMX.TrigScint.trigScint import TrigScintClusterProducer
from LDMX.TrigScint.trigScint import trigScintTrack



#pull in command line options
infile= sys.argv[1]        # input file name
nEle=int(sys.argv[2])      # simulated beam electrons
outputNameString= str(sys.argv[3]) #sample identifier
outDir= str(sys.argv[4])    #sample identifier

outname=outDir+"/triggerSumsLayer0_"+outputNameString #+".root"
print("Outname is"+outname)


#if use a file list 
#with open(fileList) as inputFiles :
#     p.inputFiles = [ line.strip('\n') for line in inputFiles.readlines() ]
p.inputFiles = [ infile ] 
     
print( p.inputFiles )

#
# Configure the sequence in which user actions should be called.
#

eCount = ElectronCounter( nEle, "ElectronCounter")
eCount.input_pass_name = inputPassName
eCount.use_simulated_electron_number = True

p.sequence=[ eCount ]

overlayStr="Overlay"
                                                            
tsDigisTag  =TrigScintDigiProducer.tagger()
tsDigisUp  =TrigScintDigiProducer.up()
tsDigisDown  =TrigScintDigiProducer.down()

#make sure to pick up the right pass 
tsDigisTag.input_pass_name = inputPassName
tsDigisTag.input_collection = tsDigisTag.input_collection+overlayStr
tsDigisUp.input_pass_name = tsDigisTag.input_pass_name
tsDigisUp.input_collection = tsDigisUp.input_collection+overlayStr
tsDigisDown.input_pass_name = tsDigisTag.input_pass_name
tsDigisDown.input_collection = tsDigisDown.input_collection+overlayStr

tsClustersTag  = TrigScintClusterProducer.tagger()
tsClustersUp  = TrigScintClusterProducer.up()
tsClustersDown  =TrigScintClusterProducer.down()

p.sequence.extend([tsDigisTag, tsDigisUp, tsDigisDown])

#make sure to pick up the right pass 
tsClustersTag.input_pass_name = "triggerSums"
tsClustersTag.input_collection = tsDigisTag.output_collection
tsClustersUp.input_pass_name = tsClustersTag.input_pass_name
tsClustersUp.input_collection = tsDigisUp.output_collection
tsClustersDown.input_pass_name = tsClustersTag.input_pass_name
tsClustersDown.input_collection = tsDigisDown.output_collection

trigScintTrack.input_pass_name = passName

p.sequence.extend( [tsClustersTag, tsClustersUp, tsClustersDown, trigScintTrack] )

layers = [20, 22, 24, 26, 28, 30, 32, 34]
tList=[]
for iLayer in range(len(layers)) : 
#     print("at layer iterator  "+str(iLayer)+" and layer nb "+str(layers[iLayer]))
     tp = TriggerProcessor("TriggerSumsLayer"+str(layers[iLayer]))
     tp.input_pass = inputPassName
     tp.start_layer = 0
     tp.end_layer = layers[iLayer]
     tp.trigger_collection = "TriggerSums"+str(layers[iLayer])+"Layers"
     tList.append(tp)
#     p.sequence.extend( [tp] )
p.sequence.extend( tList ) #=[ ecalVeto ] #TrigScintClusterProducer.tagger(), TrigScintClusterProducer.up(), TrigScintClusterProducer.down(), trigScintTrack ]# ecalVeto ]

print( tList[0].trigger_collection )

p.keep = ["drop .*SimParticles", "drop .*SimHits", "drop .*Hcal.*", "keep .*Trig.*", "drop HcalVeto", "keep .*Ecal.*", "drop .*ScoringPlaneHits.*"]

# p.maxEvents = 100

p.outputFiles=[ outname ]


p.termLogLevel = 1  # default is 2 (WARNING); but then logFrequency is ignored. level 1 = INFO.
#print this many events to stdout (independent on number of events, edge case: round-off effects when not divisible. so can go up by a factor 2 or so)
logEvents=20 
if p.maxEvents < logEvents :
     logEvents = p.maxEvents
p.logFrequency = int( p.maxEvents/logEvents )

json.dumps(p.parameterDump(), indent=2)

with open('parameterDump.json', 'w') as outfile:
     json.dump(p.parameterDump(),  outfile, indent=4)


     
