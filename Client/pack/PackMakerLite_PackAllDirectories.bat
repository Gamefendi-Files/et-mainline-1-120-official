@echo off
for /D %%i in (*.*) do (
	PackMakerLite_not_p.bat %%i
)
pause
