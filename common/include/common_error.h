/* common_error.h  : Davicom common error header.                               */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: common_error.h                                                    */
/* Purpose      : ESL scanner device header for ESL system                      */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    : https://msdn.microsoft.com/en-us/library/windows/desktop/     */
/*              : ms681382(v=vs.85).aspx                                        */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
// https://msdn.microsoft.com/zh-tw/library/windows/desktop/ms681381(v=vs.85).aspx
#ifndef _DAVICOM_COMMON_ERROR_
#define _DAVICOM_COMMON_ERROR_


//
//  Note: There is a slightly modified layout for HRESULT values below,
//        after the heading "COM Error Codes".
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_XPS                     82
#define FACILITY_WINRM                   51
#define FACILITY_WINDOWSUPDATE           36
#define FACILITY_WINDOWS_DEFENDER        80
#define FACILITY_WINDOWS_CE              24
#define FACILITY_WINDOWS                 8
#define FACILITY_USERMODE_VOLMGR         56
#define FACILITY_USERMODE_VIRTUALIZATION 55
#define FACILITY_USERMODE_VHD            58
#define FACILITY_URT                     19
#define FACILITY_UMI                     22
#define FACILITY_UI                      42
#define FACILITY_TPM_SOFTWARE            41
#define FACILITY_TPM_SERVICES            40
#define FACILITY_SXS                     23
#define FACILITY_STORAGE                 3
#define FACILITY_STATE_MANAGEMENT        34
#define FACILITY_SSPI                    9
#define FACILITY_SCARD                   16
#define FACILITY_SHELL                   39
#define FACILITY_SETUPAPI                15
#define FACILITY_SECURITY                9
#define FACILITY_SDIAG                   60
#define FACILITY_RPC                     1
#define FACILITY_RAS                     83
#define FACILITY_PLA                     48
#define FACILITY_OPC                     81
#define FACILITY_WIN32                   7
#define FACILITY_CONTROL                 10
#define FACILITY_WEBSERVICES             61
#define FACILITY_NULL                    0
#define FACILITY_NDIS                    52
#define FACILITY_METADIRECTORY           35
#define FACILITY_MSMQ                    14
#define FACILITY_MEDIASERVER             13
#define FACILITY_MBN                     84
#define FACILITY_INTERNET                12
#define FACILITY_ITF                     4
#define FACILITY_USERMODE_HYPERVISOR     53
#define FACILITY_HTTP                    25
#define FACILITY_GRAPHICS                38
#define FACILITY_FWP                     50
#define FACILITY_FVE                     49
#define FACILITY_USERMODE_FILTER_MANAGER 31
#define FACILITY_DPLAY                   21
#define FACILITY_DISPATCH                2
#define FACILITY_DIRECTORYSERVICE        37
#define FACILITY_CONFIGURATION           33
#define FACILITY_COMPLUS                 17
#define FACILITY_USERMODE_COMMONLOG      26
#define FACILITY_CMI                     54
#define FACILITY_CERT                    11
#define FACILITY_BCD                     57
#define FACILITY_BACKGROUNDCOPY          32
#define FACILITY_ACS                     20
#define FACILITY_AAF                     18


//
// Define the severity codes
//


//
// MessageId: ERROR_SUCCESS
//
// MessageText:
//
// The operation completed successfully.
//
//#define ERROR_SUCCESS                    			0L
#define NO_ERROR 0L                                                 // dderror
#define SEC_E_OK                         			((HRESULT)0x00000000L)
#define ERROR_SUCCESS								0 	//(0x00) The operation completed successfully.

//
// MessageId: ERROR_INVALID_FUNCTION
//
// MessageText:
//
// Incorrect function.
//

