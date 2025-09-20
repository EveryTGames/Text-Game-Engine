@echo off
chcp 65001 > nul
echo Building all .cpp files recursively...

powershell -Command "$files = Get-ChildItem -Recurse -Filter *.cpp | ForEach-Object { '\"' + $_.FullName + '\"' }; $cmd = 'g++ -fdiagnostics-color=always -g -IEngine -ISprites ' + ($files -join ' ') + ' -o main.exe'; iex $cmd"

