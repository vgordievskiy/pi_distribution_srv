#!/bin/bash
conan install \
  --profile ./conan_profiles/conanprofile_arm_v8 \
  conanfile.txt \
  -if Build_armV8 \
  --build=missing

cmake \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++-9 \
  -BBuild_armV8 .
