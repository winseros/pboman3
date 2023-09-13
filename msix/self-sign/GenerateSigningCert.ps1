# this is a reminder script for myself of how to generate a self signed certificate

$UserName = "Nikita Kobzev"
$YearsValid = 20
$CertStoreLocationMy = "Cert:\CurrentUser\My\"

function MakeCCertSubject {
    param (
        [Parameter(Mandatory)][string]$UserName
    )
    return "CN=PBO Manager, O=" + $UserName
}

function CreateNewSelfSignedCertificate {    
    param (
        [Parameter(Mandatory)][string]$UserName,
        [Parameter(Mandatory)][int]$YearsValid
    )

    $Subject = MakeCCertSubject -UserName $UserName
  
    $Cert = New-SelfSignedCertificate -Type Custom -Subject $Subject `
        -CertStoreLocation $CertStoreLocationMy `
        -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13") `
        -KeyExportPolicy Exportable `
        -NotAfter (Get-Date).AddYears($YearsValid)

    return $Cert
}

$Subject = MakeCCertSubject -UserName $UserName
$Cert = CreateNewSelfSignedCertificate -UserName $UserName -YearsValid $YearsValid
Write-Host $Cert
