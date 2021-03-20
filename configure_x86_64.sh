#!/bin/bash
conan install \
  --profile ./conan_profiles/conanprofile_x86_64 \
  conanfile.txt \
  -if Build_x86_64 \
  --build=missing

cmake -BBuild_x86_64 .

ln -s ./Build_x86_64/compile_commands.json compile_commands.json
