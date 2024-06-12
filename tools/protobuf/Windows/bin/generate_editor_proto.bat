@rem generate proto buffer source files.
@del /Q ..\..\source

@set input_path=..\..\proto\editor
@set output_path=..\..\source

for /r "%input_path%" %%f in (*.proto) do (
  protoc --proto_path="%input_path%" --cpp_out="%output_path%" %%~nxf
)


@rem copy all generated files to target directory.
@set launcher_dir=..\..\..\..\source\Editor\TinyLauncher\proto\
@set editor_dir=..\..\..\..\source\Editor\TinyEditor\proto\

@del /Q %launcher_dir%
@xcopy /Y ..\..\source\*.pb.* %launcher_dir%
@del /Q %editor_dir%
@xcopy /Y ..\..\source\*.pb.* %editor_dir%
@pause
