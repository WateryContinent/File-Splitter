cd "$(dirname "$0")"
clang++ -mmacosx-version-min=11.0 -std=c++17 main.cpp -o File\ Splitter `./wx-config --cxxflags --libs`
./create_app.command