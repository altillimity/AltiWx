import altiwx
import subprocess
import os

altiwx.info("Processing METEOR LRPT data...")

temp_file = altiwx.filename + ".wav"
command = "sox -t raw -e floating-point -b 32 -c 2 -r " + str(altiwx.samplerate) + " '" + altiwx.input_file + "' -t wav -e signed-integer -b 16 -c 2 -r " + str(altiwx.samplerate) + " '" + temp_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

lrpt_file = altiwx.filename + ".lrpt"
command = "meteor_demod -q -B -s " + str(altiwx.samplerate) + " '" + temp_file + "' -o '" + lrpt_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

os.remove(temp_file)

output_file = altiwx.filename + ".bmp"
command = "medet '" + lrpt_file + "' '" + altiwx.filename + "'  -r 65 -g 65 -b 64 > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

corrected_file = altiwx.filename + "_corrected.png"
command = "meteor_corrector -o '" + corrected_file + "' '" + output_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

corrected_file_png = altiwx.filename + ".png"
rotation = ""
if altiwx.northbound:
	rotation = "-rotate 180"

command = "convert " + rotation + " '" + corrected_file + "' '" + corrected_file_png + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR LRPT data!")
os.remove(altiwx.input_file) 
altiwx.info("IQ File Deleted")

#Written and tested by Felix-OK9UWU 

# Discord
#exec(open("scripts/discord.py").read(#))

#direction = ""
#if altiwx.northbound:
#   direction = "Northbound"
#if altiwx.southbound:
#   direction = "Southbound"

p#ostToDiscord(altiwx.satellite_name + " " + altiwx.downlink_name, 16711783, altiwx.elevation, direction, "MSU-MR",  altiwx.date + " UTC", altiwx.frequency, corrected_file)