#define ERROR_INVALID_FUNCTION						1 	//(0x01) Incorrect function.
#define ERROR_FILE_NOT_FOUND						2 	//(0x02) The system cannot find the file specified.
#define ERROR_PATH_NOT_FOUND						3 	//(0x03) The system cannot find the path specified.
#define ERROR_TOO_MANY_OPEN_FILES 					4 	//(0x04) The system cannot open the file.
#define ERROR_ACCESS_DENIED 						5 	//(0x05) Access is denied.
#define ERROR_INVALID_HANDLE						6 	//(0x06) The handle is invalid.
#define ERROR_ARENA_TRASHED							7 	//(0x07) The storage control blocks were destroyed.
#define ERROR_NOT_ENOUGH_MEMORY						8 	//(0x08) Not enough storage is available to process this command.
#define ERROR_INVALID_BLOCK							9 	//(0x09) The storage control block address is invalid.
#define ERROR_BAD_ENVIRONMENT						10 	//(0x0A) The environment is incorrect.
#define ERROR_BAD_FORMAT							11 	//(0x0B) An attempt was made to load a program with an incorrect format.
#define ERROR_INVALID_ACCESS						12 	//(0x0C) The access code is invalid.
#define ERROR_INVALID_DATA							13 	//(0x0D) The data is invalid.
#define ERROR_OUTOFMEMORY							14	//(0x0E) Not enough storage is available to complete this operation.
#define ERROR_INVALID_DRIVE							15 	//(0x0F) The system cannot find the drive specified.
#define ERROR_CURRENT_DIRECTORY						16 	//(0x10) The directory cannot be removed.
#define ERROR_NOT_SAME_DEVICE						17 	//(0x11) The system cannot move the file to a different disk drive.
#define ERROR_NO_MORE_FILES							18  //(0x12) There are no more files.
#define ERROR_WRITE_PROTECT							19  //(0x13) The media is write protected.
#define ERROR_BAD_UNIT								20  //(0x14) The system cannot find the device specified.
#define ERROR_NOT_READY								21  //(0x15) The device is not ready.
#define ERROR_BAD_COMMAND							22  //(0x16) The device does not recognize the command.
#define ERROR_CRC									23  //(0x17) Data error (cyclic redundancy check).
#define ERROR_BAD_LENGTH							24  //(0x18) The program issued a command but the command length is incorrect.
#define ERROR_SEEK									25  //(0x19) The drive cannot locate a specific area or track on the disk.
#define ERROR_NOT_DOS_DISK							26 	//(0x1A) The specified disk or diskette cannot be accessed.
#define ERROR_SECTOR_NOT_FOUND						27  //(0x1B) The drive cannot find the sector requested.
#define ERROR_OUT_OF_PAPER							28  //(0x1C) The printer is out of paper.
#define ERROR_WRITE_FAULT							29 	//(0x1D) The system cannot write to the specified device.
#define ERROR_READ_FAULT							30  //(0x1E) The system cannot read from the specified device.
#define ERROR_GEN_FAILURE							31  //(0x1F) A device attached to the system is not functioning.
#define ERROR_SHARING_VIOLATION						32  //(0x20) The process cannot access the file because it is being used by another process.
#define ERROR_LOCK_VIOLATION						33  //(0x21) The process cannot access the file because another process has locked a portion of the file.
#define ERROR_WRONG_DISK							34  //(0x22) The wrong diskette is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.
#define ERROR_SHARING_BUFFER_EXCEEDED				36  //(0x24) Too many files opened for sharing.
#define ERROR_HANDLE_EOF							38  //(0x26) Reached the end of the file.
#define ERROR_HANDLE_DISK_FULL						39  //(0x27) The disk is full.

