

# Start pyc-objdump output

.set version_pyvm	 62211
.set flags		 0x00000040
.set filename		 "second_deg.py"
.set name		 "<module>"
.set stack_size		 2
.set arg_count		 0

.interned
	"format"
	"math"
	"sqrt"
	"a"
	"b"
	"c"
	"discriminant"
	"x"
	"x1"
	"x2"
	"second_deg"
	"<module>"


	-1
	None
.code_start 3
.set version_pyvm	 62211
.set flags		 0x00000043
.set filename		 "second_deg.py"
.set name		 "second_deg"
.set stack_size		 3
.set arg_count		 3

.consts
	None
	2
	4
	0
	"Pas de solution réelle"
	"Une solution réelle : x = {:.2f}."
	"Deux solutions réelles : x1 = {:.2f} and x2 = {:.2f}."

.names
	"format"
	"math"
	"sqrt"

.varnames
	"a"
	"b"
	"c"
	"discriminant"
	"x"
	"x1"
	"x2"

.text
.line 5
	LOAD_FAST             1	# "b"
	LOAD_CONST            1	# 2
	BINARY_POWER          
	LOAD_CONST            2	# 4
	LOAD_FAST             0	# "a"
	BINARY_MULTIPLY       
	LOAD_FAST             2	# "c"
	BINARY_MULTIPLY       
	BINARY_SUBTRACT       
	STORE_FAST            3	# "discriminant"
.line 8
	LOAD_FAST             3	# "discriminant"
	LOAD_CONST            3	# 0
	COMPARE_OP            0	# "<"
	POP_JUMP_IF_FALSE     label_0
.line 9
	LOAD_CONST            4	# "Pas de solution réelle"
	RETURN_VALUE          
.line 10
label_0:
	LOAD_FAST             3	# "discriminant"
	LOAD_CONST            3	# 0
	COMPARE_OP            2	# "=="
	POP_JUMP_IF_FALSE     label_1
.line 11
	LOAD_FAST             1	# "b"
	UNARY_NEGATIVE        
	LOAD_CONST            1	# 2
	LOAD_FAST             0	# "a"
	BINARY_MULTIPLY       
	BINARY_DIVIDE         
	STORE_FAST            4	# "x"
.line 12
	LOAD_CONST            5	# "Une solution réelle : x = {:.2f}."
	LOAD_ATTR             0
	LOAD_FAST             4	# "x"
	CALL_FUNCTION         1
	RETURN_VALUE          
.line 14
label_1:
	LOAD_FAST             1	# "b"
	UNARY_NEGATIVE        
	LOAD_GLOBAL           1
	LOAD_ATTR             2
	LOAD_FAST             3	# "discriminant"
	CALL_FUNCTION         1
	BINARY_ADD            
	LOAD_CONST            1	# 2
	LOAD_FAST             0	# "a"
	BINARY_MULTIPLY       
	BINARY_DIVIDE         
	STORE_FAST            5	# "x1"
.line 15
	LOAD_FAST             1	# "b"
	UNARY_NEGATIVE        
	LOAD_GLOBAL           1
	LOAD_ATTR             2
	LOAD_FAST             3	# "discriminant"
	CALL_FUNCTION         1
	BINARY_SUBTRACT       
	LOAD_CONST            1	# 2
	LOAD_FAST             0	# "a"
	BINARY_MULTIPLY       
	BINARY_DIVIDE         
	STORE_FAST            6	# "x2"
.line 16
	LOAD_CONST            6	# "Deux solutions réelles : x1 = {:.2f} and x2 = {:.2f}."
	LOAD_ATTR             0
	LOAD_FAST             5	# "x1"
	LOAD_FAST             6	# "x2"
	CALL_FUNCTION         2
	RETURN_VALUE          
	LOAD_CONST            0	# None
	RETURN_VALUE          
.code_end

.names
	"math"
	"second_deg"

.text
.line 1
	LOAD_CONST            0	# -1
	LOAD_CONST            1	# None
	IMPORT_NAME           0
	STORE_NAME            0	# "math"
.line 3
	LOAD_CONST            2	# <code:"second_deg">
	MAKE_FUNCTION         0
	STORE_NAME            1	# "second_deg"
	LOAD_CONST            1	# None
	RETURN_VALUE          

# Disassembled Mon Jan 13 13:59:20 2025

