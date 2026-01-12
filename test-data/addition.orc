( [1:0:newline] 

 [3:0:comment] # Start pyc-objdump output [3:26:newline] 

 [5:0:dir::set] .set [5:4:blank]   [5:5:version_pyvm] version_pyvm [5:17:blank] 	  [5:19:integer::dec] 62211 [5:24:newline] 
 [6:0:dir::set] .set [6:4:blank]   [6:5:flags] flags [6:10:blank] 		  [6:13:integer::hex] 0x00000040 [6:23:newline] 
 [7:0:dir::set] .set [7:4:blank]   [7:5:filename] filename [7:13:blank] 		  [7:16:string] "addition.py" [7:29:newline] 
 [8:0:dir::set] .set [8:4:blank]   [8:5:name] name [8:9:blank] 		  [8:12:string] "<module>" [8:22:newline] 
 [9:0:dir::set] .set [9:4:blank]   [9:5:stack_size] stack_size [9:15:blank] 		  [9:18:integer::dec] 2 [9:19:newline] 
 [10:0:dir::set] .set [10:4:blank]   [10:5:arg_count] arg_count [10:14:blank] 		  [10:17:integer::dec] 0 [10:18:newline] 

 [12:0:dir::interned] .interned [12:9:newline] 
 [13:0:blank] 	 [13:1:string] "a" [13:4:newline] 
 [14:0:blank] 	 [14:1:string] "b" [14:4:newline] 
 [15:0:blank] 	 [15:1:string] "c" [15:4:newline] 
 [16:0:blank] 	 [16:1:string] "<module>" [16:11:newline] 

 [18:0:dir::consts] .consts [18:7:newline] 
 [19:0:blank] 	 [19:1:integer::dec] 1 [19:2:newline] 
 [20:0:blank] 	 [20:1:integer::dec] 2 [20:2:newline] 
 [21:0:blank] 	 [21:1:pycst::None] None [21:5:newline] 

 [23:0:dir::names] .names [23:6:newline] 
 [24:0:blank] 	 [24:1:string] "a" [24:4:newline] 
 [25:0:blank] 	 [25:1:string] "b" [25:4:newline] 
 [26:0:blank] 	 [26:1:string] "c" [26:4:newline] 

 [28:0:dir::text] .text [28:5:newline] 
 [29:0:dir::line] .line [29:5:blank]   [29:6:integer::dec] 1 [29:7:newline] 
 [30:0:blank] 	 [30:1:insn::1::0x64] LOAD_CONST [30:11:blank]              [30:23:integer::dec] 0 [30:24:blank] 	 [30:25:comment] # 1 [30:28:newline] 
 [31:0:blank] 	 [31:1:insn::1::0x5a] STORE_NAME [31:11:blank]              [31:23:integer::dec] 0 [31:24:blank] 	 [31:25:comment] # "a" [31:30:newline] 
 [32:0:dir::line] .line [32:5:blank]   [32:6:integer::dec] 2 [32:7:newline] 
 [33:0:blank] 	 [33:1:insn::1::0x64] LOAD_CONST [33:11:blank]              [33:23:integer::dec] 1 [33:24:blank] 	 [33:25:comment] # 2 [33:28:newline] 
 [34:0:blank] 	 [34:1:insn::1::0x5a] STORE_NAME [34:11:blank]              [34:23:integer::dec] 1 [34:24:blank] 	 [34:25:comment] # "b" [34:30:newline] 
 [35:0:dir::line] .line [35:5:blank]   [35:6:integer::dec] 3 [35:7:newline] 
 [36:0:blank] 	 [36:1:insn::1::0x65] LOAD_NAME [36:10:blank]               [36:23:integer::dec] 0 [36:24:blank] 	 [36:25:comment] # "a" [36:30:newline] 
 [37:0:blank] 	 [37:1:insn::1::0x65] LOAD_NAME [37:10:blank]               [37:23:integer::dec] 1 [37:24:blank] 	 [37:25:comment] # "b" [37:30:newline] 
 [38:0:blank] 	 [38:1:insn::0::0x17] BINARY_ADD [38:11:blank]              [38:23:newline] 
 [39:0:blank] 	 [39:1:insn::1::0x5a] STORE_NAME [39:11:blank]              [39:23:integer::dec] 2 [39:24:blank] 	 [39:25:comment] # "c" [39:30:newline] 
 [40:0:dir::line] .line [40:5:blank]   [40:6:integer::dec] 4 [40:7:newline] 
 [41:0:blank] 	 [41:1:insn::1::0x65] LOAD_NAME [41:10:blank]               [41:23:integer::dec] 2 [41:24:blank] 	 [41:25:comment] # "c" [41:30:newline] 
 [42:0:blank] 	 [42:1:insn::0::0x47] PRINT_ITEM [42:11:blank]              [42:23:newline] 
 [43:0:blank] 	 [43:1:insn::0::0x48] PRINT_NEWLINE [43:14:blank]           [43:23:newline] 
 [44:0:blank] 	 [44:1:insn::1::0x64] LOAD_CONST [44:11:blank]              [44:23:integer::dec] 2 [44:24:blank] 	 [44:25:comment] # None [44:31:newline] 
 [45:0:blank] 	 [45:1:insn::0::0x53] RETURN_VALUE [45:13:blank]            [45:23:newline] 

 [47:0:comment] # Disassembled Thu Jan 16 11:38:59 2025 [47:39:newline] 

 )
