#this is the script for adding a self-signed version of an MSIX into a ZIP archive with an unsigned MSIX

param(
    [Parameter(Mandatory)][string]$ZipArchivePath
)

if (-not (Test-Path $ZipArchivePath)) {
    Write-Host ("The file '{0}' does not exist" -f $ZipArchivePath)
    Exit 1
}

$Temp = Join-Path ([System.IO.Path]::GetTempPath()) (New-Guid).Guid
Write-Host ("Extracting the archive to: {0}" -f $Temp)
Expand-Archive -Path $ZipArchivePath -DestinationPath $Temp

$Items = Get-ChildItem -Path $Temp -Filter "*.msix"
if ($Items.Count -ne 1) {
    Write-Host ("Expected the extracted archive contain a single MSIX file, but found {0} files" -f $Items.Count)
    Exit 1
}

$MsixItem = $Items[0]
$SignedCopyName = $MsixItem.BaseName + ".SIGNED" + $MsixItem.Extension
$SignedCopyPath = Join-Path $Temp $SignedCopyName

Copy-Item -Path $MsixItem.FullName -Destination $SignedCopyPath

./SignWithCert.ps1 -MsixPath $SignedCopyPath

Copy-Item -Path "./PBO Manager Trust Cert.cer" -Destination $Temp
Copy-Item -Path ./README.txt -Destination $Temp

Compress-Archive -Path (Join-Path $Temp "*") -DestinationPath $ZipArchivePath -CompressionLevel Optimal -Update
