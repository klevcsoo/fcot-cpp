# Film Colours Over Time - C++ Edition

**The stupid name hides a useless idea.**\
This C++ program converts a given video file into a picture, by taking a frame every *n* seconds, calculating the average colour of that frame, and adding a vertical line to the output picture with the given colour.\
It was inspired by another repository of mine, called [fcot-py](https://www.github.com/klevcsoo/fcot-py).
> The script uses [FFMpeg](https://www.ffmpeg.org/) to extract the frames, so it is required for it to work.

## Installation
You can either download the program from the [releases](https://www.github.com/klevcsoo/fcot-cpp/releases) page, or clone the repository with **`git clone https://github.com/klevcsoo/fcot-py.git`** and build the executable file from the source code yourself.

## Usage
From the command line, execute the file, with the first argument being the input video file and the second argument being the output image file.\
**`./fcot-cpp <input video file> <optional: output image file>`**

If the output file is left out, the output image file will be put into the same directory, that the input video file is located in. For example, **`./fcot-cpp ~/Movies/film.mp4`** will result in the output image file being located at *~/Movies/film.mp4.png*.

### No pull requests are accepted for this repository.
