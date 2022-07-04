@setlocal

@rem Redirect FFHacksterEx's stdout and stderr to a file.

@set ffhtrace=FFHacksterEx.exe-trace.log
@echo Launch FFHacksterEx.exe with logging redirected to a filename '%ffhtrace%'.
FFHacksterEx.exe > %ffhtrace% 2>&1

@endlocal