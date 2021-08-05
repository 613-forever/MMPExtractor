# MMP Extractor

An extractor for resources from disk image for a game, MMP for short.

Note that paths to all external tools should be set manually in scripts.

Short and ASCII-only directory name are preferred. (Long path might be cumbersome in some operating systems, and non-ASCII characters might cause weird encoding-related troubles.)

CRs `\r` may break the bash files. It is recommended that git `autocrlf` be disabled for this repo.

## Tools Required

+ Specialized unpacker (https://github.com/613-forever/CriPakTools-specialized-for-mmp).
+ `QuickBMS` (http://aluigi.altervista.org/quickbms.htm) is needed to unpack `PAC`s.
+ A `GIM`-to-`PNG` convertor, e.g. `GIM2PNG`(In documentation, http://www.geocities.jp/junk2ool/ , currently seems to be https://junk2ool.net/tools/psx/start) is required.
+ Other stand-alone tools (this repo). Used to extract `GIM`s from `CSB` files, handle 16-bit index `GIM` image, and assemble image parts into one image, respectively.

## Steps
1. Note that the script files is written for msys2, but it might also work in other *nix-like shells, such as WSL. (I do not suggest running on *nix just because I do not know C# language well and can not prove the CRI unpacker can work.)
1. Mount the ISO, and an `archive.cpk` and an `install.cpk` lie in `/PSP_GAME/USRDIR`. Copy them into a directory `some_directory`.
1. Unpack them with this specialized unpacker extractor. That is, run `CriPakTool.exe xxx.cpk ALL`.
1. Move `some_directory/install/CharaBustup` into a newly-created `pac` directory, for the files need assembling. Run `extract.sh` to extract and assemble them.
1. Run extracting script `extract2.sh` to extract `GIM` images and convert them into `PNG`s. A file named `error_files.txt` will be generated if some files are not successfully handled. (`GIM2PNG` can not handle 16-bit index images perfectly.)
1. Run `extract3.sh` in the directory where `error_files.txt` is generated.

## Tree
``` plain
somedirectory
|-- archive.cpk (copied)
|-- archive
|   |-- ... (recursively unpacked)
|-- archive_png
|   |-- ... (converted from GIMs in "archive")
|
|-- install.cpk (copied)
|-- install
|   |-- ... (recursively unpacked)
|-- install_png
|   |-- ... (converted from GIMs in "install")
|
|-- pac
|   |-- talkchara00_0a.pac (moved from install/CharaBustup)
|   |-- ...
|
|-- extracted_raw (unpacked from PACs in "pac")
|   |-- talkchara00_0a
|   |   |-- GIMDATA.DAT
|   |   |-- POSDATA.DAT
|   |   |-- TALKCHARA00_0A.GIM
|   |   |-- TALKCHARA00_0A.POS
|   |   |-- TALKCHARA00_0A_00_E0.GIM
|   |   |-- ...
|   |-- ...
|
|-- extracted_png (converted from GIMs in "extracted_raw")
|   |-- talkchara00_0a
|   |   |-- TALKCHARA00_0A.png
|   |   |-- TALKCHARA00_0A.POS (copied for further use)
|   |   |-- TALKCHARA00_0A_00_E0.png
|   |   |-- ...
|   |-- ...
|
|-- extracted_png (converted from GIMs in "extracted_raw")
    |-- talkchara00_0a
    |   |-- TALKCHARA00_0A_00.png (assembled from xx_0a.png, xx_0a_00_e0.png, xx_0a_00_m0.png)
    |   |-- ...
    |-- ...
```
