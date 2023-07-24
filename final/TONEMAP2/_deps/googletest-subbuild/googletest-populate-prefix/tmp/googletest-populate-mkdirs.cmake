# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-src"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-build"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
