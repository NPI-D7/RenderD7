# RenderD7
RenderD7 is now LibRenderD7.
### Installation (Ubuntu)
first run this
`sudo su`
then this
```
curl -L https://github.com/NPI-D7/RenderD7/releases/download/v0.8.0-pre1/renderd7-0.8.0.tar.bz2 -o renderd7-0.8.0.tar.bz2
mkdir -p /opt/devkitpro/libctru
bzip2 -cd renderd7-0.8.0.tar.bz2 | tar -xf - -C /opt/devkitpro/libctru
```
Make sure that `-lrenderd7` is before `-lcitro2d`, `-lcitro3d`, `-lctru`.

# RenderD7 (https://npi-d7.github.io/RenderD7/)
Simple and Easey to use UI and Graphics helper.
Create DOCS
