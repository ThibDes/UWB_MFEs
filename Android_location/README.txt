Slave (anchor): Just change the slave number in main.h and the antenna delays if needed
PSoC_not_RTOS: Old version of the code working with RTOS anchors
PSoC: Current version of the code
Android: If the application keeps crashing, verify that the permissions are allowed
		settings -> Apps -> Manage apps -> IndoorNav -> App permissions -> Storage in Allowed
		The results of your tests are saved in a TestDir directory on the phone
		(currentf data collected : the distances in PSOCMaster.java, line 198)