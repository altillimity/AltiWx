import altiwx
import subprocess

altiwx.info("Processing NOAA APT data...")

output_file = altiwx.filename + ".png"

outflag = ""
if altiwx.southbound:
    outflag = "-ef"

command = "aptdec " + outflag + " -o '" + output_file + "' '" + altiwx.input_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA APT data!")
