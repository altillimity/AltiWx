# AltiWx

Yet another automated satellite station software... With additional features such as the ability to receive multiples downlink at once with a single SDR, if it fits in the bandwidth!

**Note 1: This is still considered Beta, but it does not mean I do not encourage using it yet! This should work fine already, just don't expect a bug-free experience yet... If you do use it, please report bugs since I make sure everything works myself :)**

**Note 2: This currently only supports RTL-SDR, support for other SDRs is planned but I am still deciding how I will do it.**

# What is this?

So... What is this? This is a software I've decided to make after experimenting around with several automated satellite station solutions, mostly aimed at the NOAA APT and METEOR LRPT transmissions on VHF. I originally used software that wrapped around rtl_fm and other tools such as sox, predict, some APT decoder, etc to automatically record and decode passes. It worked great but something was bothering me, mostly the fact this same RTL-SDR has 2.4Mhz of usable bandwidth... So in theory the whole VHF weather band can fit right? And that's how the idea started.

AltiWx is an automated satellite station recording software built from the ground up for that purpose, with integrated DSP, pass prediction and processing mechanism. Recording is done by monitoring a whole portion of the spectrum, and by some digital signal processing, isolate each signal from this whole sampled spectrum to allow recording as many satellites (or downlinks) in parrallel as you want as long as long as they fit in the bandwidth of your SDR!  
Obviously, this brought up a few issues such as how to properly handle passes of satellites that do not fit in the bandwidth... In the end it will prioritize better passes (or satellite priorities set in the configuration file) per-band as doable.

In fact, AltiWx will only ever tune the SDR to some predefined frequency bands in the configuration, as an example let's take 137.5Mhz and 145Mhz at 2.4Mhz bandwidth.
With this SDR configuration, you could have NOAA 19, which has both APT on 137.1Mhz and DSB on 137.77Mhz. 

```
  - norad: 33591 # NOAA 19
    min_elevation: 10
    priority: 1
    downlinks:
      - name: APT
        frequency: 137100000
        bandwidth: 42000
        doppler: false
        post_processing_script: apt-noaa.py
        output_extension: wav
        type: FM
        parameters:
          audio_samplerate: 48000
      - name: DSB
        frequency: 137770000
        bandwidth: 48000
        doppler: false
        post_processing_script: none
        output_extension: raw
        type: IQ
```

Once a pass occurs, AltiWx will create a Modem that will be removed at the end of pass (a Modem being an unit that will be fed the part of the spectrum correponding to a downlink and process it, would it be FM demodulation or just writing baseband) for each downlink, each is assigned a thread so the only limit to how many you can have is your processing power. 

If you have multiples satellites and they all fit on the same band, no passes will ever be skipped assuming your machine can keep up with the processing, otherwise, passes will be skipped on a priority basis as described earlier to allow the SDR to retune over to the other band... Where of course you can also have as many downlinks as you like :)

FInally, the processing is done by simple python scripts that can be set in the configuration. I settled on this way to keep things modular, and python allows doing pretty much anything without any hard-coded processing in the software.

# Installation

### Generic system
In general, the installation procedure consists in installing all the dependencies enumerated below in some way, and building AltiWx via CMake as usual.

```
git clone https://github.com/altillimity/AltiWx.git
cd Altiwx
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

### Raspbian / Debian installation guide

Since I would expect most installations to be on devices such as Raspberry PIs or other kind of common Debian systems, here's a guide covering those.

About Raspberry PIs, the software should work fine on anything over a 3B, and definitely on a Pi 4, but this has not been "officially" confirmed. The Pi 4 should handle everything down to live QPSK Demodulation fine. Obviously, this will highly depend on your load, but the above examples assume a station tracking a few satellites (eg: METEOR, NOAA, ISS) which would be expected to be a common usecase. 

```
# Install build tools and dependencies present in the repo
sudo apt install libspdlog-dev librtlsdr-dev libfmt-dev pybind11-dev python3-dev libliquid-dev cmake build-essential git libvolk1-dev

# Build and install libpredict
git clone https://github.com/la1k/libpredict
cd libpredict
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j3
sudo make install
cd ../..
rm -rf libpredict

# Build and install libdsp
git clone https://github.com/altillimity/libdsp
cd libdsp
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j3
sudo make install
cd ../..
rm -rf libdsp

# Build and install yaml-cpp
git clone https://github.com/jbeder/yaml-cpp
cd yaml-cpp
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j3
sudo make install
cd ../..
rm -rf yaml-cpp

# Finally, build AltiWx
git clone https://github.com/altillimity/AltiWx.git
cd AltiWx
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j3
```

### Configuration

Once you got AltiWx built, you need to setup a few other things.

In the folder you will be running AltiWx, you need to have a config.yml file (template in this repository).

The general concept of the configuration file is the following, I won't go into details explaining it all since I (truly) hope the comments are good enough :

```
# How should this station be called?
station_name: My Station

# Location used for pass prediction, altitude in meters
station:
  latitude: 0.0
  longitude: 0.0
  altitude: 0

# How often should TLEs be checked for updates against Celestrak?
tle_update: 0 0 * * *

# Where to store collected data? (Eg, recordings, etc)
data_directory: data

