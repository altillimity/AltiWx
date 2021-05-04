import altiwx
import subprocess
import os

altiwx.info("Processing METEOR-M 2 LRPT data...")

output_file = altiwx.filename + ".bmp"
output_file = output_file.replace(' ', '\ ')
# workingdir = os.path.split(os.path.abspath(altiwx.input_file))[0]
command = "medet '" + altiwx.input_file + \
    "' '" + altiwx.filename + "' -cd > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing METEOR-M 2 LRPT data!")

decoded_file = altiwx.filename + ".dec"
if os.path.isfile(decoded_file):
    altiwx.info("File exist with decoded data - continue processing!")
    command = "medet '" + altiwx.input_file + ".dec' '" + \
        altiwx.filename + "' -d -cn > /dev/null"
    altiwx.debug(command)
    subprocess.Popen([command], shell=1).wait()
    altiwx.info("Images created.")
    for bmpfile in os.listdir(workingdir):
        if bmpfile.endswith(".bmp"):
            command = "convert " + bmpfile + " " + \
                bmpfile.replace(".bmp", ".png")
            altiwx.debug(command)
            subprocess.Popen([command], shell=1).wait()
    for pngfile in os.listdir(workingdir):
        if pngfile.endswith(".png"):
            command = "mv " + pngfile + " " + \
                pmgfile.split(
                    '.')[0] + altiwx.satellite_name.replace(' ', '_') + ".png"
            altiwx.debug(command)
            subprocess.Popen([command], shell=1).wait()

else:
    altiwx.info("File not exist")
