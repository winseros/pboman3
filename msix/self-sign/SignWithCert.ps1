# this is a reminder script for myself of how to sign an individual MSIX file

param(
    [Parameter(Mandatory)][string]$MsixPath    
)

signtool.exe sign /fd SHA256 /n "PBO Manager" /s "My" $MsixPath
