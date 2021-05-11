import altiwx
import subprocess
import os
from datetime import datetime


# enhencements that should be calculated
enh = ["HVCT", "MSA", "HVC", "HVC", "HVC-precip",
       "HVCT", "HVCT-precip", "MCIR", "MCIR-precip"]

# define tmporary directory to use while calculating - mind the slash at the end
tempdir = "/var/ramfs/"

# creating a dateobject from the filename
datetime_object = datetime.strptime(os.path.split(
    os.path.abspath(altiwx.input_file))[1], '%Y%m%dT%H%M%SZ')

output_file = os.path.split(os.path.abspath(altiwx.input_file))[1]

command = "cp '"+altiwx.input_file+".wav' " + tempdir

altiwx.info("Processing NOAA APT data...(extended version)")

altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()


wavfile = tempdir+output_file + ".wav"


outflag = "-N "
if altiwx.southbound:
    outflag = "-S "

command = "wxmap -T '"+altiwx.satellite_name+"' -G /home/pi/.wxtoimg '" + \
    datetime_object.strftime("%H:%M:%S") + "' "+tempdir+output_file+"_map.png"


altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

mapfile = tempdir+output_file+"_map.png"
outflag = outflag + "-M '"+mapfile+"'"


for enhencement in enh:
    command = "wxtoimg " + outflag + " -e " + enhencement + outflag + " '" + \
        altiwx.input_file + "' " + tempdir + output_file + "_"+enhencement+".png"
    altiwx.debug(command)
    subprocess.Popen([command], shell=1).wait()
altiwx.info("Done processing NOAA APT data!")
