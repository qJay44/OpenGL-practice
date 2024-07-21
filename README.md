Source: https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-

Downloading resourses from tutorial 13 example:

```bat
git clone -n --depth=1 --filter=tree:0 https://github.com/VictorGordan/opengl-tutorials
cd opengl-tutorials
git sparse-checkout set --no-cone "Resources/YoutubeOpenGL 13 - Model Loading"
git checkout

```
