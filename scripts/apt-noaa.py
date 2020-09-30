import altiwx
import subprocess

altiwx.info("Processing NOAA APT data...")

output_file = altiwx.filename + ".png"

outflag = ""
if altiwx.southbound:
    outflag = "f"

command = "aptdec -e " + outflag + " '" + output_file + "' '" + altiwx.input_file + "'"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA APT data!")

#Written and tested by  Felix-OK9UWU
#aptdec software by Xerbo https://github.com/Xerbo/aptdec

# Discord
#exec(open("scripts/discord.py").read())

#direction = ""
#if altiwx.northbound:
#    direction = "Northbound"
#if altiwx.southbound:
#    direction = "Southbound"

#postToDiscord(altiwx.satellite_name + " " + altiwx.downlink_name, 65280, altiwx.elevation, direction, "AVHRR",  altiwx.date + " UTC", altiwx.frequency, output_file)

#altiwx.info("Webhook upload complete!")
