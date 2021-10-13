Despite this is a VS project, I don't intend to compile it via Visual Studio.

To compile, please use the commands:

candle.exe -dArtifactsFolder={where-the-artifcats-are} -dProductVersion="0.0.1" PBOManager.wxs
light.exe -ext WixUIExtension -dWixUILicenseRtf={path_to_license_file_in_RTF_format} .\PBOManager.wixobj

If you ever need to harvest the artifcats dir for the files, use:

heat dir {where-the-artifcats-are} -srd -sreg -gg -g1 -o {result-file-name}
