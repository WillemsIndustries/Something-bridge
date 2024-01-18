// remote id
#define REMOTE1 0xa535d2
#define REMOTE2 0xb15a3a
#define REMOTE3 0xe828d0
#define REMOTE4 0x620416
#define REMOTE5 0x6e668f
// set rolling code storage location
EEPROMRollingCodeStorage rollingCodeStorage1(0);
EEPROMRollingCodeStorage rollingCodeStorage2(2);
EEPROMRollingCodeStorage rollingCodeStorage3(4);
EEPROMRollingCodeStorage rollingCodeStorage4(6);
EEPROMRollingCodeStorage rollingCodeStorage5(8);
// create the remotes
SomfyRemote somfyRemote1(rts_data_pin, REMOTE1, &rollingCodeStorage1);
SomfyRemote somfyRemote2(rts_data_pin, REMOTE2, &rollingCodeStorage2);
SomfyRemote somfyRemote3(rts_data_pin, REMOTE3, &rollingCodeStorage3);
SomfyRemote somfyRemote4(rts_data_pin, REMOTE4, &rollingCodeStorage4);
SomfyRemote somfyRemote5(rts_data_pin, REMOTE5, &rollingCodeStorage5);

// function for sending RTS command
// "remote" is the number of the remote you want to use (1 to 5)
// "command" is the comand you want to send (up, dowm, my, prog)
void shutter(int remote, String command) {
	const Command rts_command = getSomfyCommand(command);
	 	if (remote == 1) {
     	somfyRemote1.sendCommand(rts_command);
		}
		else if(remote == 2) {
     	somfyRemote2.sendCommand(rts_command);
		}
		else if(remote == 3) {
		somfyRemote3.sendCommand(rts_command);
		}
		else if(remote == 4) {
		somfyRemote4.sendCommand(rts_command);
		}
		else if(remote == 5) {
		somfyRemote5.sendCommand(rts_command);
		}
		if(command != "prog") {
			digitalWrite(pair_led, HIGH);
			delay(20);
			digitalWrite(pair_led, LOW);
		}
	}
