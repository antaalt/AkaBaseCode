# Aka base code

## Description

This is a base code for [aka framework](https://github.com/antaalt/Aka/) to quickly prototype some rendering code which is also used as a game engine framework (and maybe someday a game...).

## Preparation

You will need to init your repo as a git repo with 

`git init`

Then create a main.cpp such as in our example with a class extending `aka::Application` & a CMakeLists.txt such as in our example.

Don't forget a [gitignore](https://www.toptal.com/developers/gitignore/api/cmake,vs) with CMake default build target (out).

Then you need to add Aka framework as dependency:

`git submodule add git@github.com:antaalt/Aka.git lib/Aka`

The path here is important because some code expect aka to be in lib/Aka. Should be changed int the future. Run `git init-submodule update --init --recursive` to download all dependencies, then checkout the `vulkan-backend` branch of aka for latest changes & update submodules.

## Create !

Enjoy your prototyping now !