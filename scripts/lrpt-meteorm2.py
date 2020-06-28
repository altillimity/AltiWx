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

output_image = altiwx.filename + ".bmp"
command = "medet '" + lrpt_file + "' '" + altiwx.filename + "' -r 66 -g 65 -b 64 > /dev/null"
altiwx.debug(command)
subprocess.Popen([command], shell=1).wait()

output_file = altiwx.filename + ".png"

# Correct geometry. Using https://github.com/Xerbo/meteor_corrector
import re
import sys
from os.path import basename, splitext
from math import *
import numpy as np
import cv2

# Satellite paramaters
EARTH_RADIUS = 6371
SAT_HEIGHT = 820
SWATH = 2800
VIEW_ANGLE = SWATH / EARTH_RADIUS

def sat2earth_angle(radius, height, angle):
    '''
    Convert from the viewing angle from a point at (height) above a
    circle to internal angle from the center of the circle.
    See http://ceeserver.cee.cornell.edu/wdp2/cee6150/monograph/615_04_geomcorrect_rev01.pdf page 4.
    '''
    return asin((radius+height)/radius * sin(angle)) - angle

def earth2sat_angle(radius, height, angle):
    '''
    Oppsite of `sat2earth_angle`, convert from a internal angle
    of a circle to the viewing angle of a point at (height).
    '''
    return -atan(sin(angle)*radius / (cos(angle)*radius - (radius+height)))

# Load the image
src_img = cv2.imread(output_image)

# Gracefully handle a non-existent file
if src_img is None:
    raise FileNotFoundError("Could not open image")

# Get image diemensions
src_height, src_width = src_img.shape[:2]

# Calculate output size
edge_angle = sat2earth_angle(EARTH_RADIUS, SAT_HEIGHT, VIEW_ANGLE*2)  # Angle at edge of image
correction_factor = sat2earth_angle(EARTH_RADIUS, SAT_HEIGHT, 0.001)/0.001  # Change at nadir of image
out_width = int((edge_angle/correction_factor) * src_width)

sat_edge = earth2sat_angle(EARTH_RADIUS, SAT_HEIGHT, VIEW_ANGLE/2)

abs_corr = np.zeros(out_width)
for x in range(out_width):
    angle = ((x/out_width)-0.5)*VIEW_ANGLE
    angle = earth2sat_angle(EARTH_RADIUS, SAT_HEIGHT, angle)
    abs_corr[x] = (angle/sat_edge + 1)/2 * src_width

# Deform mesh
xs, ys = np.meshgrid(
    np.full(out_width, abs_corr, dtype=np.float32),
    np.arange(src_height, dtype=np.float32)
)

# Remap the image, with lanczos4 introplation
out_img = cv2.remap(src_img, xs, ys, cv2.INTER_LANCZOS4)

# Sharpen
amount = 0.3
radius = 5
out_img = cv2.addWeighted(out_img, amount+1, cv2.GaussianBlur(out_img, (0, 0), radius), -amount, 0)

# Rotate if that was a southbound pass
if altiwx.southbound:
    out_img = cv2.rotate(out_img, cv2.ROTATE_180)

# Write image
cv2.imwrite(output_file, out_img)

altiwx.info("Done processing METEOR LRPT data!")