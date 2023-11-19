import altiwx
import subprocess


altiwx.info("Processing NOAA APT data...")

command = "satdump noaa_apt audio_wav '" + altiwx.input_file + "' '" + altiwx.filename + "' --satellite_number " + altiwx.satellite_name.replace('NOAA ', '')
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA APT data!")