# How detailed should logging be?
# Available : trace, debug, info, warn, error, critical, off
# In production you should at least keep info, but debug can provide some valuable informations
log_level: debug

# Configure your SDR here, with some optional settings such as PPM correction
# The sample rate and frequency are in Hz, and the sample rate determines how much band you will be able to cover
# The gain is an INTEGER! Do not use floats (eg, 33.8), this is not supported and will cause the config to fail
radio:
  frequencies:
    - 137500000
    - 145000000
  samplerate: 1000000
  gain: 49
  # ppm_correction: 0

# All satellites you want to track
satellites:
  - norad: 40069 # METEOR-M 2
    min_elevation: 10
    priority: 1
    downlinks:
      - name: LRPT
        frequency: 137100000
        bandwidth: 140000
        doppler: false
        post_processing_script: lrpt-meteorm2.py
        output_extension: soft
        type: QPSK
        parameters:
          agc_rate: 0.1
          symbolrate: 72000
          rrc_alpha: 0.6
          rrc_taps: 31
          costas_bw: 0.005
          iq_invert: true
  - norad: 33591 # NOAA 19
    min_elevation: 10
    priority: 1
    downlinks:
      - name: APT
        frequency: 137100000
        bandwidth: 42000
        doppler: false
        post_processing_script: apt-noaa.py
        output_extension: wav
        type: FM
        parameters:
          audio_samplerate: 48000
      - name: DSB
        frequency: 137770000
        bandwidth: 48000
        doppler: false
        post_processing_script: dsb-noaa.py
        output_extension: raw
        type: NOAA_DSB
  - norad: 25544 # ISS
    min_elevation: 20
    priority: 1
    downlinks:
      - name: SSTV
        frequency: 145800000
        bandwidth: 14000
        doppler: true
        post_processing_script: none
        output_extension: wav
        type: FM
        parameters:
          audio_samplerate: 48000
```

### Modems

Doppler tracking can be enabled in the downlink configuration.

**IQ**  
This modem is a simple I/Q recorder that will output raw float32 samples to a file.   

No parameters

**FM**  
This modem is a standard FM demodulator, of an arbitrary audio output rate that will output a 16-bit PCM wav file.  

Parameters :
- audio_samplerate, output audio samplerate in Hz

**QPSK**  
This modem is a generic QPSK Demodulator, that will output soft symbols. This can be used to live-demodulate things like METEOR-M 2 LRPT. It is multi-threaded and mostly meant for low-rate signals.  

Parameters :
- agc_rate, AGC update rate
- symbolrate, symbolrate of the signal being demodulated
- rrc_alpha, Root-Raised Cosine filter alpha
- rrc_taps, tap count of the RRC filter, tuning this will highly impact the processing power required
- costas_bw, PLL bandwidth
- iq_invert, optional (default disabled), to invert I/Q soft symbols in the output file

### Scripts
Scripts must be in a "scripts" folder in the folder you will be running AltiWx in, then, they can be set for each downlink using their filename. Obviously, some samples are included in the repository.

Miniminal example :
```
import altiwx

altiwx.info("Processing Downlink data...")

altiwx.info("Done processing Downlink data!")
```

Data from AltiWx is passed through the altiwx module, which exposes the following variables and functions :
- altiwx.input_file, path of the recorded file
- altiwx.filename, path and name of the current file but with no extension, if you want to reuse the same filename for something else
- altiwx.satellite_name, name of the satellite the data is from
- altiwx.samplerate, samplerate of this downlink, mostly if you need to process a baseband
- altiwx.frequency, frequency of this downlink
- altiwx.northbound, will be true of the pass was northbound
- altiwx.southbound, will be true of the pass was southbound
- altiwx.elevation, peak elevation of this pass
- altiwx.sun_elevation, sun elevation angle at the moment of the pass

## Systemd service

A systemd service is way to automatically start and restart Altiwx after a crash or a reboot. Listed below is a sample service that is placed as `/etc/systemd/system/altiwx.service` in the system. 

```
[Unit]
Description=Altiwx automated satellite station
After=network.target
StartLimitIntervalSec=0
[Service]
Type=simple
Restart=always
RestartSec=1
User=pi
# fill in the path to the Altiwx executable
ExecStart=/home/pi/AltiWx/AltiWx
# fill in the directory where the Altiwx executable is located in
WorkingDirectory=/home/pi/AltiWx/

[Install]
WantedBy=multi-user.target
```

## Dependencies

AltiWx will require as dependencies :
- [spdlog](https://github.com/gabime/spdlog)
- [librtlsdr](https://git.osmocom.org/rtl-sdr)
- [liquid-dsp](https://github.com/jgaeddert/liquid-dsp)
- [libpredict](https://github.com/la1k/libpredict)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [libdsp](https://github.com/altillimity/libdsp)
- [pybind11](https://github.com/pybind/pybind11)
- [fmt](https://github.com/fmtlib/fmt) 
- [volk](https://github.com/gnuradio/volk) 

But also uses the following libraries, included in the repository :
- [tinywav](https://github.com/mhroth/tinywav)
- [scheduler](https://github.com/Bosma/Scheduler)
- [ctpl](https://github.com/vit-vit/ctpl)
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
