This folder contains the Building Controller model implemented in ECadmium


For complete explanation of the model and sample inputs and outputs refer the document "Model explanation.pdf".
	
/*************/
/****STEPS****/
/*************/

1 - Update include paths in all the makefiles in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
    
2 - Run the top model
	1 - Open the terminal. Press in your keyboard Ctrl+Alt+t
	2 - Set the command prompt in the top_model folder. To do so, type in the terminal the path to this folder.
		Example: cd ../../top_model
	3 - To compile the model, type in the terminal:
		make clean; make all
	4 - To run the model, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this test you need to type: ./SUHA_BOT_TOP
	5 - To check the output of the model, open "outputs" under top_model and check all the text files.
	6 - To run the model with in embedded mode (real time):
		6.1. Connect the controller and open the terminal in the location of top_model.
		6.2. Run the model using the instructions: make clean, make embedded, make flash
		6.3. Give 10 seconds for it to flash.
