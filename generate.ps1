if(!(Test-Path .\bin))
{
    New-Item .\bin -type directory
}

if(!(Test-Path .\bin\Debug))
{
    New-Item .\bin\Debug -type directory
}

if(!(Test-Path .\bin\Release))
{
    New-Item .\bin\Release -type directory
}

if(!(Test-Path .\msc150_64))
{
    New-Item .\msc150_64 -type directory
}

cd .\msc150_64
cmake -G "Visual Studio 15 2017 Win64" ..\