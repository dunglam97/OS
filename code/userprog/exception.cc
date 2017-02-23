// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"
#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void ExceptionHandler(ExceptionType which)
{
	DebugInit("f");
    int type = machine->ReadRegister(2);
 	switch(which){
    	case NoException:
    		return;
    	case SyscallException:
		 switch (type){
			case SC_Halt:
				 DEBUG('a', "\n Shutdown, initiated by user program.");
				 printf ("\n\n Shutdown, initiated by user program.");
				 interrupt->Halt();
				 break;
			case SC_Create:
			{
				int virtAddr;
				char* filename;
				DEBUG('a',"\n SC_Create call ...");
				DEBUG('a',"\n Reading virtual address of filename");
				// Lấy tham số tên tập tin từ thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				DEBUG ('a',"\n Reading filename.");
				// MaxFileLength là = 32
				filename = machine->User2System(virtAddr,MaxFileLength+1);
				if (filename == NULL)
				{
				 printf("\n Not enough memory in system");
				 DEBUG('a',"\n Not enough memory in system");
				 machine->WriteRegister(2,-1); // trả về lỗi cho chương
				 // trình người dùng
				 delete[] filename;
				 return;
				}
				DEBUG('a',"\n Finish reading filename.");
				DEBUG('a',"\n File name : %s",filename);
				// Create file with size = 0
				// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
				// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
				// hành Linux, chúng ta không quản ly trực tiếp các block trên
				//đĩa cứng cấp phát cho file, việc quản ly các block của file
				// trên ổ đĩa là một đồ án khác
				if (!fileSystem->Create(filename,0))
				{
				 printf("\n Error create file '%s'",filename);
				 machine->WriteRegister(2,-1);
				 delete[] filename;
				 return;
				}
				machine->WriteRegister(2,0); // trả về cho chương trình
				 // người dùng thành công
				DEBUG('a',"\n File Created");
				delete[] filename;
			}
				break;
			case SC_PrintStr:
			{
				DEBUG('a',"\n SC_PrintStr call");
				int addr;
				addr = machine->ReadRegister(4);
				int i = 0;
				char* buffer = machine->User2System(addr,LIMIT);
				while(buffer[i] != 0){
					gSynchConsole->Write(buffer+i,1);
					++i;
				}
				buffer[i] = '\n';
				gSynchConsole->Write(buffer+i,1);
				delete[] buffer;
			}
			break;
			case SC_Open:
			{
				DEBUG('f',"\n SC_Open system calls");
				int type = machine->ReadRegister(5);
				if(type != 0 && type != 1){
					printf("Invalid File Type Access\n");
				}
				int virtAddr = machine->ReadRegister(4);
				char* filename = machine->User2System(virtAddr,MaxFileLength+1);
				if(filename == NULL){
					DEBUG('f',"\n Kernel Space out of memory");
					machine->WriteRegister(2,-1);
					delete[] filename;
					return;
				}
				OpenFile* file = fileSystem->Open(filename);
				if(file == NULL){
					DEBUG('f',"\n File Cannot Open");
					machine->WriteRegister(2,-1);
					delete[] filename;
				}


				delete[] filename;		

			}			
			default:
				printf("\n Unexpected user mode exception (%d %d)", which,type);
				interrupt->Halt();
			}
	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg,pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg,pc);
	pc+=4;
	machine->WriteRegister(NextPCReg,pc);
	}
   
}
