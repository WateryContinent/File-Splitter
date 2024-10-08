# File Splitter
Small tool I've made to split files down into set size chunks for uploading parts through a service that may require smaller files.

*If you have any ideas/issues with the software please let me know*

## Steps to compile (MacOS)

1. Download the "MacOS" folder from the source.
2. Make sure you have xcode installed _*WITH*_ command line tools. (for clang++) If you have xcode install and try and run the script, it will automatically install the commmand line tools for you
3. Download WxWidgets from their website [here](https://www.wxwidgets.org/downloads/), make sure you get the *__"Source for Linux, MacOS etc"__*
4. Download CMAKE, if not downloaded, follow the tutorial in the next step.
5. Compile WxWidgets, if you are unsure as to how, [this](https://www.youtube.com/watch?v=BBv3FkFcPwI) tutorial is great.
6. Once compiled, __*copy*__ the alias of "wx-config" to the root of the MacOS folder from this repo that you downloaded.
7. You may have to open the terminal and type "chmod +x " then drag the build.command and run it and repeat with the create_app.command.
8. Double click on the "build" script and wait till completion.
9. Run your freshly compiled application!

## Steps to compile (Windows)
TODO: Compile scripts to make the building process easier and write the compliation steps.
