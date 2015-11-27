@echo off
setlocal EnableDelayedExpansion 
set /A j=0
for /l %%i in (%1,1,%2) do (
	set /A j=%%i+%3
	ren test%%i.in test!j!.in
)