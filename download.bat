git clone -n --depth=1 --filter=tree:0 https://github.com/VictorGordan/opengl-tutorials
cd opengl-tutorials
git sparse-checkout set --no-cone "Resources/YoutubeOpenGL 15 - Stencil Buffer"
git checkout
