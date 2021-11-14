# PLOP
**[A fully-featured Falling-Sand game powered by WebAssembly!](https://caltrop.dev/plop/)**

[![haii~ i hope u like tha thing :\]](https://caltrop.dev/plop/logo.png)](https://caltrop.dev/plop/)

# Background
## What?
PLOP follows a tradition of games called "Falling-Sand games". A host of independently developed and published games, usually distributed for free in the form of Web-Apps. They allow the player to paint different elements onto the playing field and lets them discover interesting interactions and emerging behavior. Creative and imaginative interplay with the different systems of the sandbox make up the gameplay, instead of story progression or other external motivators.

## Mechanics
PLOP makes use of two intersecting systems: a cellular-automaton and a fluid simulation. Elements are portrayed as pixels, or "cells", on a grid. Each cell interacts with its direct neighbors based on their types and produces unique behavior. Fire will set flammable material like wood ablaze. Hot cells will evaporate water, creating steam that rises to the top of the canvas, only to later rain down again. Supplementary to the cellular-automaton, a fluid simulation approximates wind and allows temperature changes to propagate.

# Building

Required Dev-Dependencies:
- clang
- uglifyjs

```
$ git clone https://github.com/Caltrop256/plop
```

```
$ cd plop
```

```
$ make
```

# License

This project is licensed under the [GNU General Public License v3.0](https://github.com/Caltrop256/plop/blob/master/LICENSE). You may copy, distribute, and modify the software as long as you track changes/dates in the source files. Any modifications to this project must be made available under the GPL along with build & install instructions.