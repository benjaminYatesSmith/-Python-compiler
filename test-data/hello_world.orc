( [1:0:newline] 

 [3:0:comment] # Start pyc-objdump output [3:26:newline] 

 [5:0:dir::set] .set [5:4:blank]   [5:5:version_pyvm] version_pyvm [5:17:blank] 	  [5:19:integer::dec] 62211 [5:24:newline] 
 [6:0:dir::set] .set [6:4:blank]   [6:5:flags] flags [6:10:blank] 		  [6:13:integer::hex] 0x00000040 [6:23:newline] 
 [7:0:dir::set] .set [7:4:blank]   [7:5:filename] filename [7:13:blank] 		  [7:16:string] "hello_world.py" [7:32:newline] 
 [8:0:dir::set] .set [8:4:blank]   [8:5:name] name [8:9:blank] 		  [8:12:string] "<module>" [8:22:newline] 
 [9:0:dir::set] .set [9:4:blank]   [9:5:stack_size] stack_size [9:15:blank] 		  [9:18:integer::dec] 1 [9:19:newline] 
 [10:0:dir::set] .set [10:4:blank]   [10:5:arg_count] arg_count [10:14:blank] 		  [10:17:integer::dec] 0 [10:18:newline] 

 [12:0:dir::interned] .interned [12:9:newline] 
 [13:0:blank] 	 [13:1:string] "<module>" [13:11:newline] 
 [14:0:blank] 	 [14:1:string] "" [14:3:newline] 

 [16:0:dir::consts] .consts [16:7:newline] 
 [17:0:blank] 	 [17:1:string] "Hello, world!" [17:16:newline] 
 [18:0:blank] 	 [18:1:pycst::None] None [18:5:newline] 

 [20:0:dir::text] .text [20:5:newline] 
 [21:0:dir::line] .line [21:5:blank]   [21:6:integer::dec] 1 [21:7:newline] 
 [22:0:blank] 	 [22:1:insn::1::0x64] LOAD_CONST [22:11:blank]              [22:23:integer::dec] 0 [22:24:blank] 	 [22:25:comment] # "Hello, world!" [22:42:newline] 
 [23:0:blank] 	 [23:1:insn::0::0x47] PRINT_ITEM [23:11:blank]              [23:23:newline] 
 [24:0:blank] 	 [24:1:insn::0::0x48] PRINT_NEWLINE [24:14:blank]           [24:23:newline] 
 [25:0:blank] 	 [25:1:insn::1::0x64] LOAD_CONST [25:11:blank]              [25:23:integer::dec] 1 [25:24:blank] 	 [25:25:comment] # None [25:31:newline] 
 [26:0:blank] 	 [26:1:insn::0::0x53] RETURN_VALUE [26:13:blank]            [26:23:newline] 

 [28:0:comment] # Disassembled Thu Jan 16 11:40:32 2025 [28:39:newline] 

 )
