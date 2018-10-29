20180221 1. Amiccom M8017 RF7217 Repeater Mode one picture backup.
         2. Add 1 second and 500ms mode. only update not yet final.

20180224 1. Reply Tag Rx status to server on server update status.

20180302 1. Modify project Name.
         2. Settup Rx Gateway Beacon Sync OK.
	    send Beacon to Tag sometime overflow 0.01 second.

20180308 1.Add Receive from Raspberrypi Tag update Date. can receive 3 picture date.
           send to Tag and reply Tag update status OK. but unstable.
         2.Define repeater status format. for receive update data done,
           reply repeater status to gateway.

20180313 1.Debug Sync Beacon Timer. and add combine Tag update result reply gateway.
         2.have lost data receive update Tag data run long timer. and lost beacon.


20180322 1.Backup for 20180321 Demo.
	 2.Solve the lost beacon problem
 	 3.Organize source code
	 4.define version v0.0.0.3

20180329 1.Debug updates more Tag timeout lost beacon.
         2.Modify repeater Rx mode runtime 525ms, TX mode 475ms.
           RX early 25ms receive beacon.
	   Increase time delay interrupt trigger before Timer0 interrupt trigger
	 3.define version v0.0.0.6

20180403 1.Add Repeater default setting write to dataflash.
         2.Debug lost Beacon bug and add Tx mode early end timer.
	 3.Add Repeater RTC function and wtrite standbytimer to dataflash.
	 4.define version v0.0.0.7

20180502 Modify Tag update error or success result, all combine reply to gateway v0.0.0.10 .

20180717 Add Dual RF M8107 main RF, A7127 second RF 500ms mode, and Debug Dual RF no 500ms transparent noise issue v0.0.0.14.

20180802 1. Add Dual RF A7127 main RF, M8107 second RF 500ms mode.
	 2. Debug Timer1 value over 65535 issue. add receive packet timeout not beacon state,
	    other state Timer1 value is less than 40. state set 0.

20180911 modify repeater architecture version 2, streamline global variable, and use struct pointer pass value.
         only main A8107 one RF 500ms final modify.

20181016 1. Backup git phase2 vsersion 0.1.0, only M8017_500msOneRFMode & M8107_A7127_TransDualRFMode.
