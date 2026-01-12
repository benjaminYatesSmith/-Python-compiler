

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

.consts
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

