# Frame Interleaver Script
## Compilation Requirements :
- g++ compiler
- python3 installed
- opencv-python
- ffmpeg installed
## Compilation command :
```
g++ pattern.cpp -o pattern.exe
```

## Run command :
```
./pattern.exe <lpi of sheet> <frames to interleave> <offset of first back/white bar pattern>
```

## Output :
The output is stored in file as pattern_<frame_no.>_<lpi>_<frames>_<offset>.png
