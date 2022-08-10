To run the code:

                                    PARTONE - ASSEMBLER 

1. Open windows powershell with environment g++ 9.2.0
2. Execute the command "g++ 190101023_Assembler.cpp" with the input file present in same folder with name input.txt
3. Execute "./a.exe"

Generated Files:
middle.txt : After running Pass1 the intermediate file generated with this name
output.txt : Listing file after running Pass2
object.o : Final output object file
objectwithcaret.txt : Final output object file with caretsfor better readability.


                                    PARTTWO - Linker-Loader 

1. Open windows powershell with environment g++ 9.2.0
2. Execute the command "g++ 190101023_linker-loader.cpp" with the input file present in same folder with name linker_loader_input.txt
3. Execute "./a.exe"

Generated Files:
Start Address is initilaised to 0x4000 at start of pass1.
linker_loader_output.txt : Final program after Linking and Loading.