#define ERROR_NOT_SUPPORTED							50  //(0x32) The request is not supported.
#define ERROR_REM_NOT_LIST							51  //(0x33) Windows cannot find the network path. Verify that the network path is correct and the destination computer is not busy or turned off. If Windows still cannot find the network path, contact your network administrator.
#define ERROR_DUP_NAME								52  //(0x34) You were not connected because a duplicate name exists on the network. If joining a domain, go to System in Control Panel to change the computer name and try again. If joining a workgroup, choose another workgroup name.
#define ERROR_BAD_NETPATH							53  //(0x35) The network path was not found.
#define ERROR_NETWORK_BUSY							54  //(0x36) The network is busy.
#define ERROR_DEV_NOT_EXIST							55  //(0x37) The specified network resource or device is no longer available.
#define ERROR_TOO_MANY_CMDS							56  //(0x38) The network BIOS command limit has been reached.
#define ERROR_ADAP_HDW_ERR							57  //(0x39) A network adapter hardware error occurred.
#define ERROR_BAD_NET_RESP							58  //(0x3A) The specified server cannot perform the requested operation.
#define ERROR_UNEXP_NET_ERR							59  //(0x3B) An unexpected network error occurred.
#define ERROR_BAD_REM_ADAP							60  //(0x3C) The remote adapter is not compatible.
#define ERROR_PRINTQ_FULL							61  //(0x3D) The printer queue is full.
#define ERROR_NO_SPOOL_SPACE						62  //(0x3E) Space to store the file waiting to be printed is not available on the server.
#define ERROR_PRINT_CANCELLED						63  //(0x3F) Your file waiting to be printed was deleted.
#define ERROR_NETNAME_DELETED						64  //(0x40) The specified network name is no longer available.
#define ERROR_NETWORK_ACCESS_DENIED					65  //(0x41) Network access is denied.
#define ERROR_BAD_DEV_TYPE							66  //(0x42) The network resource type is not correct.
#define ERROR_BAD_NET_NAME							67  //(0x43) The network name cannot be found.
#define ERROR_TOO_MANY_NAMES						68  //(0x44) The name limit for the local computer network adapter card was exceeded.
#define ERROR_TOO_MANY_SESS							69  //(0x45) The network BIOS session limit was exceeded.
#define ERROR_SHARING_PAUSED						70  //(0x46) The remote server has been paused or is in the process of being started.
#define ERROR_REQ_NOT_ACCEP							71  //(0x47) No more connections can be made to this remote computer at this time because there are already as many connections as the computer can accept.
#define ERROR_REDIR_PAUSED							72  //(0x48) The specified printer or disk device has been paused.
#define ERROR_FILE_EXISTS							80  //(0x50) The file exists.
#define ERROR_CANNOT_MAKE							82  //(0x52) The directory or file cannot be created.
#define ERROR_FAIL_I24								83  //(0x53) Fail on INT 24.
#define ERROR_OUT_OF_STRUCTURES						84  //(0x54) Storage to process this request is not available.
#define ERROR_ALREADY_ASSIGNED						85  //(0x55) The local device name is already in use.
#define ERROR_INVALID_PASSWORD						86  //(0x56) The specified network password is not correct.
#define ERROR_INVALID_PARAMETER						87  //(0x57) The parameter is incorrect.
#define ERROR_NET_WRITE_FAULT						88  //(0x58) A write fault occurred on the network.
#define ERROR_NO_PROC_SLOTS							89  //(0x59) The system cannot start another process at this time.

