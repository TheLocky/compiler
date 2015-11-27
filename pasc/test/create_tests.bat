@echo off             
for /l %%i in (%1,1,%2) do (
	type nul > test%%i.in
)
pause