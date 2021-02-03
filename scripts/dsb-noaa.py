import altiwx
import subprocess
import os

altiwx.info("Processing NOAA DSB data...")

command = "(cd '" + os.path.split(os.path.abspath(altiwx.input_file))[0] + "' && NOAA-HIRS-Decoder '" + altiwx.input_file + "' dsb)"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

altiwx.info("Done processing NOAA DSB data!")
