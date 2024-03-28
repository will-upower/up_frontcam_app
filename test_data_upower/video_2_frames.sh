mkdir -p Test_Images
ffmpeg -i input.avi -vf fps=30 Test_Images/output_%04d.png
