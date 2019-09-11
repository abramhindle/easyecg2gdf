LICENSE
=======

This software is Free Software (GPLv2)

Copyright (C) 2000-2014 George B. Moody and Edna S. Moody
Copyright (C) 2017 Peter Lawrence
Copyright (C) 2019 Abram Hindle

Peter claims:

tool to convert Easy ECG device data into open biosig format 
============================================================

There is a consumer ECG monitor sold under various names like "Easy ECG", "PC-80B Color", and "Cardio B Palm".

It can collect 30 sec single-electrode ECG recordings, and a subset of the devices (with no consistent naming) have a "continuous mode" that can record up to 10 hours when using an external wiring harness.

The device connects to a PC using USB and appears as a Mass Storage device (often called a "flash drive" or "USB drive").

The manufacturer provides Windows software to view the recordings, but there is no means to export nor use the data under other operating systems.

THIS IS NOT A REPLACEMENT FOR THE MANUFACTURER'S SOFTWARE AND DOES NOT PROVIDE WAVEFORM VIEWING!

It does hopefully translate the data into a .GDF file in the hopes that others have or can provide tools to view this ECG data.

## Sample Usage

./easyecg2gdf /media/user/EASY\ ECG/

would result in all recordings found on the device (the specified directory) being written to the current directory as .GDF files, each with the date and time encoded into the filename.

Abram: But I did not like at all.

## Modifications

I ignore README.txt and just allow you to process individual scp files

./easyecg2gdf 1.SCP produces and .gdf file

I did a bad job, a hack job you might say.
