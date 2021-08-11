import altiwx
import subprocess

altiwx.info("Processing NOAA APT data...")

output_file = altiwx.filename + ".png"

outflag = ""
if altiwx.southbound:
    outflag = "-R yes"

command = f"noaa-apt {altiwx.input_file} -o {output_file}  {outflag}"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA APT data!")
