This folder contains the HALF_ADDER DEVS model implemented in Cadmium

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt	

atomics [This folder contains atomic models implemented in Cadmium]
	andCAD.hpp
	xorCAD.hpp

data_structures [This folder contains message data structure used in the model]
	message.hpp
	message.cpp

test [This folder the unit test of the atomic models]
	and [This folder contains the unit test of the andCAD atomic model]
		main.cpp
		makefile
		andCAD_input_test_1.txt
		andCAD_input_test_2.txt
		andCAD_test_output.txt
	xor [This folder contains the unit test of the xorCAD atomic model]
		main.cpp
		makefile
		xorCAD_input_test_1.txt
		xorCAD_input_test_2.txt
		xorCAD_test_output.txt

vendor [This folder contains the time class and the model to generate the inputs to the DEVS model]
	iestream.hpp
	NDTime.hpp

top_model [This folder contains the HALF_ADDER top model]	
	main.cpp
	makefile
	half_adder_input_1.txt
	half_adder_input_2.txt
	half_adder_output.txt
	
/*************/
/****STEPS****/
/*************/

1 - Update include paths in all the makefiles in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
    
2 - Run the unit tests
	2.1. Run 'and' test
		1 - Open the terminal. Press in your keyboard Ctrl+Alt+t
		2 - Set the command prompt in the test/and folder. To do so, type in the terminal the path to this folder.
			Example: cd Documents/Half_Adder/test/and
		3 - To compile the test, type in the terminal:
			make clean; make all
		4 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this specific test you need to type:
			./and_TEST
		5 - To check the output of the test, open  "andCAD_test_output.txt"
	2.2. To run 'xor' test, the steps are analogous to 2.1
			
3 - Run the top model
	1 - Open the terminal. Press in your keyboard Ctrl+Alt+t
	2 - Set the command prompt in the top_model folder. To do so, type in the terminal the path to this folder.
		Example: cd ../../top_model
	3 - To compile the model, type in the terminal:
		make clean; make all
	4 - To run the model, type in the terminal "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
		./HALF_ADDER andCAD_input_test_1.txt andCAD_input_test_2.txt
	5 - To check the output of the model, open  "andCAD_test_output.txt"
	6 - To run the model with different inputs
		6.1. Create new .txt files with the same structure as andCAD_input_test_1.txt or andCAD_input_test_2.txt
		6.2. Run the model using the instructions in step 4
		6.3. If you want to keep the output, rename andCAD_test_output.txt. To do so, type in the terminal: "mv andCAD_test_output.txt NEW_NAME"
			Example: mv andCAD_test_output.txt andCAD_test_output_0.txt