#define ERROR_TOO_MANY_SEMAPHORES					100 //(0x64) Cannot create another system semaphore.
#define ERROR_EXCL_SEM_ALREADY_OWNED				101 //(0x65) The exclusive semaphore is owned by another process.
#define ERROR_SEM_IS_SET							102 //(0x66) The semaphore is set and cannot be closed.
#define ERROR_TOO_MANY_SEM_REQUESTS					103 //(0x67) The semaphore cannot be set again.
#define ERROR_INVALID_AT_INTERRUPT_TIME				104 //(0x68) Cannot request exclusive semaphores at interrupt time.
#define ERROR_SEM_OWNER_DIED						105 //(0x69) The previous ownership of this semaphore has ended.
#define ERROR_SEM_USER_LIMIT						106 //(0x6A) Insert the diskette for drive %1.
#define ERROR_DISK_CHANGE							107 //(0x6B) The program stopped because an alternate diskette was not inserted.
#define ERROR_DRIVE_LOCKED							108 //(0x6C) The disk is in use or locked by another process.
#define ERROR_BROKEN_PIPE							109 //(0x6D) The pipe has been ended.
#define ERROR_OPEN_FAILED							110 //(0x6E) The system cannot open the device or file specified.
#define ERROR_BUFFER_OVERFLOW						111 //(0x6F) The file name is too long.
#define ERROR_DISK_FULL								112 //(0x70) There is not enough space on the disk.
#define ERROR_NO_MORE_SEARCH_HANDLES				113 //(0x71) No more internal file identifiers available.
#define ERROR_INVALID_TARGET_HANDLE					114 //(0x72) The target internal file identifier is incorrect.
#define ERROR_INVALID_CATEGORY						117 //(0x75) The IOCTL call made by the application program is not correct.
#define ERROR_INVALID_VERIFY_SWITCH					118 //(0x76) The verify-on-write switch parameter value is not correct.
#define ERROR_BAD_DRIVER_LEVEL						119 //(0x77) The system does not support the command requested.
#define ERROR_CALL_NOT_IMPLEMENTED					120 //(0x78) This function is not supported on this system.
#define ERROR_SEM_TIMEOUT							121 //(0x79) The semaphore timeout period has expired.
#define ERROR_INSUFFICIENT_BUFFER					122 //(0x7A) The data area passed to a system call is too small.

