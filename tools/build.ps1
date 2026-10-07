param(
    [string]$IdePath = 'C:\ST\STM32CubeIDE_2.2.0\STM32CubeIDE',
    [string]$Workspace = ''
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path -Parent $PSScriptRoot
if (!$Workspace) { $Workspace = Join-Path $taskRoot 'build\ide-workspace' }
$projectPath = Join-Path $taskRoot 'Rover_UART_GPIO'
$projectUri = ([System.Uri]($projectPath + '\')).AbsoluteUri
$ideExecutable = Join-Path $IdePath 'stm32cubeidec.exe'
if (!(Test-Path -LiteralPath $ideExecutable)) { throw "STM32CubeIDE bulunamadi: $ideExecutable" }
& $ideExecutable --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data $Workspace -import $projectUri -build 'Rover_UART_GPIO/Debug'
if ($LASTEXITCODE -ne 0) { throw "Derleme basarisiz: $LASTEXITCODE" }
