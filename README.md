# Diabolika

Diabolika is a game clone of the original Diabolika 2 game.  
Diabolika uses DGEngine core - a 2D game engine.

![diabolika](https://user-images.githubusercontent.com/20025614/103456786-56155500-4cf1-11eb-8911-954eb57fc886.png)

You need the original game files (bgm, gfx, sfx) to be inside the gamefilesdia2 folder.  

If you want to have upscaled graphics, use an images upscaler ([xbrz](https://sourceforge.net/projects/xbrz/))
to save a copy of each image with 2x the resolution in gamefilesdia2/gfx2x and press `F1` to use the new scaled images.  

[Getting original assets](#Getting-original-assets)

## keys

Action                 | Key/Mouse button
---------------------- | ----------------------------
`Select Units 1-3`     | `1`, `2`, `3`
`Place Unit/Explode`   | `Space`, `Right Shift`, `Left Click`
`Move Hand`            | `Up`, `Down`, `Left`, `Right`
`Rotate`               | `R`, `Right Click`
`Load Game`            | `F8`
`Save Game`            | `F5`
`Fullscreen`           | `Alt + Enter`, `F11`
`Settings`             | `F1`

## Configuration

Press `F1` to change settings, apply shaders, select a different mod, etc..  

Unfifished games are automatically saved on exit.  

Current window position and size are automatically saved.

### Portable mode

To run in portable mode, create a folder `portable` next to the DGEngine binary (current working directory).  

On Linux, if using an `AppImage`, create a folder with the same name as the AppImage ending with `.home`.

```
Diabolika.AppImage         (file)
Diabolika.AppImage.home    (folder)
```

## Build

See how to build in [BUILD.txt](BUILD.txt).

### Getting original assets

Fork this repository, go to `Actions` and manually run the `Package_gamefiles` workflow.  
An artifact (`gamefilesdia2.zip`) will be created with the original assets.  
Extract that archive over the `gamefilesdia2` folder or delete the `gamefilesdia2` folder (Diabolika will use the zip instead).