#define ERROR_INVALID_NAME							123 //(0x7B) The filename, directory name, or volume label syntax is incorrect.
#define ERROR_INVALID_LEVEL							124 //(0x7C) The system call level is not correct.
#define ERROR_NO_VOLUME_LABEL						125 //(0x7D) The disk has no volume label.
#define ERROR_MOD_NOT_FOUND							126 //(0x7E) The specified module could not be found.
#define ERROR_PROC_NOT_FOUND						127 //(0x7F) The specified procedure could not be found.
#define ERROR_WAIT_NO_CHILDREN						128 //(0x80) There are no child processes to wait for.
#define ERROR_CHILD_NOT_COMPLETE					129 //(0x81) The %1 application cannot be run in Win32 mode.
#define ERROR_DIRECT_ACCESS_HANDLE					130 //(0x82) Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.
#define ERROR_NEGATIVE_SEEK							131 //(0x83) An attempt was made to move the file pointer before the beginning of the file.
#define ERROR_SEEK_ON_DEVICE						132 //(0x84) The file pointer cannot be set on the specified device or file.
#define ERROR_IS_JOIN_TARGET						133 //(0x85) A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.
#define ERROR_IS_JOINED								134 //(0x86) An attempt was made to use a JOIN or SUBST command on a drive that has already been joined.
#define ERROR_IS_SUBSTED							135 //(0x87) An attempt was made to use a JOIN or SUBST command on a drive that has already been substituted.
#define ERROR_NOT_JOINED							136 //(0x88) The system tried to delete the JOIN of a drive that is not joined.
#define ERROR_NOT_SUBSTED							137 //(0x89) The system tried to delete the substitution of a drive that is not substituted.
#define ERROR_JOIN_TO_JOIN							138 //(0x8A) The system tried to join a drive to a directory on a joined drive.
#define ERROR_SUBST_TO_SUBST						139 //(0x8B) The system tried to substitute a drive to a directory on a substituted drive.
#define ERROR_JOIN_TO_SUBST							140 //(0x8C) The system tried to join a drive to a directory on a substituted drive.
#define ERROR_SUBST_TO_JOIN							141 //(0x8D) The system tried to SUBST a drive to a directory on a joined drive.
#define ERROR_BUSY_DRIVE							142 //(0x8E) The system cannot perform a JOIN or SUBST at this time.
#define ERROR_SAME_DRIVE							143 //(0x8F) The system cannot join or substitute a drive to or for a directory on the same drive.
#define ERROR_DIR_NOT_ROOT							144 //(0x90) The directory is not a subdirectory of the root directory.
#define ERROR_DIR_NOT_EMPTY							145 //(0x91) The directory is not empty.
#define ERROR_IS_SUBST_PATH							146 //(0x92) The path specified is being used in a substitute.
#define ERROR_IS_JOIN_PATH							147 //(0x93) Not enough resources are available to process this command.
#define ERROR_PATH_BUSY								148 //(0x94) The path specified cannot be used at this time.
#define ERROR_IS_SUBST_TARGET						149 //(0x95) An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.
#define ERROR_SYSTEM_TRACE							150 //(0x96) System trace information was not specified in your CONFIG.SYS file, or tracing is disallowed.
#define ERROR_INVALID_EVENT_COUNT					151 //(0x97) The number of specified semaphore events for DosMuxSemWait is not correct.
#define ERROR_TOO_MANY_MUXWAITERS					152 //(0x98) DosMuxSemWait did not execute; too many semaphores are already set.
#define ERROR_INVALID_LIST_FORMAT					153 //(0x99) The DosMuxSemWait list is not correct.
#define ERROR_LABEL_TOO_LONG						154 //(0x9A) The volume label you entered exceeds the label character limit of the target file system.
#define ERROR_TOO_MANY_TCBS							155 //(0x9B) Cannot create another thread.
#define ERROR_SIGNAL_REFUSED						156 //(0x9C) The recipient process has refused the signal.
#define ERROR_DISCARDED								157 //(0x9D) The segment is already discarded and cannot be locked.
#define ERROR_NOT_LOCKED							158 //(0x9E) The segment is already unlocked.
#define ERROR_BAD_THREADID_ADDR						159 //(0x9F) The address for the thread ID is not correct.
#define ERROR_BAD_ARGUMENTS							160 //(0xA0) One or more arguments are not correct.
#define ERROR_BAD_PATHNAME							161 //(0xA1) The specified path is invalid.
#define ERROR_SIGNAL_PENDING						162 //(0xA2) A signal is already pending.
#define ERROR_MAX_THRDS_REACHED						164 //(0xA4) No more threads can be created in the system.
#define ERROR_LOCK_FAILED							167 //(0xA7) Unable to lock a region of a file.
#define ERROR_BUSY									170 //(0xAA) The requested resource is in use.
#define ERROR_DEVICE_SUPPORT_IN_PROGRESS			171 //(0xAB) Device's command support detection is in progress.
#define ERROR_CANCEL_VIOLATION						173 //(0xAD) A lock request was not outstanding for the supplied cancel region.
#define ERROR_ATOMIC_LOCKS_NOT_SUPPORTED			174 //(0xAE) The file system does not support atomic changes to the lock type.
#define ERROR_INVALID_SEGMENT_NUMBER				180 //(0xB4) The system detected a segment number that was not correct.
#define ERROR_INVALID_ORDINAL						182 //(0xB6) The operating system cannot run %1.
#define ERROR_ALREADY_EXISTS						183 //(0xB7) Cannot create a file when that file already exists.
#define ERROR_INVALID_FLAG_NUMBER					186 //(0xBA) The flag passed is not correct.
#define ERROR_SEM_NOT_FOUND							187 //(0xBB) The specified system semaphore name was not found.
#define ERROR_INVALID_STARTING_CODESEG				188 //(0xBC) The operating system cannot run %1.
#define ERROR_INVALID_STACKSEG						189 //(0xBD) The operating system cannot run %1.
#define ERROR_INVALID_MODULETYPE					190 //(0xBE) The operating system cannot run %1.
#define ERROR_INVALID_EXE_SIGNATURE					191 //(0xBF) Cannot run %1 in Win32 mode.
#define ERROR_EXE_MARKED_INVALID					192 //(0xC0) The operating system cannot run %1.
#define ERROR_BAD_EXE_FORMAT						193 //(0xC1) %1 is not a valid Win32 application.
#define ERROR_ITERATED_DATA_EXCEEDS_64k				194 //(0xC2) The operating system cannot run %1.
#define ERROR_INVALID_MINALLOCSIZE					195 //(0xC3) The operating system cannot run %1.
#define ERROR DYNLINK_FROM_INVALID_RING				196 //(0xC4) The operating system cannot run this application program.
#define ERROR_IOPL_NOT_ENABLED						197 //(0xC5) The operating system is not presently configured to run this application.
#define ERROR_INVALID_SEGDPL						198 //(0xC6) The operating system cannot run %1.
#define ERROR_AUTODATASEG_EXCEEDS_64k				199 //(0xC7) The operating system cannot run this application program.
#define ERROR_RING2SEG_MUST_BE_MOVABLE				200 //(0xC8) The code segment cannot be greater than or equal to 64K.
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM				201 //(0xC9) The operating system cannot run %1.
#define ERROR_INFLOOP_IN_RELOC_CHAIN				202 //(0xCA) The operating system cannot run %1.
#define ERROR_ENVVAR_NOT_FOUND						203 //(0xCB) The system could not find the environment option that was entered.
#define ERROR_NO_SIGNAL_SENT						205 //(0xCD) No process in the command subtree has a signal handler.
#define ERROR_FILENAME_EXCED_RANGE					206 //(0xCE) The filename or extension is too long.
#define ERROR_RING2_STACK_IN_USE					207 //(0xCF) The ring 2 stack is in use.
#define ERROR_META_EXPANSION_TOO_LONG				208 //(0xD0) The global filename characters, * or ?, are entered incorrectly or too many global filename characters are specified.
#define ERROR_INVALID_SIGNAL_NUMBER					209 //(0xD1) The signal being posted is not correct.
#define ERROR_THREAD_1_INACTIVE						210 //(0xD2) The signal handler cannot be set.
#define ERROR_LOCKED								212 //(0xD4) The segment is locked and cannot be reallocated.
#define ERROR_TOO_MANY_MODULES						214 //(0xD6) Too many dynamic-link modules are attached to this program or dynamic-link module.
#define ERROR_NESTING_NOT_ALLOWED					215 //(0xD7) Cannot nest calls to LoadModule.
#define ERROR_EXE_MACHINE_TYPE_MISMATCH				216 //(0xD8) This version of %1 is not compatible with the version of Windows you're running. Check your computer's system information and then contact the software publisher.
#define ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY		217 //(0xD9) The image file %1 is signed, unable to modify.
#define ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY	218 //(0xDA) The image file %1 is strong signed, unable to modify.
#define ERROR_FILE_CHECKED_OUT						220 //(0xDC) This file is checked out or locked for editing by another user.
#define ERROR_CHECKOUT_REQUIRED						221 //(0xDD) The file must be checked out before saving changes.
#define ERROR_BAD_FILE_TYPE							222 //(0xDE) The file type being saved or retrieved has been blocked.
#define ERROR_FILE_TOO_LARGE						223 //(0xDF) The file size exceeds the limit allowed and cannot be saved.
#define ERROR_FORMS_AUTH_REQUIRED					224 //(0xE0) Access Denied. Before opening files in this location, you must first add the web site to your trusted sites list, browse to the web site, and select the option to login automatically.
#define ERROR_VIRUS_INFECTED						225 //(0xE1) Operation did not complete successfully because the file contains a virus or potentially unwanted software.
#define ERROR_VIRUS_DELETED							226 //(0xE2) This file contains a virus or potentially unwanted software and cannot be opened. Due to the nature of this virus or potentially unwanted software, the file has been removed from this location.
#define ERROR_PIPE_LOCAL							229 //(0xE5) The pipe is local.
#define ERROR_BAD_PIPE								230 //(0xE6) The pipe state is invalid.
#define ERROR_PIPE_BUSY								231 //(0xE7) All pipe instances are busy.
#define ERROR_NO_DATA								232 //(0xE8) The pipe is being closed.
#define ERROR_PIPE_NOT_CONNECTED					233 //(0xE9) No process is on the other end of the pipe.
#define ERROR_MORE_DATA								234 //(0xEA) More data is available.
#define ERROR_VC_DISCONNECTED						240 //(0xF0) The session was canceled.
#define ERROR_INVALID_EA_NAME						254 //(0xFE) The specified extended attribute name was invalid.
#define ERROR_EA_LIST_INCONSISTENT					255 //(0xFF) The extended attributes are inconsistent.
#define ERROR_WAIT_TIMEOUT							258 //(0x102) The wait operation timed out.
#define ERROR_NO_MORE_ITEMS							259 //(0x103) No more data is available.
#define ERROR_CANNOT_COPY							266 //(0x10A) The copy functions cannot be used.
#define ERROR_DIRECTORY								267 //(0x10B) The directory name is invalid.
#define ERROR_EAS_DIDNT_FIT							275 //(0x113) The extended attributes did not fit in the buffer.
#define ERROR_EA_FILE_CORRUPT						276 //(0x114) The extended attribute file on the mounted file system is corrupt.
#define ERROR_EA_TABLE_FULL							277 //(0x115) The extended attribute table file is full.
#define ERROR_INVALID_EA_HANDLE						278 //(0x116) The specified extended attribute handle is invalid.
#define ERROR_EAS_NOT_SUPPORTED						282 //(0x11A) The mounted file system does not support extended attributes.
#define ERROR_NOT_OWNER								288 //(0x120) Attempt to release mutex not owned by caller.
#define ERROR_TOO_MANY_POSTS						298 //(0x12A) Too many posts were made to a semaphore.
#define ERROR_PARTIAL_COPY							299 //(0x12B) Only part of a ReadProcessMemory or WriteProcessMemory request was completed.
#define ERROR_OPLOCK_NOT_GRANTED					300 //(0x12C) The oplock request is denied.
#define ERROR_INVALID_OPLOCK_PROTOCOL				301 //(0x12D) An invalid oplock acknowledgment was received by the system.
#define ERROR_DISK_TOO_FRAGMENTED					302 //(0x12E) The volume is too fragmented to complete this operation.
#define ERROR_DELETE_PENDING						303 //(0x12F) The file cannot be opened because it is in the process of being deleted.
#define ERROR_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING	304 //(0x130) Short name settings may not be changed on this volume due to the global registry setting.
#define ERROR_SHORT_NAMES_NOT_ENABLED_ON_VOLUME		305 //(0x131) Short names are not enabled on this volume.
#define ERROR_SECURITY_STREAM_IS_INCONSISTENT		306 //(0x132) The security stream for the given volume is in an inconsistent state. Please run CHKDSK on the volume.
#define ERROR_INVALID_LOCK_RANGE					307 //(0x133) A requested file lock operation cannot be processed due to an invalid byte range.
#define ERROR_IMAGE_SUBSYSTEM_NOT_PRESENT			308 //(0x134) The subsystem needed to support the image type is not present.
#define ERROR_NOTIFICATION_GUID_ALREADY_DEFINED		309 //(0x135) The specified file already has a notification GUID associated with it.
#define ERROR_INVALID_EXCEPTION_HANDLER				310 //(0x136) An invalid exception handler routine has been detected.
#define ERROR_DUPLICATE_PRIVILEGES					311 //(0x137) Duplicate privileges were specified for the token.
#define ERROR_NO_RANGES_PROCESSED					312 //(0x138) No ranges for the specified operation were able to be processed.
#define ERROR_NOT_ALLOWED_ON_SYSTEM_FILE			313 //(0x139) Operation is not allowed on a file system internal file.
#define ERROR_DISK_RESOURCES_EXHAUSTED				314 //(0x13A) The physical resources of this disk have been exhausted.
#define ERROR_INVALID_TOKEN							315 //(0x13B) The token representing the data is invalid.
#define ERROR_DEVICE_FEATURE_NOT_SUPPORTED			316 //(0x13C) The device does not support the command feature.
#define ERROR_MR_MID_NOT_FOUND						317 //(0x13D) The system cannot find message text for message number 0x%1 in the message file for %2.
#define ERROR_SCOPE_NOT_FOUND						318 //(0x13E) The scope specified was not found.
#define ERROR_UNDEFINED_SCOPE						319 //(0x13F) The Central Access Policy specified is not defined on the target machine.
#define ERROR_INVALID_CAP							320 //(0x140) The Central Access Policy obtained from Active Directory is invalid.
#define ERROR_DEVICE_UNREACHABLE					321 //(0x141) The device is unreachable.
#define ERROR_DEVICE_NO_RESOURCES					322 //(0x142) The target device has insufficient resources to complete the operation.
#define ERROR_DATA_CHECKSUM_ERROR					323 //(0x143) A data integrity checksum error occurred. Data in the file stream is corrupt.
#define ERROR_INTERMIXED_KERNEL_EA_OPERATION		324 //(0x144) An attempt was made to modify both a KERNEL and normal Extended Attribute (EA) in the same operation.
#define ERROR_FILE_LEVEL_TRIM_NOT_SUPPORTED			326 //(0x146) Device does not support file-level TRIM.
#define ERROR_OFFSET_ALIGNMENT_VIOLATION			327 //(0x147) The command specified a data offset that does not align to the device's granularity/alignment.
#define ERROR_INVALID_FIELD_IN_PARAMETER_LIST		328 //(0x148) The command specified an invalid field in its parameter list.
#define ERROR_OPERATION_IN_PROGRESS					329 //(0x149) An operation is currently in progress with the device.
#define ERROR_BAD_DEVICE_PATH						330 //(0x14A) An attempt was made to send down the command via an invalid path to the target device.
#define ERROR_TOO_MANY_DESCRIPTORS					331 //(0x14B) The command specified a number of descriptors that exceeded the maximum supported by the device.
#define ERROR_SCRUB_DATA_DISABLED					332 //(0x14C) Scrub is disabled on the specified file.
#define ERROR_NOT_REDUNDANT_STORAGE					333 //(0x14D) The storage device does not provide redundancy.
#define ERROR_RESIDENT_FILE_NOT_SUPPORTED			334 //(0x14E) An operation is not supported on a resident file.
#define ERROR_COMPRESSED_FILE_NOT_SUPPORTED			335 //(0x14F) An operation is not supported on a compressed file.
#define ERROR_DIRECTORY_NOT_SUPPORTED				336 //(0x150) An operation is not supported on a directory.
#define ERROR_NOT_READ_FROM_COPY					337 //(0x151) The specified copy of the requested data could not be read.
#define ERROR_FAIL_NOACTION_REBOOT					350 //(0x15E) No action was taken as a system reboot is required.
#define ERROR_FAIL_SHUTDOWN							351 //(0x15F) The shutdown operation failed.
#define ERROR_FAIL_RESTART							352 //(0x160) The restart operation failed.
#define ERROR_MAX_SESSIONS_REACHED					353 //(0x161) The maximum number of sessions has been reached.
#define ERROR_THREAD_MODE_ALREADY_BACKGROUND		400 //(0x190) The thread is already in background processing mode.
#define ERROR_THREAD_MODE_NOT_BACKGROUND			401 //(0x191) The thread is not in background processing mode.
#define ERROR_PROCESS_MODE_ALREADY_BACKGROUND		402 //(0x192) The process is already in background processing mode.
#define ERROR_PROCESS_MODE_NOT_BACKGROUND			403 //(0x193) The process is not in background processing mode.
#define ERROR_INVALID_ADDRESS						487 //(0x1E7) Attempt to access invalid address.

//
// MessageId: ERROR_FUNCTION_FAILED
//
// MessageText:
//
// Function failed during execution.
//
#define ERROR_FUNCTION_FAILED            			1627L // (0x65B) Function failed during execution.

#endif // _DAVICOM_COMMON_ERROR_

