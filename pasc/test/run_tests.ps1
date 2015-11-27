function Get-IniContent ($filePath)
{
    $ini = @{}
    switch -regex -file $FilePath
    {
        "^\[(.+)\]" # Section
        {
            $section = $matches[1]
            $ini[$section] = @{}
            $CommentCount = 0
        }
        "^(;.*)$" # Comment
        {
            $value = $matches[1]
            $CommentCount = $CommentCount + 1
            $name = "Comment" + $CommentCount
            $ini[$section][$name] = $value
        } 
        "(.+?)\s*=(.*)" # Key
        {
            $name,$value = $matches[1..2]
            $ini[$section][$name] = $value
        }
    }
    return $ini
}

function Out-IniFile($InputObject, $FilePath)
{
    $outFile = New-Item -ItemType file -Path $Filepath
    foreach ($i in $InputObject.keys)
    {
        if (!($($InputObject[$i].GetType().Name) -eq "Hashtable"))
        {
            #No Sections
            Add-Content -Path $outFile -Value "$i=$($InputObject[$i])"
        } else {
            #Sections
            Add-Content -Path $outFile -Value "[$i]"
            Foreach ($j in ($InputObject[$i].keys | Sort-Object))
            {
                if ($j -match "^Comment[\d]+") {
                    Add-Content -Path $outFile -Value "$($InputObject[$i][$j])"
                } else {
                    Add-Content -Path $outFile -Value "$j=$($InputObject[$i][$j])" 
                }

            }
            Add-Content -Path $outFile -Value ""
        }
    }
}

function Replase-Constant([string]$constName, [string]$constVal, [string]$str) {
    $cName = $constName
    $cVal = $constVal
    $nStr = $str
    $regex = "%([^:%]*)(:(.*?))*%"
    $Match = [regex]::Match($nStr, $regex)
    while ($Match.Success) {
        if ($Match.Groups[1].Value -eq $cName) {
            $repVal = $cVal
            if ($Match.Groups[3].Success) {
                $rep = $cVal -match $Match.Groups[3].Value
                if ($rep) { $repVal = $Matches[0] }
            }
            $nStr = $nStr -replace [regex]::Escape($Match.Groups[0].Value),$repVal
        }
        $Match = $Match.NextMatch()
    }
    return $nStr
}

function log([string]$file, [string]$str) {
    if (Test-Path $file) {
        Add-Content $file "$str"
    } else {
        Write-Output $str
    }
}

$iniFile = "run_tests.ini"
$ini = @{}
$ini["Program"] = @{}
$ini["Testing"] = @{}
$ini["Output"] = @{}
$ini["Program"]["path"] = ""
$ini["Program"]["params"] = ""
$ini["Testing"]["dirs"] = ".*"
$ini["Testing"]["inFormat"] = ""
$ini["Testing"]["outFormat"] = ""
$ini["Testing"]["checkFormat"] = ""
$ini["Output"]["fileName"] = ""

if (Test-Path $iniFile) {
    $ini = Get-IniContent $iniFile
}
else {
    Out-IniFile $ini $iniFile
}

$modif = ""
foreach ($arg in $args) {
    if ("$arg"[0] -eq "-") { $modif = $arg; continue; }
        switch ($modif) {
        "-dirs" {
            $ini["Testing"]["dirs"] = $arg
            $modif = ""
        }
        
    }
}

if ($ini["Output"]["fileName"] -eq "") { $ini["Output"]["fileName"] = "run_tests.out" }
$logFile = $PWD.Path+"\"+$ini["Output"]["fileName"]
if (Test-Path $logFile) { del $logFile; }
New-Item $logFile | Out-Null


$program = $PWD.Path+"\"+$ini["Program"]["path"]

if (Test-Path $program) {
    $dirs = dir -Directory -Name
    $dirs = $dirs -match $ini["Testing"]["dirs"]

    log $logFile "=================== START TESTING ===================" 

    foreach ($dir in $dirs) {
        if (Test-Path $dir) {
            cd $dir
            log $logFile "`t======= SECTION: $dir =======`n"
            $files = dir -File -Name
            $files = $files -match $ini["Testing"]["inFormat"]
            foreach ($file in $files) {
                if (Test-Path $file) {
                    $params = Replase-Constant "inFile" $file $ini["Program"]["params"]
                    $params = Replase-Constant "dir" $dir $params
                    $outFile = Replase-Constant "inFile" $file $ini["Testing"]["outFormat"]
                    $checkFile = Replase-Constant "inFile" $file $ini["Testing"]["checkFormat"]
                    & 'cmd.exe' "/c $program $params"
                    if (Test-Path $checkFile) {
                        if (Test-Path $outFile) {
                            if(Compare-Object -ReferenceObject $(Get-Content $outFile) -DifferenceObject $(Get-Content $checkFile)) {
                                log $logFile "`t`trun: $file `t`t`t`tFAIL"
                            } else {
                                log $logFile "`t`trun: $file `t`t`t`tOK"
                                del $outFile
                            }
                        }
                    } else {
                        log $logFile "`t`trun: $file `t`t`t`tFAIL NO CHECK FILE"
                    }
                }
            }
            log $logFile "`n`t===== END SECTION: $dir ====="
            cd ..
        }
    }

    log $logFile "==================== END TESTING ====================" 
}