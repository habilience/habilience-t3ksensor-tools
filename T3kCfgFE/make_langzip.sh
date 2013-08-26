#!/bin/bash

if [ -e T3kCfgFE.rsc ]
then
   rm T3kCfgFE.rsc
fi
zip -P langzip -j T3kCfgFE.rsc ./Languages/*
if [ ! -d ./debug/ ]
then
   mkdir ./debug/
fi
if [ ! -d ./release/ ]
then
   mkdir ./release/
fi
if [ ! -d ./debug_x64/ ]
then
   mkdir ./debug_x64/
fi
if [ ! -d ./release_x64/ ]
then
   mkdir ./release_x64/
fi
cp T3kCfgFE.rsc ./debug/
cp T3kCfgFE.rsc ./release/
cp T3kCfgFE.rsc ./debug_x64/
cp T3kCfgFE.rsc ./release_x64/
rm T3kCfgFE.rsc
